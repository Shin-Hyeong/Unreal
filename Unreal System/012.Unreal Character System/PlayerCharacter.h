// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;

class UGroomComponent;

UCLASS()
class UDEMYCPP_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(EditAnywhere, Category = "Input Action")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input Action")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input Action")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input Action")
	UInputMappingContext* IMC;


protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	// Hair
	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	// Eyebrows
	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;
};
