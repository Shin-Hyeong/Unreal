// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "GameFramework/Controller.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UInputAction;
class UInputMappingContext;

UCLASS() class UDEMYCPP_API ABird : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABird();

	// Input Action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	UInputAction *MoveAction;

	// InputMappingContext
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	UInputMappingContext *MappingContext;


private:
	// 충돌을 감지할 Capsule
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	// Animation를 가지고 있는 SkeletalMesh
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
