#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UBoxComponent;
class USphereComponent;
class UWidgetComponent;
class UCurveFloat;
class UCurveVector;
class AShooterCharacter;
class USoundBase;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	// 1성
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	// 2성
	EIR_Common UMETA(DisplayName = "Common"),
	// 3성
	EIR_UnCommon UMETA(DisplayName = "UnCommon"),
	// 4성
	EIR_Rare UMETA(DisplayName = "Rare"),
	// 5성
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	// 획득 가능한 상태
	EIS_PickUp UMETA(DisplayName = "PickUp"),
	// 아이템 창에 들어가는 모션 진행 상태
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	// 아이템 창에 저장되어 있는 상태
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	// 현재 장착된 상태
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	// 아이템을 버려지고 있는 상태(줍지 못하는 상태)
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),

	EIT_MAX UMETA(DisplayName = "Default Max")
};

UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	/**
		Property
	*/
	void SetItemState(EItemState State);

	/**
		Interpolation
	*/
	// ShooterCharacter 접근
	void StartItemCurve(AShooterCharacter* Char);

	/**
		Widget
	*/
	void SetWidgetVisibility(bool bVisibility);

	/**
		Custom Depth
	*/
	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();

	/**
		Material
	*/
	void EnableGlowMaterial();
	void DisableGlowMaterial();

	/**
		Get / Set
	*/
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetBoxComponent() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundBase* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundBase* GetEquipSound() const { return EquipSound; }
	// 총알 갯수
	FORCEINLINE int32 GetAmmoCount() const { return AmmoCount; }
	// 주인이 있는지
	FORCEINLINE bool GetIsOwner() const { return bIsOwner; }
	FORCEINLINE void SetIsOnwer(bool IsOwner) { bIsOwner = IsOwner; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/**
		Property
	*/
	// Widget에서 Item의 희귀도를 나타내는 ActiveStar를 설정
	void SetActiveStars();

	// ItemState에 따라 Collision,등 설정 변경
	virtual void SetItemProperties(EItemState State);

	/**
		Interpolation
	*/
	// ItemInterpTimer 타이머가 호출
	// 선형 이동이 끝난 후 SwapWeapon()
	void FinishInterping();

	// 선형 이동
	// EquipInterping 상태일때 선형 이동
	void ItemInerp(float DeltaTime);

	// Interp할 Location을 InterpLocations에서 구하기
	FVector GetInterpLocation();

	/**
		Sound
	*/
	void PlaySound(USoundBase* SoundName);


	/**
		Custom Depth
	*/
	virtual void InitializeCustomDepth();

	/**
		Material
	*/
	// Actor가 속성이 변경되거나 움직이면 호출됨.
	virtual void OnConstruction(const FTransform& Transform) override;

	// PulseCurveTime(5.s)마다 다시 호출함.
	void StartPulseTimer();
	
	// StartPulseTimer를 호출함.
	void ResetPulseTimer();

	// 
	void UpdatePulse();

	/**
		Call when Overlapping
	*/
	// Begin Overlap
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// End Overlap
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:

private:
	/**
		Property
	*/
	// Pickup Widget에 적용(바인딩)될 아이템 이름
	// BP에서 바인딩함.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName{ "Default" };

	// 총알 갯수, 등 아이템 갯수 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 AmmoCount{ 0 };

	// Item의 희귀도 표시(Widget에서 별 갯수)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity{ EItemRarity::EIR_Common };

	// Widget의 별 활성화/비활성화 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	// 아이템 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState{ EItemState::EIS_PickUp };

	// 아이템 주인이 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsOwner{ false };

	// Item이 총인지 총알인지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType{ EItemType::EIT_MAX };

	/**
		Interpolation
	*/
	// 선형 이동을 위한 Z 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Interpolation", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemZCurve;

	// 선형 이동 할 때의 X 값
	float ItemInterpX{ 0.f };

	// 선형 이동 할 때의 Y 값
	float InterInterpY{ 0.f };

	// 선형 이동 시작 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties | Interpolation", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation{ FVector(0.f) };

	// 선형 이동 도착 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties | Interpolation", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation{ FVector(0.f) };

	// 선형 이동 할지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties | Interpolation", meta = (AllowPrivateAccess = "true"))
	bool bInterping{ false };

	// 선형 이동 시작 타이머
	FTimerHandle ItemInterpTimer;

	// Player가 Weapon를 얻으면서 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties | Interpolation", meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* Character;

	// Z 선형 이동 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Interpolation", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime{ 0.8f };

	// 선형 이동중 회전 하지 않도록 YawOff으로 고정함
	float InterpInitialYawOffset{ 0.f };

	// Item이 내려갈때 점점 작아지도록 하기 위한 Curve
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Interpolation", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	// 해당 Item이 Interp하기 하기 위한 Location를 가진 구조체의 Index
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties | Interpolation", meta = (AllowPrivateAccess = "true"))
	int32 InterpLocIndex{ 0 };

	/**
		Mesh
	*/
	// 아이템 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties | Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/**
		Collision 
	*/
	// 해당 Collision에 LineTrace가 닿으면 정보 widget을 띄울것임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	// 해당 Collision에 들어와야 정보 Widget을 LineTrace를 시작함.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Collision", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	/**
		Widget
	*/
	// 플레이어가 아이템을 볼때 팝업할 Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Widget", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties | Widget")
	bool bCanWidgetVisibility{ false };

	/**
		Inventory
	*/
	// Inventory에서 사용할 Background 이미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconBackground;

	// Inventory에서 사용할 Item 이미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconItem;

	// Inventory에서 사용할 Ammo 이미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconAmmo;

	// Item이 Inventory에 저장된 Slot Index 번호
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties | Inventory", meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex{ 0 };

	/**
		Sound
	*/
	// 아이템 주울때 나는 소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties | Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* PickupSound;

	// 아이템을 장착할 때 나는 소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties | Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* EquipSound;

	/**
		Material
	*/
	// 런타임 중 변경할 Material 인덱스 번호
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex{ 0 };

	// 런타임 중 변경 할 수 있는 Dynamic Material
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstacne;

	// Dynamic Material Instance로 사용할 Material Instacne
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;

	

	// Dynamic Material의 Curve값을 주기 위해
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	UCurveVector* PulseCurve;

	// Interp 중 Hovering 할 때 반짝이도록 하는 Curve 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	UCurveVector* InterpPulseCurve;

	FTimerHandle PulseTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	float PulseCurveTime{ 5.0f };

	// LineEffect의 정도
	UPROPERTY(EditAnywhere, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	float GlowAmount{ 150.f };

	// 중앙으로부터 발광하지 않는 범위 (클 수록 범위 넒어짐.)
	UPROPERTY(EditAnywhere, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	float FresnelExponent{ 3.0f };

	// 발광 세기 정도
	UPROPERTY(EditAnywhere, Category = "Item Properties | Material", meta = (AllowPrivateAccess = "true"))
	float FresnelReflectFraction{ 4.0f };

	/**
		Custom Depth
	*/
	// Interp 도중에 테두리 효과가 사리지는것을 막음.
	bool bCanChangeCustomDepth{ true };
};
