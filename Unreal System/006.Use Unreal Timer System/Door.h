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
	// 문 스태틱메시를 가질 컴포넌트
	UStaticMeshComponent* DoorMesh;
	
	UPROPERTY(EditAnywhere, Category = "Door")
	// 문이 열렸다가 닫힐 시간
	float CloseTime;

	UPROPERTY(EditAnywhere, Category = "Door")
	// 문이 닫히는 과정 시간
	float DoorDeltaTime;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	// 문이 열린 상태인지 확인
	bool bOpen;

	// 타이머 핸들 변수
	FTimerHandle DoorTimeHandle;

	// 문의 원래 Rotate 값을 저장할 변수
	FRotator OriginRotation;

	// 문을 여는 함수
	void Open();

	// 문을 닫는 함수
	void Close();
};
