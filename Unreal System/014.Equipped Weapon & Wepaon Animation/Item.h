#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class UDEMYCPP_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine")
	float Amplitude = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine")
	float SineSpeed = 5.0f;

	float RunningTime = 0.f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* StaticMesh;

private:
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USphereComponent* Sphere;


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

public:	
	// virtual void Tick(float DeltaTime) override;

};
