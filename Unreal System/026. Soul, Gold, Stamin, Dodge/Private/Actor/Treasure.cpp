#include "Actor/Treasure.h"
// Overlap된 상대가 Player인지를 확인하기 위해서
#include "Interfaces/PickUpInterface.h"


void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Overlap 된 상대가 APlayerCharacter를 상속 받고 있는지 확인
	IPickUpInterface* Player = Cast<IPickUpInterface>(OtherActor);
	
	if (Player)
	{
		// 아이템 획득 사운드 재생
		SpawnPickupSound();
		Player->AddGold(this);
		Destroy();
	}
}

void ATreasure::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
