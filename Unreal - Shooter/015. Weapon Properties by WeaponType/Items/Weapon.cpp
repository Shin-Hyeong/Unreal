#include "Items/Weapon.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 떨어지는 동안 Yaw만 회전
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		// ItemMesh의 Yaw
		const FRotator MeshRotation{ 0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f }; 
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics); 

	// 무기의 전방 벡터
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() }; 
	// 무기의 오른쪽 벡터
	const FVector MeshRight{ GetItemMesh()->GetRightVector() }; 
	// 무기를 버릴때 던지는 방향 (MeshForward의 -20도 방향)
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward); 

	float RandoRotation = FMath::FRandRange(0.f, 30.f); 
	// MeshForward의 -20도를 회전시킨 값에 Z축 기준으로 랜덤 회전(0.0 ~ 30.0)
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandoRotation, FVector(0.0f, 0.0f, 1.0f));
	ImpulseDirection *= 15.f;

	// ImpulseDirection 방향으로 충돌
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);

	// 버릴 때 Line Effect적용
	EnableGlowMaterial();
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	// 결과식이 false이면 디버그 메시지 출력
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("탄창의 용량보다 더 많이 장전할려고 함!"));
	Ammo += Amount;
}

bool AWeapon::ClipIsFull()
{
	return MagazineCapacity <= Ammo;
}

void AWeapon::DecrementAmmo()
{
	// 총알이 0보다 작거나 같으면 0 유지
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	// 아니면 -1
	else
	{
		--Ammo;
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	OnConstruction(GetActorTransform());
}

void AWeapon::StopFalling()
{
	bFalling = false;
	// 주울 수 있는 상태로 변경
	SetItemState(EItemState::EIS_PickUp);
	// LineEffect의 파라미터 값을 다시 변화하도록 함.
	StartPulseTimer();
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Data Table 경로
	const FString WeaponTablePath{ TEXT("/Script/Engine.DataTable'/Game/_Game/DataTable/DT_Weapon.DT_Weapon'") };

	// Data Table Object 생성
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;
		switch (WeaponType) 
		{
		case EWeaponType::EWT_SubmachineGun: 
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubmachineGun"), TEXT(""));
			break;

		case EWeaponType::EWT_AssaultRifle: 
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
			break;
		}

		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazingCapacity;

			// Sound
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquipSound(WeaponDataRow->EquipSound);
			FireSound = WeaponDataRow->FireSound;

			// Mesh
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);

			// Widget
			SetItemName(WeaponDataRow->ItemName);
			SetIconItem(WeaponDataRow->InventoryIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);

			// Material
			SetMaterialInstacne(WeaponDataRow->MaterialInstance);
			GetItemMesh()->SetMaterial(GetMaterialIndex(), nullptr);
			SetMaterialIndex(WeaponDataRow->MaterialIndex);

			// Animation
			SetClipBoneName(WeaponDataRow->ClipBoneName);
			SetReloadMontageSection(WeaponDataRow->ReloadMontageSection);
			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBP);

			// Crosshair
			CrosshairMiddle = WeaponDataRow->CrosshairMiddle;
			CrosshairLeft = WeaponDataRow->CrosshairLeft;
			CrosshairRight = WeaponDataRow->CrosshairRight;
			CrosshairBottom = WeaponDataRow->CrosshairBottom;
			CrosshairTop = WeaponDataRow->CrosshairTop;

			// Properties
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
		}

		if (GetMaterialInstacne())
		{
			// DynamicMaterialInstace를 생성
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstacne(), this));
			// DataTabel에서 받아온 GlowColor를 DynamicMaterial에 적용
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColor"), GetGlowColor());
			// DynamicMaterialInstacne 적용
			GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());

			EnableGlowMaterial();
		}
	}
}
