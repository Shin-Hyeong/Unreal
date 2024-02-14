#include "Actor/Item.h"
#include "Components/SphereComponent.h"

// Overlap된 Item를 Character의 OverlappingItem 변수에 저장시키기 위해
#include "Interfaces/PickUpInterface.h"

// Niagara System 사용 -> build.cs에서 모듈에 Niagara를 추가해야함
// NS를 구현
#include "NiagaraComponent.h"
// NS를 소환
#include "NiagaraFunctionLibrary.h"

// 소리 재생
#include "Kismet/GameplayStatics.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMEshComponent"));
	StaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = StaticMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetSphereRadius(SphereRadius);

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(GetRootComponent());
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

	if (ItemState == EItemState::EIS_Hovering)
	{
		SineMovement();
	}
}

void AItem::SineMovement()
{
	float DeltaZ = Amplitude * FMath::Sin(RunningTime * SineSpeed);

	AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// OtherActor가 IPickUpInterface이거나 그를 상속받고 있다면 Interface에 IPickUpInterface를 반환, 아니면 null
	// Cast<>() -> Dynamic_Cast<>() -> 상속성을 확인하는 Cast
	IPickUpInterface* Interface = Cast<IPickUpInterface>(OtherActor);

	if (Interface)
	{	
		// 해당 Item를 PlayerCharater의 OverlappingItem에 저장 함.
		Interface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// OtherActor가 IPickUpInterface와 동일하면 Interface에 IPickUpInterface를 반환, 아니면 null
	IPickUpInterface* Interface = Cast<IPickUpInterface>(OtherActor);

	if (Interface)
	{
		// 해당 아이템과 떨어질때 OverlappingItem에 null를 주어짐
		Interface->SetOverlappingItem(nullptr);
	}
}

void AItem::SpawnPickupSystme()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, // const UObject* WorldContextObject
			PickupEffect,// UNiagaraSystem* SystemTemplate
			GetActorLocation()// FVector Location
			// FRotator Rotaion = ZeroRotator
			// FVector Scale = 1.0f
			// bool bAutoDestory = true
			// bool bAutoActivate = true
			// ENCPoolMethod PoolingMethod = ENCPoolMethod::None
			// bPreCullCheck = true
		);
	}
}

void AItem::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this, // const UObject* WorldContextObject
			PickupSound, // USoundBase* Sound
			GetActorLocation() // FVector Location
			// float VolumeMultiplier = 1.0f
			// float PitchMultiplier = 1.0f
			// float StartTime = 0.0f
			// USoundAttenuation* AttenuationSettings = nullptr
			// USoundConcurrency* ConcurrencySettings = nullptr
			// UInitialActiveSoundParam* InitialParams = nullptr
		);
		UE_LOG(LogTemp, Warning, TEXT("Play Sound"));
	}
}
 