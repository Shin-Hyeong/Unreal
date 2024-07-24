#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "AmmoType.h"
#include "WeaponType.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	// 총알 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	// 장전된 총알 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	// 탄창 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazingCapacity;

	// 획득 Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* PickupSound;

	// 장착할때 Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* EquipSound;

	// 무기 타입에 따른 Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	// Widget에 띄울 Weapon Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	// Inventory에 띄울 Weapon Icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	// Widget에 띄울 Weapon의 AmmoIcon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;

	// 무기 타입에 따라 적용될 GlowEffect를 가진 MaterialInstance
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	// 무기 타입에 따르 DynamicMaterialInstacne가 적용될 Material Index
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;
	
	// 무기에 따른 ClipBoneName
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ClipBoneName;

	// 무기에 따른 재장전 모션 SectionName
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	// 무기에 따른 사용할 AnimBP
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

	// 무기에 따른 Crosshair
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairMiddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairTop;
	
	// 무기에 따른 연사 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	// 무기에 따른 총열흔적
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash;

	// 무기에 따른 총성 소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* FireSound;

	// 숨겨야하는 Bone, 없앨 Bone이 없으면 빈칸
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	// ture이면 자동사격, false이면 반자동
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;
};

UCLASS()
class SHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	// Waepon에 충동시킴
	void ThrowWeapon();

	// 탄창에 총알을 넣음
	void ReloadAmmo(int32 Amount);

	bool ClipIsFull();

	FORCEINLINE int32 GetAmmo() { return Ammo; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }
	FORCEINLINE	EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE void SetReloadMontageSection(const FName& Section) { ReloadMontageSection = Section; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE void SetClipBoneName(const FName& BoneName) { ClipBoneName = BoneName; }
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }
	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE USoundBase* GetFireSound() const { return FireSound; }
	FORCEINLINE bool GetAutomatic() const { return bAutomatic; }

	// Player가 총을 쏴서 총알을 소모함
	void DecrementAmmo();
	
	// PistolSlide Timer
	void StartSlideTimer();

protected:
	virtual void BeginPlay() override;

	void StopFalling();

	// BP의 Construction Script의 C++버전
	virtual void OnConstruction(const FTransform& Transform) override;

	// SlideTimer가 종료되고 호출
	void FinishMovingSlide();

	// PistolSlide의 움직임 업데이트
	void UpdateSlideDisplacement();

private:
	// 무기 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType{ EWeaponType::EWT_SubmachineGun };

	// 총알 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType{ EAmmoType::EAT_9mm };

	// 탄창에 있는 총알 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo{ 30 };

	// 탄창 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapacity{ 30 };

	// ReloadMantage의 Selection에 사용될 FName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection{ FName(TEXT("Reload SMG"))};

	// 재장전 중 탄창이 손에 붙어서 움직이고 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bMovingClip{ false };

	// 탄창을 움직이게 하는 Bone 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	FName ClipBoneName{ TEXT("smg_clip") };

	// 자동 사격하도록 할것인지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bAutomatic{ true };


	/**
		Pistol
	*/
	// Slide의 움직임을 제어하기 위한 Timer
	FTimerHandle SlideTimer;

	// Pistol 발사 하는동안 Slide가 움직이는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float SlideDisplacementTime{ 0.2f };

	// true일때 Slide를 움직임, true일 때 Anim에서 움직이도록 함
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	bool bMovingSlide{ false };

	// Pistol 사격시 Slide가 움직이게 하는 정도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float SlideDisplacement{ 0.f };

	// Slide의 최대 움직임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float MaxSlideDisplacement{ 4.0f };

	// 총기 반동으로 느낌으로 총을 회전시킴
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float RecoilRotation{ 0.f };

	// 총기 최대 반동
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float MaxRecoilRotation{ 20.f };

	// Slide의 움직임을 위한 Curve
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SlideDisplacementCurve;


	// 무기 떨어트리는 Timer
	FTimerHandle ThrowWeaponTimer;

	// 무기를 떨어지는 시간
	float ThrowWeaponTime{ 0.4f };

	// 무기가 떨어지고 있는중인지
	bool bFalling{ false };

	/**
		Data Table
	*/
	// Weapon Properties를 정보를 가지고 있는 Data Table
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties | Data Table", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;

	// CrossHair
	// 무기에 따른 Crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties | Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairMiddle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties | Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties | Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties | Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties | Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairTop;

	// Weapon Properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float AutoFireRate;

	// 무기에 따른 총열흔적
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	// 무기에 따른 총성 소리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	USoundBase* FireSound;

	// 숨길 BoneName
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName BoneToHide;
};
