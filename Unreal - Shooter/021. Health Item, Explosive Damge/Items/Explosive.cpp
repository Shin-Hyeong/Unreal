#include "Items/Explosive.h"

// Particle && Sound
#include "Kismet/GameplayStatics.h"

// Particle
#include "Particles/ParticleSystemComponent.h"

// SphereCompnent
#include "Components/SphereComponent.h"

// CharacterClass
#include "GameFramework/Character.h"

// UGameplayStatic
#include "Kismet/GameplayStatics.h"

AExplosive::AExplosive()
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
}

void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(const FHitResult& HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ExplosiveSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosiveSound, GetActorLocation());
	}
	if (ExplosiveEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveEffect, HitResult.Location, FRotator(0.f), true);
	}

	TArray<AActor*> OverlappingActors;
	// AActor에서 상속받는 함수 - Overlapped된 Actor들을 TArray형태로 얻을 수있다
	// Overlap된 ACharacter들의 TArray를 OverlappingActors에 넣어줌.
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (auto Actor : OverlappingActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor damage by explosive : %s"), *Actor->GetName());
		UGameplayStatics::ApplyDamage(
			Actor,
			Damage,
			ShooterController,
			Shooter,
			UDamageType::StaticClass()
		);
	}
	
	Destroy();
}

