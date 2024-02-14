#include "Actor/Soul.h"
#include "Interfaces/PickUpInterface.h"
#include "Kismet/KismetSystemLibrary.h"


void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const double LocationZ = GetActorLocation().Z;

	if (LocationZ > DesiredZ)
	{
		const FVector DeltaLocation = FVector(0.f, 0.f, DriftRate * DeltaTime);
		AddActorWorldOffset(DeltaLocation);
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, 2000.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1); 

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	FHitResult HitResult;
	
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this, // const UObject* WorldContextObject
		Start, // const FVector Start
		End, // const FVector End
		ObjectTypes, // const TArray<TEnumAsByte<EObjectTypeQuery>> &ObjectTypes
		false, // bool TraceComplex
		ActorsToIgnore, // const TArray<AActor* ,FDefaultAllocatior> &ActorsToIgnore
		ShowLineTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,// EDrawDebugTrace::Type DrawDebugType,
		HitResult, // FHitResult &OutHit,
		true // bool bIgnoreSelf,
		// FLinearColor TraceColor = FLineColor::Red,
		// FLinearColor TraceHitColor = FLineColor::Green,
		// float DrawTime = 5.0f
	);

	DesiredZ = HitResult.ImpactPoint.Z + SoulLocationZ;
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickUpInterface* Interface = Cast<IPickUpInterface>(OtherActor);

	if (Interface)
	{
		Interface->AddSouls(this);
		Destroy();
		SpawnPickupSystme();
		SpawnPickupSound();
	}
}
