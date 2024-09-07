#include "Items/UpgradeShop.h"

// Player
#include "Character/ShooterCharacter.h"

// Box
#include "Components/BoxComponent.h"

// SkeletalMeshComponent;
#include "Components/SkeletalMeshComponent.h"


AUpgradeShop::AUpgradeShop()
{
	PrimaryActorTick.bCanEverTick = true;

	MarketOwner = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Market Owner"));
	SetRootComponent(MarketOwner);

	Market = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Market"));
	Market->SetupAttachment(GetRootComponent());

	ShopArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Shop Area"));
	ShopArea->SetupAttachment(GetRootComponent());

}

void AUpgradeShop::BeginPlay()
{
	Super::BeginPlay();
	

	ShopArea->OnComponentBeginOverlap.AddDynamic(this, &AUpgradeShop::ShopAreaBeginOverlap);
	ShopArea->OnComponentEndOverlap.AddDynamic(this, &AUpgradeShop::ShopAreaEndOverlap);
}

void AUpgradeShop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUpgradeShop::ShopAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
	if (Player)
	{
		Player->SetInShop(true);
	}
}

void AUpgradeShop::ShopAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
	if (Player)
	{
		if (Player->GetbOpenShop())
		{
			Player->SelectButtonPressed();
		}
		Player->SetInShop(false);
	}
}



