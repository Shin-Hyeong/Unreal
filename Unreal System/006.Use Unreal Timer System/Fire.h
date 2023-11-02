#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Fire.generated.h"

UCLASS()
class FORENHANCEDINPUT_API AFire : public AActor
{
	GENERATED_BODY()
	
public:	
	AFire();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Fire")
	UParticleSystemComponent* FireParticle;

	UPROPERTY(EditAnywhere, Category = "Fire")
	int8 FireTime; // 1byte

	FTimerHandle FireTimerHandle;
	// 타이머 제어를 위한 핸들

	void Combustion();
	// 불 파티클이 연소하는 함수

	UFUNCTION(BlueprintCallable)
	void OffFire();
	// 불을 끄는 함수
	// 블루프린터의 이벤트 그래프에서 커스텀 이벤트를 불러와서 연결해서 입력값을 넣으면 바로 꺼지도록 할 수 있다.
	// RootComponent에서 Auto Receive Input를 player0로 설정해야 한다.
};
