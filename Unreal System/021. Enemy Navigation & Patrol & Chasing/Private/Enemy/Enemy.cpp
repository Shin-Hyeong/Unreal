#include "Enemy/Enemy.h"
#include "DrawDebugHelpers.h"
// debug arrow를 그리기 위해
#include "Kismet/KismetSystemLibrary.h"

// SkeletalMesh의 Collision를 건들기 위해
#include "Components/SkeletalMeshComponent.h"
// CapsuleComponent를 건들기 위해
#include "Components/CapsuleComponent.h"

// AnimMontage를 사용하기 위해
#include "Animation/AnimMontage.h"

// MetaSound를 재생시키기 위해, Cascade System Particle를 사용하기 위해
#include "Kismet/GameplayStatics.h"

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

	// Capsule에 의해서 CameraBoom이 충돌을 무시시켜 줌인되는 것을 막음.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

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

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
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
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 15.0f );
		}
	}

	// PawnSensing Delegate Binding
	if (PawnSensing)
	{	
		// OnSeePawn : PawnSensingComponent에 내장된 이벤트 함수
		// FSeePawnDelegate OnSeePawn : Delegate에 Dynamic Multicast 되도록 선언되어있음.
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// EnemyState가 Chasing이거나 Attacking인 상태일 때 true
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

void AEnemy::CheckCombatTarget()
{
	// CombatTarget이 CombatRadius보다 멀어지면 true
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		// 범위를 벗어나면 추격을 멈춤
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		// EnemyState를 Patrolling로 변경함
		EnemyState = EEnemyState::EES_Patrolling;
		// 이동속도를 다시 되돌림
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);

		UE_LOG(LogTemp, Warning, TEXT("Lose Interset"));
	}

	// CombatTarget이 AttackRadius보다 멀어지면 true
	// EnemyState가 Shasing이 아닌 상태면 true
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		// 공격 범위에서 벗어나면 다시 추격함
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
		MoveToTarget(CombatTarget);

		UE_LOG(LogTemp, Warning, TEXT("Chase Player"));
	}

	// CombatTarget이 AttackRadius보다 가까우면 true
	// EnemyState가 Attacking이 아닌 상태면 true
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		// 공격 범위 안에 들어오면 CombatTarget을 공격함.
		EnemyState = EEnemyState::EES_Attacking;
		// TODO : Attack Montage
		UE_LOG(LogTemp, Warning, TEXT("Attack to Player"));
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		// 랜덤 대기 시간
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);

		// 목적지를 다른 목적지로 설정.
		PatrolTarget = ChoosePatrolTarget();

		// 5초 이후 목적지로 이동시킴.
		// TimerManager 호출
		// SetTimer(FTimerManager &InOutHandle, AEnemy* InObj, void InTimerMethod(), float InRate, 
		// bool InbLoop = false, float InFirstDelay = -1.0f)
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	// Target이 존재하지 않으면 false
	if (Target == nullptr) return false;

	// Target과의 거리를 double 형태로 받음.
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	// Target 위치를 표시
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
	MoveRequest.SetAcceptanceRadius(15.0f); 

	// 목적지로 이동시킴.
	// MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr)
	EnemyController->MoveTo(MoveRequest);
}

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

// 0.5초마다 호출됨.
void AEnemy::PawnSeen(APawn* SeenPawn)
{	
	// 이미 추격중이면 바로 반환
	if (EnemyState == EEnemyState::EES_Chasing) return;

	// Cast<>()하는 것보다 Actor에 Tag를 설정하여 Tag만 확인하는 것이 비용이 싸다.
	// SeenPawn의 Tag가 Player이면 True
	if (SeenPawn->ActorHasTag(FName("Player")))
	{
		// 추격중에는 Timer가 작동되지 않도록 함
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		// 추격중에는 이동속도 증가
		GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
		// 감지된 Pawn을 전투상태로 설정.
		CombatTarget = SeenPawn;

		// 공격 범위 밖에서 확인 되었을 떄 추격하도록 함
		if (EnemyState != EEnemyState::EES_Attacking)
		{
			// EnemyState 상태 변경
			EnemyState = EEnemyState::EES_Chasing;
			// 목표물을 향해 추격
			MoveToTarget(CombatTarget);
			UE_LOG(LogTemp, Warning, TEXT("Pawn Seen, Chase Player"));
		}
	}
}

void AEnemy::PatrolTimerFinished()
{
	// 타이머가 종료되면 MoveToTarget를 콜백함.
	MoveToTarget(PatrolTarget);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 피격 Montage를 재생하는 함수
void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

// Death Montage 재생
void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		const int8 Selection = FMath::RandRange(0,5);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0 :
			SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Death1;
			break;
		case 1 :
			SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		case 2 :
			SectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Death3;
			break;
		case 3 :
			SectionName = FName("Death4");
			DeathPose = EDeathPose::EDP_Death4;
			break;
		case 4 :
			SectionName = FName("Death5");
			DeathPose = EDeathPose::EDP_Death5;
			break;
		case 5 :
			SectionName = FName("Death6");
			DeathPose = EDeathPose::EDP_Death6;
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}

	// 죽으면 바로 체력바 비활성화
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	// 죽으면 Capsule Collision 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 죽으면 Mesh Collision 비활성화
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 죽으면 일정 시간 이후 사라짐
	SetLifeSpan(5.0f);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	// 충돌 지점 표시
	// DrawDebugSphere(GetWorld(), ImpactPoint, 12.0f, 12, FColor::Yellow, false, 5.0f);

	// 처음 공격 받으면 체력 바 표시
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	// Health가 0 이상일 때
	if (Attributes && Attributes->IsAlive())
	{
		// 충돌 방향을 구하고 방향에 맞는 AnimMontage 재생
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}

	// 피격 사운드 재생
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, // const UObject* WorldContextObject
			HitSound, // USoundBase* Sound
			ImpactPoint // FVector Location
			// float VolumeMultiplier = 1.0f
			// float PitchMultiplier = 1.0f
			// float StartTime = 0.0f
			// USoundAttenuation* AttenuationSettings = nullptr
			// USoundConcurrency* ConcurrencySettings = nullptr
			// UInitialActiveSoundParam* InitialParams = nullptr
		);
	}

	// 피격 효과 재생
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), // UWorld* World
			HitParticles, // UParticleSystem* EmitterTemplate
			ImpactPoint  // FVector Location
			// FRotator Rotaion = ZeroRotator
			// FVector Scale = 1.0f
			// bool bAutoDestroty = true
			// EPSCPoolMethod PoolingMethod
		);
	}
}

// Apply Damage를 통해 반응함.
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget)
	{
		// Health - DamageAmount
		Attributes->ReceiveDamage(DamageAmount);

		// ProgressBar를 현재 Health의 Percent에 맞게 변경함
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	// 공격 받고 있는 대상을 지정함
	// EventInstgigator(Controller)가 조종하고 있는 Pawn을 가져옴.
	CombatTarget = EventInstigator->GetPawn();

	// 공격을 한 대상을 추격함
	EnemyState = EEnemyState::EES_Chasing;

	MoveToTarget(CombatTarget); 
	// -> EnemyState가 Chasing이고 CombatTarget이 지정되어 있기 떄문에
	// -> CheckCombatTarget()에 의해서 추격함.
	// -> AttackRadius 밖에서 때리면 따로 추격하도록 해줘야함.

	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;

	// 기술적으로 데미지를 반환함.
	return DamageAmount;
}

// 피격되는 방향을 구하는 함수
void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	// Actor의 전방 Vector를 구함.
	// GetActorForwardVector() : Actor의 전방 벡터를 단위벡터로 반환함.
	const FVector Forward = GetActorForwardVector();

	// Forward와 ToHit의 각도를 구할 때 땅과 수평을 이루도록 만들기 위해서
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	// Actor의 ImpactPoint Vector를 구한 뒤 정규화(단위 벡터) 시킴.
	// ().GetSafeNormal() : 해당 FVector를 안전하게 정규화시켜 단위벡터로 만듦.
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// DotProduct() : 두 벡터의 내적의 곱 구하기, double를 반환함.
	// Forward * ToHit = |Forward| * |Tohit| * cos(Theta)
	// |Forward| = 1, |ToHit| = 1
	// Forward * ToHit = cos(Theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	// cos(Theta) = 빗변 / 밑변
	// Arccos(빗변 / 밑변) = Theta
	// Theta = Actor의 전방 벡터와 ImpactPoint와의 각도 = 전방 기준으로 충돌이 일어난 각도
	double Theta = FMath::Acos(CosTheta);

	// 각도는 Radians 기준으로 구해짐으로 degrees로 변환시켜주어야한다.
	// Radians : 각도와 반지름을가지고 호의 길이를 구하기 쉽도록 만든 각도의 단위
	// degrees : 우리가 사용되는 각도 단위 
	Theta = FMath::RadiansToDegrees(Theta);

	// 언리얼은 왼손 법칙으로 사용하기 때문에 ToHit 벡터가 Forward 벡터 기준으로 오른쪽에 있다면
	// CrossProduct의 Z는 양수이다.
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	// CrossProduct의 Z가 음수이면 -> 왼쪽에서 때림. 
	if (CrossProduct.Z < 0)
	{
		// Theta의 값을 음수로 만듦.
		Theta *= -1;
	}

	// 기본 Section 값
	FName Section = "FromBack";

	// AnimMontage Section를 정하는 과정
	// Theta의 방향 전방일 경우
	if (Theta >= -45.f && Theta < 45.0f)
	{
		Section = FName("FromFront");
	}
	// Theta의 방향이 왼쪽일 경우
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta > 45.0f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);


	// Theta를 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Blue, FString::Printf(TEXT("Theta: %f"), Theta));
	}

	// 충돌 방향을 표시하는 Arrow
	// DirectionalDebugArrow(CrossProduct, Forward, ToHit);
}

// 피격되는 방향을 그려주는 함수
void AEnemy::DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit)
{
	// Actor의 Cross Product 벡터를 그려주는 Debug Arrow
	UKismetSystemLibrary::DrawDebugArrow(
		this, // UObject* WorldContextObject
		GetActorLocation(), // const FVector LineStart,
		GetActorLocation() + CrossProduct * 100.f, // const FVector LineEnd
		8.0f, // float ArrowSize
		FColor::Blue, // FLinearColor LineColor
		5.0f, // float Duration = 0.f
		2.0f // float Thickness = 0.f
	);

	// Actor의 전방 벡터를 그려주는 Debug Arrow
	UKismetSystemLibrary::DrawDebugArrow(
		this, // UObject* WorldContextObject
		GetActorLocation(), // const FVector LineStart,
		GetActorLocation() + Forward * 60.f, // const FVector LineEnd
		8.0f, // float ArrowSize
		FColor::Red, // FLinearColor LineColor
		5.0f, // float Duration = 0.f
		2.0f // float Thickness = 0.f
	);

	// Actor의 ToHit 벡터를 그려주는 Debug Arrow
	UKismetSystemLibrary::DrawDebugArrow(
		this, // UObject* WorldContextObject
		GetActorLocation(), // const FVector LineStart,
		GetActorLocation() + ToHit * 60.f, // const FVector LineEnd
		8.0f, // float ArrowSize
		FColor::Green, // FLinearColor LineColor
		5.0f, // float Duration = 0.f
		2.0f // float Thickness = 0.f
	);
}

