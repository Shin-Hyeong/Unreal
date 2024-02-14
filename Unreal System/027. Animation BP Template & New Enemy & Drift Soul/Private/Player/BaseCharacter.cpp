#include "Player/BaseCharacter.h"

// Box Collision를 사용하기 위해
#include "Components/BoxComponent.h"
// Weapon이 가지고 있는 Equip()를 호출하기 위해
#include "Actor/Weapons/Weapon.h"
// Enemy에 속성을 추가하기 위해
// 사용자가 만든 Components 폴더의 AttributesComponent.h
#include "Components/AttributeComponent.h"
// MetaSound를 재생시키기 위해, Cascade System Particle를 사용하기 위해
#include "Kismet/GameplayStatics.h"
// AnimMontage를 사용하기 위해
#include "Animation/AnimMontage.h"
// CapsuleComponent를 건들기 위해
#include "Components/CapsuleComponent.h"
// Draw Debug 기능
#include "DrawDebugHelpers.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	// Capsule에 의해서 CameraBoom이 충돌을 무시시켜 줌인되는 것을 막음.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	// Health가 0 이상일 때
	if (IsAlive() && Hitter)
	{
		// 충돌 방향을 구하고 방향에 맞는 AnimMontage 재생
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		Die();
	}

	// 피격 사운드 재생
	PlayHitSound(ImpactPoint);

	// 피격 효과 재생
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

// 피격되는 방향을 구하는 함수
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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

// 피격 데미지 계산
void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		// Health - DamageAmount
		Attributes->ReceiveDamage(DamageAmount);
	}
} 

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
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
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
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

void ABaseCharacter::DisableCapsule()
{
	// 죽으면 Capsule Collision 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMehsCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::PlayDodgeMontage()
{
	if (DodgeMontage)
	{
		PlayMontageSection(DodgeMontage, FName("Default"));
	}
}

// 피격된 방향에 맞는 Montage 재생
void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

// 공격 모션 재생 & 재생한 Index 번호 반환
int32 ABaseCharacter::PlayAttackMontage()
{
	// 랜덤으로 AttackMontage를 재생하고 재생한 Index를 반환함.
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

// 사망 모션 재생 & 재생한 Index 번호 반환
int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	// 정수형인 Selection을 가지고 EDeathPose의 열거형 멤버를 가리킬수 있음.
	// Selection = 1 -> EDeathPose::Death1
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_Max)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// Montage_Stop(float InBlendOutTime, const UAnimMontage* Montage = 0);
		AnimInstance->Montage_Stop(0.25, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	// CombatTarget위치 + (바라보는 방향 * WarpTargetDistance)
	return CombatTargetLocation + TargetToMe;
}


FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

// ABP에서 Attack이 끝날 쯤 notify를 통해서  AttackEnd 함수를 호출하도록 함
void ABaseCharacter::AttackEnd()
{

}

void ABaseCharacter::DodgeEnd()
{

}

// 무기의 Box Collision을 비활성화/활성화 시키는 함수
// ECollisionEnabled::Type : 충돌 활성화 유형(Query Only, NoCollision, ...)을 
// 매개변수로 받을 수 있고 ECollisionEnabled Enum를 호출 가능
void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		// Player가 가지고 있는 무기의 BoxColiison의 충돌 상태를 변경함
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		// 충돌 무시할 Actors 배열을 비움.
		EquippedWeapon->IgnoreActors.Empty();
	}
}

// 피격되는 방향을 그려주는 함수
void ABaseCharacter::DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit)
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

// Montage Section을 재생
void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

// Attack / HitReact 등 다양한 Montage를 기능에 맞게 랜덤 재생할 수 있도록 하는 함수
int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName)
{
	if (SectionName.Num() <= 0) return -1;

	const int32 MaxSectionIndex = SectionName.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionName[Selection]);

	return Selection;
}