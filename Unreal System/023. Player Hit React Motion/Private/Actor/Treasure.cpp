#include "Actor/Treasure.h"
// Overlap된 상대가 Player인지를 확인하기 위해서
#include "Player/PlayerCharacter.h"
// MetaSound를 재생시키기 위해, Cascade System Particle를 사용하기 위해
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Overlap 된 상대가 APlayerCharacter를 상속 받고 있는지 확인
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	
	if (Player)
	{
		// 아이템 획득 사운드 재생
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this, // const UObject* WorldContextObject
				PickupSound, // USoundBase* Sound
				GetActorLocation() // FVector Location
				// float VolumeMultiplier = 1.0f
				// float PitchMultiplier = 1.0f
				// float StartTime = 0.0f
				// USoundAttenuation* AttenuationSettings = nullptr
				// USoundConcurrency* ConcurrencySettings = nullptr
				// UInitialActiveSoundParam* InitialParams = nullptr
			);
		}

		Destroy();
	}
}

void ATreasure::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
