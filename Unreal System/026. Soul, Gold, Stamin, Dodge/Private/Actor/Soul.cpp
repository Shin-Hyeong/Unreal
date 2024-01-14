#include "Actor/Soul.h"
#include "Interfaces/PickUpInterface.h"


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
