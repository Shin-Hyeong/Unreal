 #include "Enemy/Enemy.h"
#include "DrawDebugHelpers.h"
// SkeletalMesh(GetMesh())의 Collision를 건들기 위해
#include "Components/SkeletalMeshComponent.h"

// AnimMontage를 사용하기 위해
#include "Animation/AnimMontage.h"

// Enemy에 속성을 추가하기 위해
// 사용자가 만든 Components 폴더의 AttributesComponent.h
#include "Components/AttributeComponent.h"

// Enemy의 체력을 표시하기 위해
#include "HUD/HealthBarComponent.h"

// Enemy의 이동관련을 위해
#include "GameFramework/CharacterMovementComponent.h"

// AI Contorll를 제어하기 위해 -> Build.cs의 모듈에 AIModulue 추가 필요
#include "AIController.h"

// 특정 범위 안에 있는 Pawn을 감지하기 위해 -> Build.cs의 모듈에 AIModulue 추가 필요
#include "Perception/PawnSensingComponent.h"

// Weapon을 장착(Equip)하기 위해
#include "Actor/Weapons/Weapon.h"

// debug arrow를 그리기 위해
#include "Kismet/KismetSystemLibrary.h"

// Soul를 소환하기 위해
#include "Actor/Soul.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Enemy의 Mesh가 Pawn으로 설정되어 무기의 BoxCollision이 무시되는것을 막음
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	// Enemy의 Mesh가 Visibility Channel를 충돌하게 만들어서 BoxTrace를 감지하도록 함.
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	// Enemy의 Mesh에 의해서 CameraBoom이 충돌을 무시시켜 줌인되는 것을 막음.
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// Enemy의 Mesh가 Trigger의 Overlap 이벤트가 일어나도록 설정함.
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// 이동하는 방향을 바라보도록 함.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// AI Controller에 의해 방향이 변하지 않도록 함.
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	// PawnSensingComponent
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	// 시야 범위 설정
	PawnSensing->SightRadius = 2000.0f;
	// 시야각 설정
	PawnSensing->SetPeripheralVisionAngle(45.0f);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 사망하면 추격하지 추격 및 공격하지 못하도록 함.
	if (IsDead()) return;

	// EnemyState가 Chasing이거나 Attacking, Engaged인 상태일 때 true
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		// CombatTarget과의 거리가 CombatRadius보다 멀어지면 CombatTarget를 nullptr하고 HealthBar를 안보이도록 함.
		CheckCombatTarget();
	}
	else
	{
		// PatrolTarget과의 거리가 PatrolRadius보다 가까워지면 다른 PatrolTarget를 설정하고 5초후 이동
		CheckPatrolTarget();
	}
}

// Apply Damage를 통해 반응함.
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	// 공격 받고 있는 대상을 지정함
	// EventInstgigator(Controller)가 조종하고 있는 Pawn을 가져옴.
	CombatTarget = EventInstigator->GetPawn();

	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else
	{
		ChaseTarget();
	}

	// 기술적으로 데미지를 반환함.
	return DamageAmount;
}

// Enemy가 삭제하면 자동으로 호출하여 무기도 삭제함
void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	// 충돌 지점 표시
	// DrawDebugSphere(GetWorld(), ImpactPoint, 12.0f, 12, FColor::Yellow, false, 5.0f);

	// 처음 공격 받으면 체력 바 표시
	ShowHealthBar();

	Super::GetHit_Implementation(ImpactPoint, Hitter);

	// Patrol 명령을 받은 후 피격 되면 Patrol 명령을 취소시킴.
	ClearPatrolTimer();
	// Attack 모션중 피격 되면 AttackTimer를 제거함.
	CleaerAttackTimer();
	// 무기의 BoxCollision을 NoCollision으로 변경함.
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	// AttackMontage가 진행중이였다면 Montage를 멈춤
	StopAttackMontage();
	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));

	/** BeginPlay는 일부로 리펙토링 안함*/

	if (HealthBarWidget && Attributes)
	{
		// 처음 설정된 체력에 맞는 Percent로 ProgressBar를 설정
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		// ProgressBar가 처음에는 안보이도록 설정
		HealthBarWidget->SetVisibility(false);
	}

	// 여러번 Cast하는 작업을 피하기 위해서 변수화 시킴.
	EnemyController = Cast<AAIController>(GetController());

	// 시작하면 Enemy를 이동시킬 위치 설정
	if (EnemyController && PatrolTarget)
	{
		FAIMoveRequest MoveRequest;
		// 이동 시킬 목표지점
		MoveRequest.SetGoalActor(PatrolTarget);
		// 목표지점으로 부터의 도착했다는 허용 반경
		MoveRequest.SetAcceptanceRadius(15.0f);

		// FNavPathSharedPtr에는 목적지까지의 탐색경로를 가지고 있음.
		FNavPathSharedPtr NavPath;

		// 목적지로 이동시킴.
		// MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr)
		EnemyController->MoveTo(MoveRequest, &NavPath);

		// GetPathPoint : 목적지까지의 경로
		// GetPathPoints는 TArray<FNavPathPoint>형태를 반환함.
		// FNavPathPoint : FNavLoction을 가지고 있음. -> 이동 경로를 그릴 수 있음
		// FNavLoction은 Location과 NodeRef를 가지고 있음.
		TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();

		// 이동경로 그리기
		for (auto& Point : PathPoints)
		{
			const FVector& Location = Point.Location;
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 15.0f);
		}
	}

	// PawnSensing Delegate Binding
	if (PawnSensing)
	{
		// OnSeePawn : PawnSensingComponent에 내장된 이벤트 함수
		// FSeePawnDelegate OnSeePawn : Delegate에 Dynamic Multicast 되도록 선언되어있음.
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	SpawnDefaultWeapon();
}

// Death Montage 재생
void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();

	// EnemyState를 Dead로 설정
	EnemyState = EEnemyState::EES_Dead;

	// Enemy의 AttackTimer를 제거
	CleaerAttackTimer();

	// 이동하고자 하는 방향을 바라보지 못하도록 함.
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// 죽으면 바로 체력바 비활성화
	HideHealthBar();

	// 죽으면 Capsule Collision 비활성화
	DisableCapsule();
	// 죽으면 Mesh Collision 비활성화
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	// 죽으면 무기의 Collision 비활성화
	SetWeaponCollision(ECollisionEnabled::NoCollision);

	// 죽으면 일정 시간 이후 사라짐
	SetLifeSpan(DeathLifeSpan);

	SpawnSoul();
}

void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr) return;

	EnemyState = EEnemyState::EES_Engaged;

	// 공격 모션 재생
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
	// 상태를 아무것도 아닌 상태로 변경
	EnemyState = EEnemyState::EES_NoState;
	// CombatTarget에 따라 대응
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthBarWidget)
	{
		// ProgressBar를 현재 Health의 Percent에 맞게 변경함
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
		// 랜덤 대기 시간
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);

		// 목적지를 다른 목적지로 설정.
		PatrolTarget = ChoosePatrolTarget();

		// 5초 이후 목적지로 이동시킴.
		// TimerManager 호출
		// SetTimer(FTimerManager &InOutHandle, AEnemy* InObj, void InTimerMethod(), float InRate, 
		// bool InbLoop = false, float InFirstDelay = -1.0f)
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	// CombatTarget이 CombatRadius보다 멀어지면 true
	if (IsOutsideCombatRadius())
	{
		CleaerAttackTimer();
		LoseInterest();

		// Engaged 상태가 아니면 Patrol를 시작함
		if (!IsEngaged())
		{
			StartPatrolling();
		}
	}

	// CombatTarget이 AttackRadius보다 멀어지면 true
	// EnemyState가 Shasing이 아닌 상태면 true
	else if (IsOutsideAttackRadius() && IsChasing())
	{
		CleaerAttackTimer();

		if (!IsEngaged())
		{
			ChaseTarget();
		}
	}

	// CombatTarget이 AttackRadius보다 가까우면 true
	// EnemyState가 Attacking이 아닌 상태면 true
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::PatrolTimerFinished()
{
	// 타이머가 종료되면 MoveToTarget를 콜백함.
	MoveToTarget(PatrolTarget);
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

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	// HealthBar를 안보이게 함.
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	// EnemyState를 Patrolling로 변경함
	EnemyState = EEnemyState::EES_Patrolling;
	// 이동속도를 다시 되돌림
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	// 공격 범위에서 벗어나면 다시 추격함
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
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
	return EnemyState != EEnemyState::EES_Chasing;
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

void AEnemy::ClearPatrolTimer()
{
	// 추격중에는 Timer가 작동되지 않도록 함
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	// Target이 존재하지 않으면 false
	if (Target == nullptr) return false;

	// Target과의 거리를 double 형태로 받음.
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	// Target 위치를 표시
	if (VisibilityTarget)
	DrawDebugSphere(GetWorld(), Target->GetTargetLocation(), 24.f, 12, FColor::Red, false, -1.f);

	// Target과의 거리가 Radius보다 작으면 true
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	// Controller와 Target이 없으면 실행 안함
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest; 
	// 이동 시킬 목표지점
	MoveRequest.SetGoalActor(Target);
	// 목적지 수용범위
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	// 목적지로 이동시킴.
	// MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr)
	EnemyController->MoveTo(MoveRequest);
}

// 다른 Target을 반환해주는 함수
AActor* AEnemy::ChoosePatrolTarget()
{
	// 동일한 목적지를 설정하지 못하도록 따로 배열을 만듦
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	// PatrolTarget과의 거리가 PatrolRadius보다 가까워지면 true
	const int32 NumPatroltargets = ValidTargets.Num();
	if (NumPatroltargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatroltargets - 1);

		return  ValidTargets[TargetSelection];
	}
	
	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	// Enemy가 들고 있을 무기 소환
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

// 0.5초마다 호출됨.
void AEnemy::PawnSeen(APawn* SeenPawn)
{	
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		// PatrolTimer 정지
		ClearPatrolTimer();
		// 감지된 Pawn을 전투상태로 설정.
		CombatTarget = SeenPawn;
		// CombatTarget를 추격함
		ChaseTarget();
	}
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::CleaerAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, SoulLocationZ);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
			SpawnedSoul->SetOwner(this);
		}
	}
}