#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UBoxComponent;
class USphereComponent;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_UnCommon UMETA(DisplayName = "UnCommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_Max UMETA(DisplayName = "DefaultMax")
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
		Widget
	*/
	void SetWidgetVisibility(bool bVisibility);

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/**
		Property
	*/
	// Widget에서 Item의 희귀도를 나타내는 ActiveStar를 설정
	void SetActiveStars();
	
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
	FString ItemName{"Default"};

	// 총알 갯수, 등 아이템 갯수 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount{ 0 };

	// Item의 희귀도 표시(Widget에서 별 갯수)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity{EItemRarity::EIR_Common};

	// Widget의 별 활성화/비활성화 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

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
};
