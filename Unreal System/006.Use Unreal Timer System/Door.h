#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"


class UStaticMeshComponent;

UCLASS()
class FORENHANCEDINPUT_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Door")
	// �� ����ƽ�޽ø� ���� ������Ʈ
	UStaticMeshComponent* DoorMesh;
	
	UPROPERTY(EditAnywhere, Category = "Door")
	// ���� ���ȴٰ� ���� �ð�
	float CloseTime;

	UPROPERTY(EditAnywhere, Category = "Door")
	// ���� ������ ���� �ð�
	float DoorDeltaTime;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	// ���� ���� �������� Ȯ��
	bool bOpen;

	// Ÿ�̸� �ڵ� ����
	FTimerHandle DoorTimeHandle;

	// ���� ���� Rotate ���� ������ ����
	FRotator OriginRotation;

	// ���� ���� �Լ�
	void Open();

	// ���� �ݴ� �Լ�
	void Close();
};
