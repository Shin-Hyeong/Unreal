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

	/*배열을 사용하여 디테일 패널에서 원하는 만큼 생성 가능.*/
	UPROPERTY(EditAnywhere)
	//TArray<AActor*> Cameras; //AActor(액터) 타입의 배열 생성
	TArray<FChangeCameraData> Cameras; //구조체 타입 배열 생성

	int32 NowCameraIndex;

	/*Cpp 파일에서 정해진 수만큼만 사용. 디테일 패널에서 해당 액터에 카메라를 넣어서 사용*/
	//UPROPERTY(EditAnywhere)
	//AActor* CameraOne;
	//UPROPERTY(EditAnywhere)
	//AActor* CameraTwo;

	UPROPERTY(EditAnywhere)
	float TimeToNextCameraChange; //다음 카메라로 이동하기까지 남은 시간

};
 