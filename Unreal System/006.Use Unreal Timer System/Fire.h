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
	// Ÿ�̸� ��� ���� �ڵ�

	void Combustion();
	// �� ��ƼŬ�� �����ϴ� �Լ�

	UFUNCTION(BlueprintCallable)
	void OffFire();
	// ���� ���� �Լ�
	// ����������� �̺�Ʈ �׷������� Ŀ���� �̺�Ʈ�� �ҷ��ͼ� �����ؼ� �Է°��� ������ �ٷ� �������� �� �� �ִ�.
	// RootComponent���� Auto Receive Input�� player0�� �����ؾ� �Ѵ�.
};
