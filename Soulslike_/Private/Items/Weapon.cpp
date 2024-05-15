#include "Items/Weapon.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

#include "Character/HitInterface.h"
#include "Character/BaseCharacter.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;

	AttachMeshToSocket(InParent, InSocketName);
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	DisableSphereCollision();
	DeactivateEmbers();
	if (NewOwner->ActorHasTag(FName("Player")))
		PlayEquipSound();

	WeaponOwner = Cast<ABaseCharacter>(GetOwner());
	if (WeaponOwner)
	{
		FString WeaponOwnerName = WeaponOwner->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Weapon Owener : %s"), *WeaponOwnerName);
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRulse(EAttachmentRule::SnapToTarget, true);
	StaticMesh->AttachToComponent(InParent, TransformRulse, InSocketName);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsEnemyType(OtherActor) || IsOwner(OtherActor)) return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
		if (ActorIsEnemyType(BoxHit.GetActor())) return;

		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(),
			Damage[WeaponOwner->GetDamageType()],
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		GetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
	}
}

void AWeapon::GetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner(), WeaponOwner->GetDamageType());
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(this);
	ActorToIgnore.Add(GetOwner());
	for (AActor* Actor : IgnoreActors)
	{
		ActorToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1, // Visibility
		false,
		ActorToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true,
		FColor::Red,
		FColor::Blue,
		5.0f
	);

	IgnoreActors.AddUnique(BoxHit.GetActor());
}

bool AWeapon::ActorIsEnemyType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));;
}

bool AWeapon::IsOwner(AActor* OtherActor)
{
	return OtherActor == GetOwner();
}
