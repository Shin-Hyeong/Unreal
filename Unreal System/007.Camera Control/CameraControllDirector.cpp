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

	const float TimeBetweenCameraChanges = 2.0f; //ī�޶� �̵��ϱ���� �ð�
	const float SmoothBlendTime = 0.75f; //ī�޶�� ī�޶� �̵��ϴ� �ð�. 0 �̸� �ٷ� �� �Ѿ

	TimeToNextCameraChange -= DeltaTime;
	if (TimeToNextCameraChange <= 0.0f)
	{	
		//AActor* NowCamera = Cameras[NowCameraIndex]; //�ش� �迭�� �ִ� ī�޶� NowCamera�� ����.
		FChangeCameraData NowCamera = Cameras[NowCameraIndex]; // ����ü �迭�� �ִ� ī�޶� NowCamera�� ����.

		TimeToNextCameraChange += NowCamera.TimeBetweenCameraChanges; // �迭 ī�޶� �̵� ��Ÿ�� �ʱ�ȭ

		APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
		if (controller)
		{	
			/* ���͸� ����ؼ� ī�޶� �̵� ����
			if ((controller->GetViewTarget() != CameraOne) && CameraOne != nullptr)
			{
				controller->SetViewTarget(CameraOne);
			}
			else if ((controller->GetViewTarget() != CameraTwo) && (CameraTwo != nullptr))
			{
				controller->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);
			}*/
			
			if ((controller->GetViewTarget() != NowCamera.Camera) && (NowCamera.Camera != nullptr))
			//NowCamera�� ������ ī�޶� �ƴϸ� NowCamera�� ����.
			{	
				if (NowCamera.SmoothBlendTime <= 0) //SmoothBlendTime�� �����ų� 0�̸�
				{
					controller->SetViewTarget(NowCamera.Camera);
				}
				else //SmoothBlendTime�� ����̸�
				{
					controller->SetViewTargetWithBlend(NowCamera.Camera, NowCamera.SmoothBlendTime);
				}
			}
		}

		NowCameraIndex++; //���� �迭�� �ִ� ī�޶� �ҷ���.
		if (NowCameraIndex >= Cameras.Num())
		{
			NowCameraIndex = 0;
		}
	}

}

