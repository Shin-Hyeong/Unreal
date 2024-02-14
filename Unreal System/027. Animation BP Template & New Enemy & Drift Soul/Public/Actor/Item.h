#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;

enum class EItemState : uint8
{
	// EIS : Enum Item State
	EIS_NoState UMETA(DisplayName = "NoState"),
	// 장착하지 않는 상태
	EIS_Hovering UMETA(DisplayName = "Hovering"),
	// 캐릭터가 장착한 상태
	EIS_Equipped UMETA(DisplayName = "Equipped")
};

UCLASS()
class UDEMYCPP_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();

	virtual void Tick(float DeltaTime) override;

	// Sine Moving
	UFUNCTION(BlueprintCallable)
	void SineMovement();

protected:
	virtual void BeginPlay() override;

	// Delegate : 함수를 안전하게 호출할 수 있는 기능.
	// Delegate에 Callback 함수를 바인딩할려면 UFUNCTION()이 필요함.
	// 특정 이벤트가 발생하면 Delegate에 등록된 모든 함수를 한꺼번에 호출 할 수 있다.
	// -> BeginOverlap이 발생하면 Bind된 함수를 모두 호출함.

	// 매개변수는 PrimitiveComponent.h에 OnComponentBeginOverlap검색해서 가져온다.

	// Weapon class가 상속받아서 재정의할수 있도록 가상함수로 변경한다.
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 매개변수는 PrimitiveComponent.h에 OnComponentEndOverlap검색해서 가져온다.
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SpawnPickupSystme();
	virtual void SpawnPickupSound();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, Category = Mesh)
	float SphereRadius = 300.f;

	UPROPERTY(EditAnywhere, Category = Effect)
	UNiagaraComponent* ItemEffect;

	EItemState ItemState = EItemState::EIS_Hovering;

private:
	UPROPERTY(EditAnywhere, Category = "Sine")
	float Amplitude = 0.f;

	UPROPERTY(EditAnywhere, Category = "Sine")
	float SineSpeed = 0.f;

	float RunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;
};
