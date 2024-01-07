#include "Breakable/BreakableActor.h"

// 기하학 컬렉션 구성요소 -> 사용하기 위해서 모듈에 GeometryCollectionEngine을 추가해야함.
#include "GeometryCollection/GeometryCollectionComponent.h"

// Treasure을 Spawn 하기 위해서
#include "Actor/Treasure.h"

// Capsule 사용
#include "Components/CapsuleComponent.h"


ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	
	// OverlapEvents = true
	GeometryCollection->SetGenerateOverlapEvents(true);
	
	// GeometryCollection -> Camera / Pawn Ignore
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// Capsule -> Panw Block
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	// Destructible Mesh가 파괴되었을 때 일어날 이벤트 Delegate
	// OnChaosBreakEvent(UObject* WorldContextObject, FChaosBreakEvent& BreakEvent);
	// GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &BreakEvent);
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	// 이미 부셔져 있는 거면 더 이상 안 부셔짐.
	if (bBroken) return;
	// 처음 부셔지면 true
	bBroken = true;

	UWorld* World = GetWorld();

	// Actor를 스폰함
	// TArray.Num() : 배열의 갯수 확인
	if (World && TreasureClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += TreasureLocationZ;

		// 0 ~ 6 중 무작위 정수
		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);

		// SpawnActor<T>()
		// ATreasure인 UClass을 소환
		World->SpawnActor<ATreasure>(
			TreasureClasses[Selection],	// UClass* Class
			Location,		// FVector* Location
			GetActorRotation()// FRotator* Rotation
			// FActorSpawnParameters& SpawnParameters = FActorSpawnParameters()
		);
	}

	// Capsule -> Pawn Ignore
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

}

