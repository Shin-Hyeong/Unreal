#include "Items/PickupItem.h"
#include "Components/SphereComponent.h"

#include "Items/BaseItem.h"
#include "Character/AcquisitionInterface.h"
#include "Player/PlayableCharacter.h"

APickupItem::APickupItem()
{
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}

void APickupItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayableCharacter* Player = Cast<APlayableCharacter>(OtherActor);

	if (Player)
	{
		AcquisitionPlaySound();
		Destroy();
	}
}
