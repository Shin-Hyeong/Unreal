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

	FVector CurrentVelocity; //���� ���� ��ġ
	bool bGrowing; //ũ�⸦ Ű���� ���� ����
	float InputTime; //���� �ð�

	/*Config Test*/
	UPROPERTY(config)
	float ExampleVarialbe;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	UStaticMeshComponent* StaticMesh; 
	//��������Ʈ -> Ʈ������(Positon, Scale, Rotation)�� ����ġ(RootComponent�� ����ϱ� ����) ����
	//���忡 ��ġ ������ ������Ʈ�� �θ�Ŭ������ �ȴ�.
	//�浹�� �������� ��� ������ ���̷� ���Ǳ⵵ �Ѵ�.

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override; //BeginPlay �Լ�

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override; //Tick �Լ�

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Pawn Class�� �⺻���� ������ �Լ�, �Է¿� ���� ������ ó���ϱ� ���� �Լ�, ���ε�ó���� �ַ� �Ѵ�.

	void MoveForward(float AxisValue); 	//�յ����� �̵� �Լ�
 
	void MoveRight(float AxisValue);	//�¿� �̵� �Լ�

	void StartGrowing();				//Scale Ȯ�� �Լ�

	void StopGrowing();					//Scale ��� �Լ�

};
