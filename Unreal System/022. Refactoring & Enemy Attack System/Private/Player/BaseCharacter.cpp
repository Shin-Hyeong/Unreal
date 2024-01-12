#include "Player/BaseCharacter.h"

// Box Collision�� ����ϱ� ����
#include "Components/BoxComponent.h"
// Weapon�� ������ �ִ� Equip()�� ȣ���ϱ� ����
#include "Actor/Weapons/Weapon.h"
// Enemy�� �Ӽ��� �߰��ϱ� ����
// ����ڰ� ���� Components ������ AttributesComponent.h
#include "Components/AttributeComponent.h"
// MetaSound�� �����Ű�� ����, Cascade System Particle�� ����ϱ� ����
#include "Kismet/GameplayStatics.h"
// AnimMontage�� ����ϱ� ����
#include "Animation/AnimMontage.h"
// CapsuleComponent�� �ǵ�� ����
#include "Components/CapsuleComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	// Capsule�� ���ؼ� CameraBoom�� �浹�� ���ý��� ���εǴ� ���� ����.
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

void ABaseCharacter::Attack()
{
	
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::Die()
{

}

// �ǰݵǴ� ������ ���ϴ� �Լ�
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	// Actor�� ���� Vector�� ����.
	// GetActorForwardVector() : Actor�� ���� ���͸� �������ͷ� ��ȯ��.
	const FVector Forward = GetActorForwardVector();

	// Forward�� ToHit�� ������ ���� �� ���� ������ �̷絵�� ����� ���ؼ�
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	// Actor�� ImpactPoint Vector�� ���� �� ����ȭ(���� ����) ��Ŵ.
	// ().GetSafeNormal() : �ش� FVector�� �����ϰ� ����ȭ���� �������ͷ� ����.
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// DotProduct() : �� ������ ������ �� ���ϱ�, double�� ��ȯ��.
	// Forward * ToHit = |Forward| * |Tohit| * cos(Theta)
	// |Forward| = 1, |ToHit| = 1
	// Forward * ToHit = cos(Theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	// cos(Theta) = ���� / �غ�
	// Arccos(���� / �غ�) = Theta
	// Theta = Actor�� ���� ���Ϳ� ImpactPoint���� ���� = ���� �������� �浹�� �Ͼ ����
	double Theta = FMath::Acos(CosTheta);

	// ������ Radians �������� ���������� degrees�� ��ȯ�����־���Ѵ�.
	// Radians : ������ �������������� ȣ�� ���̸� ���ϱ� ������ ���� ������ ����
	// degrees : �츮�� ���Ǵ� ���� ���� 
	Theta = FMath::RadiansToDegrees(Theta);

	// �𸮾��� �޼� ��Ģ���� ����ϱ� ������ ToHit ���Ͱ� Forward ���� �������� �����ʿ� �ִٸ�
	// CrossProduct�� Z�� ����̴�.
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	// CrossProduct�� Z�� �����̸� -> ���ʿ��� ����. 
	if (CrossProduct.Z < 0)
	{
		// Theta�� ���� ������ ����.
		Theta *= -1;
	}

	// �⺻ Section ��
	FName Section = "FromBack";

	// AnimMontage Section�� ���ϴ� ����
	// Theta�� ���� ������ ���
	if (Theta >= -45.f && Theta < 45.0f)
	{
		Section = FName("FromFront");
	}
	// Theta�� ������ ������ ���
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta > 45.0f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);


	// Theta�� ���
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Blue, FString::Printf(TEXT("Theta: %f"), Theta));
	}

	// �浹 ������ ǥ���ϴ� Arrow
	// DirectionalDebugArrow(CrossProduct, Forward, ToHit);
}

// �ǰ� ������ ���
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
	// ������ Capsule Collision ��Ȱ��ȭ
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// �ǰݵ� ���⿡ �´� Montage ���
void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

// ���� ��� ��� & ����� Index ��ȣ ��ȯ
int32 ABaseCharacter::PlayAttackMontage()
{
	// �������� AttackMontage�� ����ϰ� ����� Index�� ��ȯ��.
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

// ��� ��� ��� & ����� Index ��ȣ ��ȯ
int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

// �ǰݵǴ� ������ �׷��ִ� �Լ�
void ABaseCharacter::DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit)
{
	// Actor�� Cross Product ���͸� �׷��ִ� Debug Arrow
	UKismetSystemLibrary::DrawDebugArrow(
		this, // UObject* WorldContextObject
		GetActorLocation(), // const FVector LineStart,
		GetActorLocation() + CrossProduct * 100.f, // const FVector LineEnd
		8.0f, // float ArrowSize
		FColor::Blue, // FLinearColor LineColor
		5.0f, // float Duration = 0.f
		2.0f // float Thickness = 0.f
	);

	// Actor�� ���� ���͸� �׷��ִ� Debug Arrow
	UKismetSystemLibrary::DrawDebugArrow(
		this, // UObject* WorldContextObject
		GetActorLocation(), // const FVector LineStart,
		GetActorLocation() + Forward * 60.f, // const FVector LineEnd
		8.0f, // float ArrowSize
		FColor::Red, // FLinearColor LineColor
		5.0f, // float Duration = 0.f
		2.0f // float Thickness = 0.f
	);

	// Actor�� ToHit ���͸� �׷��ִ� Debug Arrow
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

// ABP���� Attack�� ���� �� notify�� ���ؼ�  AttackEnd �Լ��� ȣ���ϵ��� ��
void ABaseCharacter::AttackEnd()
{

}

// ������ Box Collision�� ��Ȱ��ȭ/Ȱ��ȭ ��Ű�� �Լ�
// ECollisionEnabled::Type : �浹 Ȱ��ȭ ����(Query Only, NoCollision, ...)�� 
// �Ű������� ���� �� �ְ� ECollisionEnabled Enum�� ȣ�� ����
void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		// Player�� ������ �ִ� ������ BoxColiison�� �浹 ���¸� ������
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		// �浹 ������ Actors �迭�� ���.
		EquippedWeapon->IgnoreActors.Empty();
	}
}

// Montage Section�� ���
void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

// Attack / HitReact �� �پ��� Montage�� ��ɿ� �°� ���� ����� �� �ֵ��� �ϴ� �Լ�
int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName)
{
	if (SectionName.Num() <= 0) return -1;

	const int32 MaxSectionIndex = SectionName.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionName[Selection]);

	return Selection;
}