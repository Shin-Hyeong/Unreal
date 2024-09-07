#include "Items/Spawner.h"

// BoxComponent
#include "Components/BoxComponent.h"

// Player
#include "Character/ShooterCharacter.h"

// Enemy
#include "Character/Enemy.h"

// Explosive
#include "Items/Explosive.h"

ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Root->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetRootComponent(Root);

	SpawnOrderArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Area"));
	SpawnOrderArea->SetupAttachment(GetRootComponent());
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnTargetActor();

	// Delegate
	SpawnOrderArea->OnComponentBeginOverlap.AddDynamic(this, &ASpawner::SpawnAreaBeginOverlap);
	SpawnOrderArea->OnComponentEndOverlap.AddDynamic(this, &ASpawner::SpawnAreaEndOverlap);
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASpawner::SpawnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
	if (Player)
	{
		Player->SetInSpawnArea(true);
		Player->SetSpanwer(this);
	}
}

void ASpawner::SpawnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
	if (Player)
	{
		Player->SetInSpawnArea(false);
		Player->SetSpanwer(nullptr);

	}
}

void ASpawner::SpawnTargetActor()
{
	if (SpawnActors.Num() <= 0 && EnemySpawnTransform.Num() <= 0) return;
	
	// Enemy Spawn
	for (int i = 0; i < EnemySpawnTransform.Num(); i++)
	{
		AEnemy* Spawn = GetWorld()->SpawnActor<AEnemy>(
			SpawnActors[i],
			EnemySpawnTransform[i]->GetActorLocation(),
			EnemySpawnTransform[i]->GetActorRotation()
		);

		UE_LOG(LogTemp, Warning, TEXT("Spawn Enemy"));
	}

	// Explosive Spawn
	if (SpawnExplosive && ExplosiveSpawnTransform)
	{
		GetWorld()->SpawnActor<AExplosive>(SpawnExplosive,
			ExplosiveSpawnTransform->GetActorLocation(),
			ExplosiveSpawnTransform->GetActorRotation()
		);

		UE_LOG(LogTemp, Warning, TEXT("Spawn Explosive"));
	}
}
