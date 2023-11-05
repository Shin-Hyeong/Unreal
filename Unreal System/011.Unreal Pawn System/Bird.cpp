#include "Pawn/Bird.h"
#include "Components/CapsuleComponent.h"
#include "components/SkeletalMeshComponent.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	// Capsule 객체 생성
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	// Y축(세로) 길이 반절 설정
	Capsule->SetCapsuleHalfHeight(20.f);
	// 원의 반지름(가로) 길이 설정.
	Capsule->SetCapsuleRadius(15.f);
	// RootComponent = Capsule
	SetRootComponent(Capsule);

	// SkeletalMesh 생성
	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	// BirdMesh를 RootComponent 하위로 설정.
	BirdMesh->SetupAttachment(GetRootComponent());

}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

