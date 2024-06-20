#include "Items/Item.h"

// Collision
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Widget
#include "Components/WidgetComponent.h"

// Player
#include "Character/ShooterCharacter.h"

// Camera
#include "Camera/CameraComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// bInterp이 true일때, EquipInterping 상태일 때
	ItemInerp(DeltaTime);
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	// 변경된 상태에 맞게 아이템의 Collision, 등 설정
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char)
{
	// TimerHandle를 위한 저장
	Character = Char;

	// Item의 처음 위치 저장
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);

	// InterpYawOff 계산 (ItemYaw - CameraYaw)
	// CameraRoation Yaw
	const double CameraRotationYaw{ Character->GetCamera()->GetComponentRotation().Yaw };
	// Item Ratation Yaw
	const double ItemRotationYaw{ GetActorRotation().Yaw };

	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;
}

void AItem::SetWidgetVisibility(bool bVisibility)
{
	if (PickupWidget && bCanWidgetVisibility)
	{
		PickupWidget->SetVisibility(bVisibility);
	}
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		// Widget 숨김
		PickupWidget->SetVisibility(false);
	}

	// Rarity 설정
	SetActiveStars();

	// 기본 아이템 설정 적용
	SetItemProperties(ItemState);
	
	// Delegate : Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::SetActiveStars()
{
	for (int8 i = 0; i <= 5; i++)
	{
		// false로 초기화
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_UnCommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_PickUp:
		// 총기 메쉬 설정
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// AreaSphere 설정
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// CollisionBox 설정
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EIS_Equipped:
		// Widget 설정
		PickupWidget->SetVisibility(false);
		// 총기 메쉬 설정
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// AreaSphere 설정
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// CollisionBox 설정
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_EquipInterping:
		// Widget 설정
		PickupWidget->SetVisibility(false);
		// 총기 메쉬 설정
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// AreaSphere 설정
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// CollisionBox 설정
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling:
		// 총기 메쉬 설정
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		// AreaSphere 설정
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// CollisionBox 설정
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character)
	{
		Character->GetPickupItem(this);
	}
	// Interpolation 중 작아진 Scale를 다시 되돌려놓음
	SetActorScale3D(FVector(1.0f));
}

void AItem::ItemInerp(float DeltaTime)
{
	if (bInterping == false)  return;

	if (Character && ItemZCurve)
	{
		// 타이머가 시작한 후 경과된 시간
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// ItemZCurze의 값에 맞는 값 나옴
		// 0.2s = 1.5f , 0.3s = 1.0f , 0.7s = 0.f
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);

		// Curve가 시작됬을 때 Item의 위치
		FVector ItemLocation = ItemInterpStartLocation;
		// Camera 앞에 있는 Interp 위치
		const FVector CameraInterpLocation{ Character->GetCameraInterpLocation() };

		// Item에서 Camera Interp Location까지의 Z값
		const FVector ItemToCamera{ FVector(0.0f, 0.0f, (CameraInterpLocation - ItemLocation).Z) };
		// Z값만 있는 ItemToCamera의 float화
		const float DeltaZ = ItemToCamera.Size();

		// Interpolation의 X값
		const FVector CurrentLocation{ GetActorLocation() };
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.0f);

		// Interpolation의 Y값
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.0f);
		
		// Interpolation하기 위한 X, Y값 저장
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;
		// Curve값(CurveValue) * Z 이동 방향(DeltaZ)을 처음 아이템 위치(ItemLocation)에 더한다.
		ItemLocation.Z += CurveValue * DeltaZ;
		// Item 이동
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// 카메라가 Interp중 회전해도 Item이 회전하지 않고 유지 되도록 함
		const FRotator CameraRotation{ Character->GetCamera()->GetComponentRotation() };
		FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		// Interpolation중 내려갈 때 Scale이 점점 작아지도록 함
		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			// Player가 정보를 볼 수 있는 아이템 갯수 카운팅
			ShooterCharacter->IncrementOverlappedItemCount(1);
			// 해당 아이템의 Info Widget를 볼 수 있음
			bCanWidgetVisibility = true;
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			// Player가 정보를 볼 수 있는 아이템 갯수 카운팅
			ShooterCharacter->IncrementOverlappedItemCount(-1);
			// 해당 아이템의 Info Widget를 볼 수 없음
			bCanWidgetVisibility = false;
		}
	}
}



