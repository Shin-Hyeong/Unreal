#include "Door.h"
//#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	RootComponent = DoorMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh (TEXT("/Game/StarterContent/Props/SM_Door"));
	if (StaticMesh.Succeeded() && StaticMesh.Object != NULL)
	{
		DoorMesh->SetStaticMesh(StaticMesh.Object);
		DoorMesh->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
		DoorMesh->SetWorldScale3D(FVector(1.0f));
	}
	// StaticMesh 불러와서 저장하기.

	CloseTime = 3.0f;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	OriginRotation = GetActorRotation();

	GetWorldTimerManager().SetTimer(DoorTimeHandle, this, &ADoor::Open, 0.01f, true);
	// 문이 열리도록 하는 Open 함수를 0.01초마다 불러옴.
	
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoorDeltaTime += DeltaTime;
}

void ADoor::Open()
{
	if (!bOpen)
	{
		bOpen = true;
		DoorDeltaTime = 0.0f;
	}

	FRotator rotation = OriginRotation + FRotator(0.0f, FMath::Lerp(0.0f, 90.0f, DoorDeltaTime), 0.0f);
	// DoorDeltaTime의 상태에 따라서 문을 회전시킴. Rotate(0, (0.0에서 90.0으로 lerp값 가짐.), 0)
	SetActorRotation(rotation);
	// 회전한 문의 Rotate 값을 rotation 변수에 저장함.
	if (DoorDeltaTime > 1.0f)
	{
		GetWorldTimerManager().ClearTimer(DoorTimeHandle);
		// 문을 여는 타이머를 종료 시킴.
		GetWorldTimerManager().SetTimer(DoorTimeHandle, this, &ADoor::Close, 0.01f, true, CloseTime);
		// 문을 닫는 Close 함수를 0.01초 마다 불러옴.
	}
}

void ADoor::Close()
{
	if (bOpen)
	{
		bOpen = false;
		DoorDeltaTime = 0.0f;
	}

	FRotator rotation = OriginRotation + FRotator(0.0f, FMath::Lerp(90.0f, 0.0f, DoorDeltaTime), 0.0f);
	// DoorDeltaTime의 상태에 따라서 문을 회전시킴. Rotate(0, (90.0에서 0.0으로 lerp값 가짐.), 0)
	SetActorRotation(rotation);
	// 회전한 문의 Rotate 값을 rotation 변수에 저장함.
	if (DoorDeltaTime > 1.0f)
	{
		GetWorldTimerManager().ClearTimer(DoorTimeHandle);
	}
}
