#include "Tutorial/TutorialText.h"

// BoxComponent
#include "Components/BoxComponent.h"

// Player
#include "Character/ShooterCharacter.h"

ATutorialText::ATutorialText()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Root->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetRootComponent(Root);

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow Mesh"));
	ArrowMesh->SetupAttachment(GetRootComponent());
	ArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowMesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);

	TutorialBoxArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Tutorial Box Area"));
	TutorialBoxArea->SetupAttachment(GetRootComponent());
	TutorialBoxArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TutorialBoxArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TutorialBoxArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ATutorialText::BeginPlay()
{
	Super::BeginPlay();

	ArrowMesh->SetVisibility(bBeginVisible);
	

	// Delegate
	TutorialBoxArea->OnComponentBeginOverlap.AddDynamic(this, &ATutorialText::GuideAreaBeginOverlap);
	TutorialBoxArea->OnComponentEndOverlap.AddDynamic(this, &ATutorialText::GuideAreaEndOverlap);
}

void ATutorialText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ArrowMesh && bCanTurn)
	{
		ArrowMesh->AddLocalRotation(FRotator(0.f, YawRate, 0.f));
		SineMove(DeltaTime);
	}
}

void ATutorialText::GuideAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("GuideAreaBeginOverlap"));

		// 현재 Guide표시 비활성화
		bCanTurn = false;
		ArrowMesh->SetVisibility(false);

		// 다음 Guide위치 표시
		if (NextTarget)
		{
			NextTarget->SetCanTurn(true);
			NextTarget->GetArrowMesh()->SetVisibility(true);
		}

		if (GuideTextIndx <= Player->GetTileArray().Num())
		{
			Player->SetGuideTextIdx(GuideTextIndx);
		}

		Player->GetGuideOverlapDelegate().Broadcast(GuideTextIndx, 0);
	}
}

void ATutorialText::GuideAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("GuideAreaEndOverlap"));
	}
}

void ATutorialText::SineMove(float DeltaTime)
{
	RunningTime += DeltaTime;

	float DeltaZ = Amplitude * FMath::Sin(RunningTime * SineSpeed);
	ArrowMesh->AddWorldOffset(FVector(0.f, 0.f, DeltaZ));
}