#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

class USphereComponent;

UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()
public:
	AAmmo();

	/** <AActor> */ 
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <AItem> */
	// 픽업할때 AmmoMesh도 속성이 변경되도록 오버라이딩
	virtual void SetItemProperties(EItemState State) override;
	/** </AItem> */

	/**
		Custom Depth
	*/
	// virtual void InitializeCustomDepth() override;
	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;

	UFUNCTION()
	void AmmoSphereCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	/**
		Properties
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Item Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType{ EAmmoType::EAT_9mm };

	/**
		Collision
	*/
	// Player가 Overlap되면 자동으로 획득되도록 함.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Collision", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AmmoCollisionSphere;


	/** 
		Mesh
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo | Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	/**
		Texture
	*/
	// Pickup Widget에서 총알의 타입에 맞게 Texture를 적용하기 위해
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo | Texture | Widget", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;
};
