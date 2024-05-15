#include "Items/Exp.h"

#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"

#include "Character/AcquisitionInterface.h"

void AExp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const double LocationZ = GetActorLocation().Z;

	if (LocationZ > DesiredZ)
	{
		const FVector DeltaLocation = FVector(0.f, 0.f, DriftRate * DeltaTime);
		AddActorWorldOffset(DeltaLocation);
	}
	if (LocationZ - 5.f < DesiredZ && ItemState == EItemState::EIS_NoState)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemState set Hovering"));
		ItemState = EItemState::EIS_Hovering;
	}
}

void AExp::BeginPlay()
{
	Super::BeginPlay();
	ItemState = EItemState::EIS_NoState;

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, 2000.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// ObjectTypeQurey2 = WorldStatic
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ObjectTypes,
		false,
		ActorsToIgnore,
		bShowLineTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true
	);

	DesiredZ = HitResult.ImpactPoint.Z + ExpLocationZ;
}

void AExp::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString OtherActorName = OtherActor->GetName();

	IAcquisitionInterface* Interface = Cast<IAcquisitionInterface>(OtherActor);

	if (Interface)
	{
		Interface->AddExp(this);
		Destroy();

		if (GetExpEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation
			(
				this,
				GetExpEffect,
				GetActorLocation()
			);
		}
	}

	
}
