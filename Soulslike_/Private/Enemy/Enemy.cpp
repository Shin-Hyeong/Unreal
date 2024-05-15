#include "Enemy/Enemy.h"

#include "Animation/AnimMontage.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "Components/AttributesComponent.h"
#include "Character/BaseCharacter.h"
#include "Player/PlayableCharacter.h"
#include "Items/Weapon.h"
#include "Items/Exp.h"
#include "HUD/HealthBarComponent.h"

#include "DrawDebugHelpers.h"

AEnemy::AEnemy()
{
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 2000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;
	
	if (EnemyState > EEnemyState::EES_Patrolling)
		CheckCombatTarget();
	else
		CheckPatrolTarget();
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType)
{
	ShowHealthBar();

	// HitReact & Die & HitSound & HitParticle
	Super::GetHit(GetHitter, ImpactPoint, Hitter, DamageType);

	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponBoxCollision(ECollisionEnabled::NoCollision);
	StopMontage(AttackMontage);
	if (IsInsideAttackRadius())
	{
		if (!IsDead())
			StartAttackTimer();
	}
	if (IsDead())
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy Is Dead"));
		APlayableCharacter* Player = Cast<APlayableCharacter>(Hitter);
		Player->EnemyIsDead(this);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Attributes에 Damage만큼 뺌.
	HandleDamage(DamageAmount);

	CombatTarget = EventInstigator->GetPawn();

	if (IsInsideAttackRadius())
		EnemyState = EEnemyState::EES_Attacking;
	else
		ChaseTarget();

	return DamageAmount;
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("Enemy"));

	EnemyController = Cast<AAIController>(GetController());

	if (EnemyController && PatrolTarget)
	{
		StartPatrolling();
	}

	if (HealthBarWidget && Attributes)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		HealthBarWidget->SetVisibility(bShowStartHUD);
	}
	
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	SpawnEnemyWeapon();
}

void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;

	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack = IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();

	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::Die(const FName& HitDirection)
{
	Super::Die(HitDirection);
	EnemyState = EEnemyState::EES_Dead;
	UE_LOG(LogTemp, Log, TEXT("Set Dead State"));

	ClearPatrolTimer();
	ClearAttackTimer();

	GetCharacterMovement()->bOrientRotationToMovement = false;
	HideHealthBar();
	DisableCapusle();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SetWeaponBoxCollision(ECollisionEnabled::NoCollision);

	SpawnExp();
	SetLifeSpan(DeathLifeSpan);
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;

		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);

		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::CheckCombatTarget()
{
	// 너무 멀어져서 흥미 잃음
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();

		// 공격 모션 중이 아니면 다시 Patrol를 시작함. -> 공격 모션이 끝나면 Patrol
		if (!IsEngaged())
		{
			StartPatrolling();
			// UE_LOG(LogTemp, Warning, TEXT("Vampire loses insterest to Player, Start patrol again"));
		}
	}

	// 공격 범위 벗어나서 쫒아감
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();

		if (!IsEngaged())
		{
			ChaseTarget();
			// UE_LOG(LogTemp, Warning, TEXT("Player get out AttackRadius, Vampire chase to Player"));
		}
	}

	// 공격 범위 안에 있어서 공격함
	else if (CanAttack())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Play is Vampire's AttackRadius, Vampire start Attacking Player"));
		StartAttackTimer();
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::SpawnExp()
{
	UWorld* World = GetWorld();
	if (World && ExpClass && Attributes)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, ExpLocationZ);
		UE_LOG(LogTemp, Warning, TEXT("SpawnLocation : %f"), ExpLocationZ);
		AExp* SpawnExp = World->SpawnActor<AExp>(ExpClass, SpawnLocation, GetActorRotation());
		if (SpawnExp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Success Spawn"));
			SpawnExp->SetExp(Attributes->GetExp());
			SpawnExp->SetOwner(this);
		}
		else if (SpawnExp == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail Spawn"));
		}

	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		(EnemyState == EEnemyState::EES_Patrolling ||
			EnemyState == EEnemyState::EES_Chasing) &&
		SeenPawn->ActorHasTag(FName("Player"));

	const bool bPawnHasDead =
		SeenPawn->ActorHasTag(FName("Dead"));

	if (bShouldChaseTarget && bPawnHasDead == false)
	{
		ShowHealthBar();
		ClearPatrolTimer();
		CombatTarget = SeenPawn;
		ChaseTarget();
	}
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;

	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

void AEnemy::SpawnEnemyWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* EnemyWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		EnemyWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = EnemyWeapon;
	}
}