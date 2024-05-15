#include "Character/BaseCharacter.h"

#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

#include "Items/Weapon.h"
#include "Components/AttributesComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributesComponent>(TEXT("Attributes"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
}

void ABaseCharacter::GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType)
{
	FName HitDirection = GetHitDirectional(Hitter->GetActorLocation());
	if (IsAlive() && Hitter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Direction = %s"), *(HitDirection.ToString()));
		if (DamageType == 0)
			PlayMontageSection(HitSmallReactMontage, HitDirection);
		else if (DamageType == 1)
			PlayMontageSection(HitLargeReactMontage, HitDirection);
	}
	else
	{
		Die(HitDirection);
	}

	PlayHitSound(ImpactPoint);
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

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::SetWeaponBoxCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBoxComponent())
	{
		EquippedWeapon->GetWeaponBoxComponent()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

FName ABaseCharacter::GetHitDirectional(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// DotProduct() : 두 벡터의 내적의 곱 구하기, double를 반환함.
	// Forward * ToHit = |Forward| * |Tohit| * cos(Theta)
	// |Forward| = 1, |ToHit| = 1
	// Forward * ToHit = cos(Theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	// CrossProduct의 Z는 양수이다.
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	// CrossProduct의 Z가 음수이면 -> 왼쪽에서 때림. 
	if (CrossProduct.Z < 0)
	{
		Theta *= -1;
	}

	FName Section = "FromBack";

	if (Theta >= -45.f && Theta < 45.0f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta > 45.0f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	if (bShowDirectionalDebugArrow)
	{
		DirectionalDebugArrow(CrossProduct, Forward, ToHit);
	}

	return Section;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::Die(const FName& HitDirection)
{
	PlayDeathMontage(HitDirection);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::DisableCapusle()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ABaseCharacter::StopMontage(UAnimMontage* Montage)
{
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, Montage);
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayRMAttackMontage()
{
	// TODO
	return PlayRandomMontageSection(RMAttackMontage, AttackMontageSections);
}

void ABaseCharacter::AttackEnd()
{
	StopMontage(AttackMontage);
}

int32 ABaseCharacter::PlayDeathMontage(const FName& HitDirection)
{
	PlayMontageSection(DeathMontage, HitDirection);
	int32 Selection = GetDeathPoseSection(HitDirection);

	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_DeathPoseMax)
		DeathPose = Pose;

	return Selection;
}

int32 ABaseCharacter::GetDeathPoseSection(const FName& HitDirection)
{
	int32 Selection = 0;
	if (HitDirection == FName("FromFront"))
		Selection = 0;
	else if (HitDirection == FName("FromBack"))
		Selection = 1;
	else if (HitDirection == FName("FromRight"))
		Selection = 2;
	else if (HitDirection == FName("FromLeft"))
		Selection = 3;

	return Selection;
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName)
{
	if (SectionName.Num() <= 0) return -1;

	const int32 MaxSectionIndex = SectionName.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionName[Selection]);

	return Selection;
}


FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();
	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;
	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();
	return CombatTarget->GetActorLocation();
}

bool ABaseCharacter::IsJumping()
{
	bool IsFalling = GetCharacterMovement()->IsFalling();
	return IsFalling;
}

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