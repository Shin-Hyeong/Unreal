#include "Actor/Item.h"
#include "Components/SphereComponent.h"

// Overlap된 Item를 Character의 OverlappingItem 변수에 저장시키기 위해
#include "Player/PlayerCharacter.h"

AItem::AItem()
{
	// PrimaryActorTick.bCanEverTick = true;

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
/*
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	FVector Location = GetActorLocation();

	float DeltaZ = Amplitude * FMath::Sin(RunningTime * SineSpeed);

	AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
}
*/

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// OtherActor가 APlayerCharater이거나 그를 상속받고 있다면 Player에 APlayerCharacter를 반환, 아니면 null
	// Cast<>() -> Dynamic_Cast<>() -> 상속성을 확인하는 Cast
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{	
		// 해당 Item를 PlayerCharater의 OverlappingItem에 저장 함.
		Player->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// OtherActor가 APlayerCharater와 동일하면 Player에 APlayerCharacter를 반환, 아니면 null
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{
		// 해당 아이템과 떨어질때 OverlappingItem에 null를 주어짐
		Player->SetOverlappingItem(nullptr);
	}
}
