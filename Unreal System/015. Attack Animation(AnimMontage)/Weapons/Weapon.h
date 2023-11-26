#pragma once

#include "CoreMinimal.h"
#include "Actor/Item.h"
#include "Weapon.generated.h"

UCLASS()
class UDEMYCPP_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	// 무기 장착 함수 -> PlayerCharacher에서 호출 할 예정
	// AttachToComponent()의 인자값을 참고해서 부모가 될 Component와 소켓 이름을 받는다.
	void Equip(USceneComponent* InParent, FName InSocketName);

protected:
	// Item class에서 상속받아 재정의
	// 상속 받은 함수는 UFUNCTION()를 제거해야한다.
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
