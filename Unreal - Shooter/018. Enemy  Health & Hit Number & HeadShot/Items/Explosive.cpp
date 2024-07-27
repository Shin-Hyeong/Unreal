#include "Items/Explosive.h"

// Particle && Sound
#include "Kismet/GameplayStatics.h"

// Particle
#include "Particles/ParticleSystemComponent.h"

AExplosive::AExplosive()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(const FHitResult& HitResult)
{
	if (ExplosiveSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosiveSound, GetActorLocation());
	}
	if (ExplosiveEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveEffect, HitResult.Location, FRotator(0.f), true);
	}

	// TODO : Apply explosive Damage

	
	Destroy();
}

