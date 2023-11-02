// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraControllDirector.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraControllDirector::ACameraControllDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACameraControllDirector::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraControllDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TimeBetweenCameraChanges = 2.0f; //카메라 이동하기까지 시간
	const float SmoothBlendTime = 0.75f; //카메라와 카메라를 이동하는 시간. 0 이면 바로 휙 넘어감

	TimeToNextCameraChange -= DeltaTime;
	if (TimeToNextCameraChange <= 0.0f)
	{	
		//AActor* NowCamera = Cameras[NowCameraIndex]; //해당 배열에 있는 카메라를 NowCamera에 넣음.
		FChangeCameraData NowCamera = Cameras[NowCameraIndex]; // 구조체 배열에 있는 카메라를 NowCamera에 넣음.

		TimeToNextCameraChange += NowCamera.TimeBetweenCameraChanges; // 배열 카메라 이동 쿨타임 초기화

		APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
		if (controller)
		{	
			/* 액터를 사용해서 카메라 이동 구현
			if ((controller->GetViewTarget() != CameraOne) && CameraOne != nullptr)
			{
				controller->SetViewTarget(CameraOne);
			}
			else if ((controller->GetViewTarget() != CameraTwo) && (CameraTwo != nullptr))
			{
				controller->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);
			}*/
			
			if ((controller->GetViewTarget() != NowCamera.Camera) && (NowCamera.Camera != nullptr))
			//NowCamera에 지정된 카메라가 아니면 NowCamera로 변경.
			{	
				if (NowCamera.SmoothBlendTime <= 0) //SmoothBlendTime가 음수거나 0이면
				{
					controller->SetViewTarget(NowCamera.Camera);
				}
				else //SmoothBlendTime이 양수이면
				{
					controller->SetViewTargetWithBlend(NowCamera.Camera, NowCamera.SmoothBlendTime);
				}
			}
		}

		NowCameraIndex++; //다음 배열에 있는 카메라를 불러옴.
		if (NowCameraIndex >= Cameras.Num())
		{
			NowCameraIndex = 0;
		}
	}

}

