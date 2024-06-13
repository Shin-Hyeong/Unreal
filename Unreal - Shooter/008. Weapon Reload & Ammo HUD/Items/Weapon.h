#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
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


	FORCEINLINE int32 GetAmmo() { return Ammo; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }
	FORCEINLINE	EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

	// Player가 총을 쏴서 총알을 소모함
	void DecrementAmmo();

protected:
	void StopFalling();

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

	FTimerHandle ThrowWeaponTimer;

	float ThrowWeaponTime{ 0.4f };

	bool bFalling{ false };
};
