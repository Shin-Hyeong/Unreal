#include "Breakable/Breakalbe.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Items/Exp.h"
#include "Items/Weapon.h"


ABreakalbe::ABreakalbe()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SetRootComponent(GeometryCollection);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakalbe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakalbe::GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType)
{
	if (bBroken) return;
	bBroken = true;

	// SpawnWeapon이 참이면 무기 스폰
	SpawnWeapon ? SpawnDropWeapon() : SpawnDropExp();

	PlayBreakSound();
	DisableCapsuleCollision();

	SetLifeSpan(5.f);
}

void ABreakalbe::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakalbe::PlayBreakSound()
{
	if (BreakSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, BreakSound, GetActorLocation());
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}

void ABreakalbe::DisableCapsuleCollision()
{
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABreakalbe::SpawnDropExp()
{
	UWorld* World = GetWorld();
	if (World && ExpClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += DropItemLocationZ;
		const int32 Selection = FMath::RandRange(0, ExpClasses.Num() - 1);
		World->SpawnActor<AExp>(
			ExpClasses[Selection],
			Location,
			GetActorRotation()
		);
	}
}

void ABreakalbe::SpawnDropWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += DropItemLocationZ;
		const int32 Selection = FMath::RandRange(0, WeaponClasses.Num() - 1);
		World->SpawnActor<AWeapon>(
			WeaponClasses[Selection],
			Location,
			GetActorRotation()
		);
	}
}
