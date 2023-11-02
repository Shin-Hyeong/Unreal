// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraControllDirector.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FChangeCameraData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	AActor* Camera;
	UPROPERTY(EditAnywhere)
	float TimeBetweenCameraChanges;
	UPROPERTY(EditAnywhere)
	float SmoothBlendTime;
};

UCLASS()
class FORENHANCEDINPUT_API ACameraControllDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraControllDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*�迭�� ����Ͽ� ������ �гο��� ���ϴ� ��ŭ ���� ����.*/
	UPROPERTY(EditAnywhere)
	//TArray<AActor*> Cameras; //AActor(����) Ÿ���� �迭 ����
	TArray<FChangeCameraData> Cameras; //����ü Ÿ�� �迭 ����

	int32 NowCameraIndex;

	/*Cpp ���Ͽ��� ������ ����ŭ�� ���. ������ �гο��� �ش� ���Ϳ� ī�޶� �־ ���*/
	//UPROPERTY(EditAnywhere)
	//AActor* CameraOne;
	//UPROPERTY(EditAnywhere)
	//AActor* CameraTwo;

	UPROPERTY(EditAnywhere)
	float TimeToNextCameraChange; //���� ī�޶�� �̵��ϱ���� ���� �ð�

};
 