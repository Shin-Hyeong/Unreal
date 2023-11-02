#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TestEnhancedInput.generated.h"

class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class FORENHANCEDINPUT_API ATestEnhancedInput : public ACharacter
{
	GENERATED_BODY()

public:
	ATestEnhancedInput();

	/*SkeletalMesh*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkeletalMesh")
	USkeletalMeshComponent *SkeletalMesh;

	/*InputMappingContext*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext *TestMappingContext;

	/*InputAction*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *MoveAction;

protected:
	virtual void BeginPlay() override;

	/*Function Move*/
	void Move(const FInputActionValue &Value);

public:	
	//virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
