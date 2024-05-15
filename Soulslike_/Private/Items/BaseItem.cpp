#include "Items/BaseItem.h"

#include "Components/SphereComponent.h"
#include "Components/MeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"

#include "Character/AcquisitionInterface.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	StaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(StaticMesh);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Sphere->SetSphereRadius(SphereRadius);

	ItemAroundEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemAroundEffect->SetupAttachment(GetRootComponent());
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsHovering())
	{
		SineMove(DeltaTime);
	}
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnSphereEndOverlap);
}

void ABaseItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IAcquisitionInterface* Interface = Cast<IAcquisitionInterface>(OtherActor);
	if (Interface)
	{
		Interface->SetOverlappingItem(this);
	}
}

void ABaseItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IAcquisitionInterface* Interface = Cast<IAcquisitionInterface>(OtherActor);
	if (Interface)
	{
		Interface->SetOverlappingItem(nullptr);
	}
}

void ABaseItem::DeactivateEmbers()
{
	if (ItemAroundEffect)
	{
		ItemAroundEffect->Deactivate();
	}
}

void ABaseItem::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABaseItem::AcquisitionPlaySound()
{
	if (AcquistionSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, AcquistionSound, GetActorLocation());
	}
}

void ABaseItem::SineMove(float DeltaTime)
{
	RunningTime += DeltaTime;

	float DeltaZ = Amplitude * FMath::Sin(RunningTime * SineSpeed);
	AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
}

bool ABaseItem::IsHovering()
{
	return ItemState == EItemState::EIS_Hovering;
}


