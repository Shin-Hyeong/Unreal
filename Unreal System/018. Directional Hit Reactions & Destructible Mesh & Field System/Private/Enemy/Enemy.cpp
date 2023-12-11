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

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	// 충돌 지점 표시
	DrawDebugSphere(GetWorld(), ImpactPoint, 12.0f, 12, FColor::Yellow, false, 5.0f);

	// 충돌 방향을 구하고 방향에 맞는 AnimMontage 재생
	DirectionalHitReact(ImpactPoint);

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

	DirectionalDebugArrow(CrossProduct, Forward, ToHit);
}

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
