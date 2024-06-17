#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UBoxComponent;
class USphereComponent;
class UWidgetComponent;
class UCurveFloat;
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


	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetBoxComponent() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundBase* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundBase* GetEquipSound() const { return EquipSound; }

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
	void SetItemProperties(EItemState State);

	/**
		Interpolation
	*/
	// ItemInterpTimer 타이머가 호출
	// 선형 이동이 끝난 후 SwapWeapon()
	void FinishInterping();

	// 선형 이동
	// EquipInterping 상태일때 선형 이동
	void ItemInerp(float DeltaTime);

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
	int32 ItemCount{ 0 };

	// Item의 희귀도 표시(Widget에서 별 갯수)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity{ EItemRarity::EIR_Common };

	// Widget의 별 활성화/비활성화 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	// 아이템 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState{ EItemState::EIS_PickedUp };

	/**
		Interpolation
	*/
	// 선형 이동을 위한 Z 커브
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemZCurve;

	// 선형 이동 할 때의 X 값
	float ItemInterpX{ 0.f };

	// 선형 이동 할 때의 Y 값
	float InterInterpY{ 0.f };

	// 선형 이동 시작 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation{ FVector(0.f) };

	// 선형 이동 도착 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation{ FVector(0.f) };

	// 선형 이동 할지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bInterping{ false };

	// 선형 이동 시작 타이머
	FTimerHandle ItemInterpTimer;

	// Player가 Weapon를 얻으면서 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* Character;

	// Z 선형 이동 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime{ 0.8f };

	// 선형 이동중 회전 하지 않도록 YawOff으로 고정함
	float InterpInitialYawOffset{ 0.f };

	// Item이 내려갈때 점점 작아지도록 하기 위한 Curve
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	/**
		Mesh
	*/
	// 아이템 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/**
		Collision 
	*/
	// 해당 Collision에 LineTrace가 닿으면 정보 widget을 띄울것임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	/**
		Widget
	*/
	// 플레이어가 아이템을 볼때 팝업할 Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	bool bCanWidgetVisibility{ false };

	/**
		Sound
	*/
	// 아이템 주울때 나는 소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundBase* PickupSound;

	// 아이템을 장착할 때 나는 소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundBase* EquipSound;
};
