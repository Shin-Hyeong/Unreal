#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TestEnhancedInput.generated.h"

/*Use Component*/
class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class FORENHANCEDINPUT_API ATestEnhancedInput : public ACharacter
{
	GENERATED_BODY()

	/*Camera Arm*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom;

	/*Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

public:
	ATestEnhancedInput();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CameraZoom")
	float ZoomVariable;

	/*SkeletalMesh*/
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkeletalMesh")
	//USkeletalMeshComponent *SkeletalMesh;

	/*InputMappingContext*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext *TestMappingContext;

	/*InputAction*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *ZoomAction;

	/*virtual Function*/
	virtual void Jump() override;

protected:
	virtual void BeginPlay() override;
	/*Function Move*/
	void Move(const FInputActionValue &Value);

	/*Fuction Look*/
	void Look(const FInputActionValue &Value);

	/*Function Camera Zoom*/
	void Zoom(const FInputActionValue &Value);

public:	
	//virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
