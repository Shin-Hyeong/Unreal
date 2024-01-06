#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "InputActionValue.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

class UInputAction;
class UInputMappingContext;

UCLASS() 
class UDEMYCPP_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();

	// Input Action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	UInputAction *MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	UInputAction* LookAction;

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

	// Spring Arm
	// Spring Arm을 사용하는 이유
	// - 1) 동적으로 카메라의 위치를 조절하기 쉬워짐
	// - 2) 카메라가 벽을 만나면 벽을 통과하는게 아니라 객체 기준으로 확대됨.
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

protected:
	virtual void BeginPlay() override;

	// 이동 함수
	void Move(const FInputActionValue& Value);
	// 카메라 회전
	void Look(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
