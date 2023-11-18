#include "Actor/Item.h"
#include "Components/SphereComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMEshComponent"));
	RootComponent = StaticMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetSphereRadius(300.f);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	// CallBack Func bind in Delegate
	// AddDynamic(UserObject, FuncName) -> 앞에 있는 함수에 
	// UserObject : 콜백 함수가 있는 개체. 포인트형태
	// FuncName : 콜백 함수 주소 -> Heap 영역에 저장되기 떄문에 값전달보다 참조전달를 하는 것이 좋다.
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	FVector Location = GetActorLocation();

	float DeltaZ = Amplitude * FMath::Sin(RunningTime * SineSpeed);

	AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FString OtherActorName = OtherActor->GetName();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Red, OtherActorName);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Green, TEXT("EndOverlap"));
	}
}
