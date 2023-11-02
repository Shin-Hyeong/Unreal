#include "Fire.h"

AFire::AFire()
{
	PrimaryActorTick.bCanEverTick = false;

	FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire Particle"));
	RootComponent = FireParticle;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Fire(TEXT("/Game/StarterContent/Particles/P_Fire"));
	if (Fire.Succeeded() && Fire.Object != NULL)
	{
		FireParticle->SetTemplate(Fire.Object);
		// 파티클을 불어와서 슬로셍 저장함.
	}

	FireTime = 5;
}

void AFire::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AFire::Combustion, 1.0f,true);
}

void AFire::Combustion()
{
	--FireTime;
	if (FireTime <= 0)
	{
		OffFire();
	}
}

void AFire::OffFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	FireParticle->DeactivateSystem();
}
