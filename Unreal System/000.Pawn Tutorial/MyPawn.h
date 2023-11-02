// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

UCLASS(config = Game)
class CPPTEST_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

	FVector CurrentVelocity; //현재 벡터 위치
	bool bGrowing; //크기를 키울지 말지 선택
	float InputTime; //누른 시간

	/*Config Test*/
	UPROPERTY(config)
	float ExampleVarialbe;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	UStaticMeshComponent* StaticMesh; 
	//씬컴포넌트 -> 트랜스폼(Positon, Scale, Rotation)과 어태치(RootComponent로 사용하기 좋음) 지원
	//월드에 배치 가능한 컴포넌트를 부모클래스가 된다.
	//충돌과 렌더링이 없어서 가벼워 더미로 사용되기도 한다.

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override; //BeginPlay 함수

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override; //Tick 함수

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Pawn Class가 기본으로 가지는 함수, 입력에 대한 설정을 처리하기 위한 함수, 바인딩처리를 주로 한다.

	void MoveForward(float AxisValue); 	//앞뒤으로 이동 함수
 
	void MoveRight(float AxisValue);	//좌우 이동 함수

	void StartGrowing();				//Scale 확대 함수

	void StopGrowing();					//Scale 축소 함수

};
