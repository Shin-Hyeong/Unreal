#include "Character/Enemy.h"

// AI Controller
#include "Character/EnemyController.h"

// Particle && Sound
#include "Kismet/GameplayStatics.h"

// Particle
#include "Particles/ParticleSystemComponent.h"

// Widget
#include "Blueprint/UserWidget.h"

// MathLibrary
#include "Kismet/KismetMathLibrary.h"

// BlackboardComponent
#include "BehaviorTree/BlackboardComponent.h"

// SphreComponent
#include "Components/SphereComponent.h"

// ShooterCharacter
#include "Character/ShooterCharacter.h"

// BoxComponent
#include "Components/BoxComponent.h"

// Spawner
#include "Items/Spawner.h"

// MonsterWave
#include "Character/MonsterWave.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// AgroSpere 생성
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	// CombatRangeSphere 생성
	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	// Weapon Collision 생성
	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Weapon Box"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponSocket"));
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Rigth Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponSocket"));

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 데미지 표시 위치 업데이트
	UpdateHitNumbers();
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EvenetInstigator, AActor* DamageCauser)
{
	// 방어력 계산은 Player ApplyDamage에서 함.
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		
		AShooterCharacter* Player = Cast<AShooterCharacter>(DamageCauser);
		if (Player)
		{
			Player->AddCoin(Coin);
		}
		Die();
	}
	else
	{
		Health -= DamageAmount;

		// Blackboard의 Target Key값에 DamageCauser를 넣음 -> 추격함.
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsObject(
				TEXT("Target"),
				DamageCauser
			);
		}
	}

	if (bDead == true) return DamageAmount;

	// 피격시 HealthBar UI 보여줌.
	ShowHealthBar();
	// 피격 모션 or 죽음 모션 재생
	// PlayHitMontage(FName("HitRecatFront"));

	// 일정 확률로 기절 시킴
	const float Stunned = FMath::FRandRange(0.f, 1.f);
	if (Stunned <= StunChance)
	{
		PlayHitMontage(FName("HitRecatFront"));
		// Blackboard에 Stunned 값 전달
		SetStunned(true);
	}

	return DamageAmount;
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(const FHitResult& HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}
}

void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	// TMap에서 제거
	HitNumbers.Remove(HitNumber);

	HitNumber->RemoveFromParent();
}

void AEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };
		FVector2D ScreenPostion{};

		// HitNubmer가 ViewPort상에서 위치 알아내기
		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScreenPostion
		);

		// HitNumber Position 적용하기
		HitNumber->SetPositionInViewport(ScreenPostion);
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// AI Controller 가져옴
	EnemyController = Cast<AEnemyController>(GetController());

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Death"), false);
	}

	// Player의 총에 맞을 수 있도록(LineTrace에 감지되도록 함.) 함.
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// PatrolPoint를 BlackBoard에 Key값으로 설정
	SetPartolPoint();

	EnemyController->GetBlackboardComponent()->SetValueAsObject(
		TEXT("Target"),
		GetWorld()->GetFirstPlayerController()->GetPawn()
	);

	// Delegate
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);
}

void AEnemy::Die()
{
	if (bDead == true) return;
	HideHealthBar();
	bDead = true;

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UAnimInstance* EnemyInstance = GetMesh()->GetAnimInstance();
	if (EnemyInstance)
	{
		const int32 SectionIdx = FMath::RandRange(0, DeathMontageSection.Num() - 1);
		EnemyInstance->Montage_Play(DeathMontage);
		EnemyInstance->Montage_JumpToSection(DeathMontageSection[SectionIdx], DeathMontage);
	}

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(
			TEXT("Death"),
			true
		);
		EnemyController->StopMovement();
	}

	if (MonsterWave)
	{
		MonsterWave->SubSpawnCount(1);
	}
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(
			TEXT("Stunned"),
			bStunned
		);
	}
}

void AEnemy::SetInAttackRange(bool InAttackRange)
{
	bInAttackRange = InAttackRange;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(
			TEXT("InAttackRange"),
			bInAttackRange
		);
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), bCanAttack);
	}
}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::PlayHitMontage(const FName& Section, float PlayRate)
{
	if (bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && HitMontage)
		{

			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}
		
		bCanHitReact = false;
		const float HitReactTime{ FMath::FRandRange(HitReactTimeMin, HitReactTimeMax) };
		GetWorldTimerManager().ClearTimer(HitReactTimer);
		GetWorldTimerManager().SetTimer(HitReactTimer, 
			this, 
			&AEnemy::ResetHitReactTimer, 
			HitReactTime
		);
	}
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
	SetStunned(false);
}

void AEnemy::PlayAttackMontage(const FName& Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); 
	if (AnimInstance && AttackMontage) 
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
	}
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackWaitTimer, 
		this,
		&AEnemy::ResetCanAttack,
		AttackWaitTime
	);
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), bCanAttack);
	}
}

FName AEnemy::GetAttackSectionName() 
{
	if (AttackSectionName.Num() == 0) return FName();
	const int32 Section{ FMath::RandRange(0, AttackSectionName.Num() - 1) };
	return AttackSectionName[Section];
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(
		DeathTimer,
		this,
		&AEnemy::DestroyEnemy,
		DeathTime
	);
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

// BP에서 호출해서 사용함.
void AEnemy::ShowHealthBar_Implementation()
{
	// 다시 한대 맞으면 기존 Timer를 취소시키고 다시 Timer를 돌림.
	GetWorldTimerManager().ClearTimer(HealthBarTimer);

	GetWorldTimerManager().SetTimer(
		HealthBarTimer, 
		this, 
		&AEnemy::HideHealthBar, 
		HealthBarDisplayTime
	);
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	// Enemy.h에 생성하지 않은 이유는 각 Widget이 별도로 Timer를 갖게 하기 위함
	FTimerHandle HitNumberTimer;

	// Delegate를 통해 Timer에 사용할 함수에 매개변수를 넣을 수 있음.
	// DestroyHitNumber에 UUserWidget* HitNumber라는 매개변수를 넣기 위함.
	FTimerDelegate HitNumberDelegate;

	// 매체, Bind 함수, Bind함수에 사용되는 매개변수
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorld()->GetTimerManager().SetTimer(
		HitNumberTimer, 
		HitNumberDelegate,
		HitNumberDestoryTime, 
		false
	);
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	DoDamage(Character);
	StunCharacter(Character);
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor); 
	DoDamage(Character);
	StunCharacter(Character);
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)  return;

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (Character && EnemyController)
	{
		// Black Board의 Target에 Key값을 넘김.
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			TEXT("Target"),
			Character
		);
	}
}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterCharacter)
	{
		SetInAttackRange(true);
	}
}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterCharacter)
	{
		SetInAttackRange(false);
	}
}

void AEnemy::SetPartolPoint()
{
	// TransformLocation(FTransform, FVector)
	// : FTransform 기준으로 Localspace의 FVector를 Worldspace의 FVector를 반환함
	// 처음 Patrol 위치
	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint
	);

	// 두번째 Patrol 위치
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint2
	);

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);

		// BehaviorTree를 실행
		EnemyController->RunBehaviorTree(BehaviorTree);
	}
}

void AEnemy::DoDamage(AShooterCharacter* Victim)
{
	if (Victim == nullptr) return;
	if (Victim)
	{
		UGameplayStatics::ApplyDamage(
			Victim,
			BaseDamage - Victim->GetDefensive(),
			EnemyController,
			this,
			UDamageType::StaticClass()
		);

		if (Victim->GetMeleeImpactSound())
		{
			UGameplayStatics::PlaySoundAtLocation(this, 
				Victim->GetMeleeImpactSound(),
				GetActorLocation()
			);
		}
	}
}

void AEnemy::StunCharacter(AShooterCharacter* Victim)
{
	if (Victim == nullptr)  return;

	const float Stun{ FMath::FRandRange(0.f, 1.f) };
	if (Stun <= Victim->GetStunChane())
	{
		Victim->Stun();
	}
}
