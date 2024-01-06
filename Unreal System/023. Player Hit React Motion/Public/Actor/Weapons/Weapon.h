#pragma once

#include "CoreMinimal.h"
#include "Actor/Item.h"
#include "Weapon.generated.h"

// 무기 종류에 따른 장착 소리를 넣어 주기 위해(검, 망치의 소리는 다름)
class USoundBase;
// 무기의 공격 충돌 감지 콜리전
class UBoxComponent;

UCLASS()
class UDEMYCPP_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	// 생성자
	AWeapon();

	// 무기 장착 함수 -> PlayerCharacher에서 호출 할 예정
	// AttachToComponent()의 인자값을 참고해서 부모가 될 Component와 소켓 이름을 받는다.
	// 무기의 주인(Owner)와 선동자(Instigator)가 될 것을 정하기 위해서 매개변수 2개 추가
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	void DeactivateEmbers();

	void DisableSphereCollision();

	void PlayEquipSound();

	// Mesh를 Socket에 붙도록 하는 함수
	void AttachMashToSocket(USceneComponent* InParent, const FName& InSocketName);

	// private에 있는 WeaponBox를 가져오는 함수
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

protected:
	virtual void BeginPlay() override;

	// Box Collision에 충돌 함수
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& BoxHit);

	// FieldSystem
	// BlueprintImplementableEvent : BP에서 구현한다.
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	void BoxTrace(FHitResult& BoxHit);

public:
	TArray<AActor*> IgnoreActors;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properities")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properities")
	UBoxComponent* WeaponBox;

	// BoxTrace의 시작과 끝지점
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properities")
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properities")
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properities")
	float Damage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properities")
	FVector BoxTraceExtent = FVector(5.0f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properities")
	bool bShowBoxDebug = false;

};
