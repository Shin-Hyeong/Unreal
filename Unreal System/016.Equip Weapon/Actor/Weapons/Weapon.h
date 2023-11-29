#pragma once

#include "CoreMinimal.h"
#include "Actor/Item.h"
#include "Weapon.generated.h"

// 무기 종류에 따른 장착 소리를 넣어 주기 위해(검, 망치의 소리는 다름)
class USoundBase;

UCLASS()
class UDEMYCPP_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	// 무기 장착 함수 -> PlayerCharacher에서 호출 할 예정
	// AttachToComponent()의 인자값을 참고해서 부모가 될 Component와 소켓 이름을 받는다.
	void Equip(USceneComponent* InParent, FName InSocketName);

	// Mesh를 Socket에 붙도록 하는 함수
	void AttachMashToSocket(USceneComponent* InParent, const FName& InSocketName);

protected:
	// Item class에서 상속받아 재정의
	// 상속 받은 함수는 UFUNCTION()를 제거해야한다.
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properities")
	USoundBase* EquipSound;
};
