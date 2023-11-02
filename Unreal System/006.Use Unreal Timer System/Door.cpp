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
	// StaticMesh �ҷ��ͼ� �����ϱ�.

	CloseTime = 3.0f;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	OriginRotation = GetActorRotation();

	GetWorldTimerManager().SetTimer(DoorTimeHandle, this, &ADoor::Open, 0.01f, true);
	// ���� �������� �ϴ� Open �Լ��� 0.01�ʸ��� �ҷ���.
	
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
	// DoorDeltaTime�� ���¿� ���� ���� ȸ����Ŵ. Rotate(0, (0.0���� 90.0���� lerp�� ����.), 0)
	SetActorRotation(rotation);
	// ȸ���� ���� Rotate ���� rotation ������ ������.
	if (DoorDeltaTime > 1.0f)
	{
		GetWorldTimerManager().ClearTimer(DoorTimeHandle);
		// ���� ���� Ÿ�̸Ӹ� ���� ��Ŵ.
		GetWorldTimerManager().SetTimer(DoorTimeHandle, this, &ADoor::Close, 0.01f, true, CloseTime);
		// ���� �ݴ� Close �Լ��� 0.01�� ���� �ҷ���.
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
	// DoorDeltaTime�� ���¿� ���� ���� ȸ����Ŵ. Rotate(0, (90.0���� 0.0���� lerp�� ����.), 0)
	SetActorRotation(rotation);
	// ȸ���� ���� Rotate ���� rotation ������ ������.
	if (DoorDeltaTime > 1.0f)
	{
		GetWorldTimerManager().ClearTimer(DoorTimeHandle);
	}
}
