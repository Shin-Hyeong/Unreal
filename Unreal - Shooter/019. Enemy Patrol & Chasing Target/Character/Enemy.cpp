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

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 데미지 표시 위치 업데이트
	UpdateHitNumbers();
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EvenetInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(const FHitResult& HitResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}

	// 피격시 HealthBar UI 보여줌.
	ShowHealthBar();
	// 피격 모션 or 죽음 모션 재생
	PlayHitMontage(FName("HitRecatFront"));
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

	// Player의 총에 맞을 수 있도록(LineTrace에 감지되도록 함.) 함.
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// PatrolPoint를 BlackBoard에 Key값으로 설정
	SetPartolPoint();

	// Delegate
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
}

void AEnemy::Die()
{
	HideHealthBar();

	// TODO DieAnimation
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)  return;

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		// Black Board의 Target에 Key값을 넘김.
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			TEXT("Target"), 
			Character
		);
	}
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