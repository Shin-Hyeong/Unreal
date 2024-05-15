#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UNiagaraComponent;
class USoundBase;
class UMaterialInterface;

enum class EItemState : uint8
{
	// 아무런 설정 안된 상태
	EIS_NoState UMETA(DisplayName = "NoState"),
	// 아이템이 드랍된 상태(둥실둥실 떠 있음)
	EIS_Hovering UMETA(DisplayName = "Hovering"),
	// 플레이어가 장착한 상태
	EIS_Equipped UMETA(DisplayName = "Equipped")
};


UCLASS()
class SOULSLIKE__API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <UPrimitiveComponent>*/
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/** </UPrimitiveComponent>*/

	// 아이템 주변 효과 제거
	void DeactivateEmbers();
	// 아이템 Overrlap Collision 제거
	void DisableSphereCollision();
	// 아이템 획득 사운드 재생
	void AcquisitionPlaySound();

public:	
	// Hovering 효과를 위한 Sine파동 움직임
	void SineMove(float DeltaTime);

	/** Check */
	bool IsHovering();

protected:
	/** Components */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, Category = Mesh)
	float SphereRadius = 300.f;

	/** ItemState */
	EItemState ItemState = EItemState::EIS_Hovering;

private:
	/** Effect */
	UPROPERTY(EditAnywhere, Category = Effect)
	UNiagaraComponent* ItemAroundEffect;

	UPROPERTY(EditAnywhere, Category = Effect)
	UNiagaraComponent* AcquisitionEffect;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* AcquistionSound;

	/** Sine Movement */
	UPROPERTY(EditAnywhere, Category = SineMove)
	float Amplitude = 0.f;

	UPROPERTY(EditAnywhere, Category = SineMove)
	float SineSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, Category = SineMove)
	float RunningTime = 0.f;
};
