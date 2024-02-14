// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// EnhancedInput 기능을 사용하기 위해
#include "InputActionValue.h"
// Enum CharacterState를 사용하기 위해서
#include "CharacterType.h"
// Player와 Enemy의 공통된 기능을 사용하기 위해
#include "BaseCharacter.h"
// 물건을 획득 할때 사용됨.
#include "Interfaces/PickUpInterface.h"

#include "PlayerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class ASoul;
class ATreasure;
class UAnimMontage; 
class UPlayerOverlay;

// 헤어와 눈썹을 부착하기 위한 Groom
class UGroomComponent;

UCLASS()
class UDEMYCPP_API APlayerCharacter : public ABaseCharacter, public IPickUpInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	/** </AActor> */

	/** <APawn> */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** </APawn> */

	/** <IHitInterface -> BaseCharacter> */
	// _Implementation 붙여 Native Function으로 만듦.
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface -> BaseCharacter> */

	/** <IPickUpInterface> */
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	/** </IPickUpInterface> */

	// private인 CharacterState를 접근할 수 있도록 Set 함수를 구현한다.
	// inline : 함수가 호출 되었을 때 함수 본체를 찾는 간접적인 작업을 줄일 수 있다
	// 단, Set, Get같은 단순한 작업만 가능하다.
	// 하지만 컴파일러가 최종적으로 inline를 시킬지 결정한다.
	// FORCEINLINE : 프로그래머가 강제적으로 inline를 결정한다.
	// const를 사용하여 이 함수를 통해 어떠한 값도 변경하지 않겠다는 것을 인지시킴.
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/**  Callbacks for input*/
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Press_EKey(const FInputActionValue& value);
	void Attack(const FInputActionValue& value);
	void Zoom(const FInputActionValue& value);
	void Dodge(const FInputActionValue& value);

	/**  Combat */
	/** <BaseCharacter> */
	virtual void AttackEnd() override; 
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override; 
	virtual void Die_Implementation() override;
	/** </BaseCharacter> */
	bool CanDisarm(); 
	bool CanArm(); 
	void PlayEquipMontage(const FName& SectionName); 

private:
	UFUNCTION(BlueprintCallable)
	void Equipped();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	bool IsUnoccupied();

	/** Attributes */
	void InitializePlayerOverlay(APlayerController* PlayerController);
	void SetHUDHeatlh();

public:
	UPROPERTY(EditAnywhere, Category = "Move")
	float One_HandWalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomSpeed = 20.f;

private:
	/**  Input Action */
	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* EquipAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* ZoomAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* DodgeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputMappingContext* IMC;

	/** Character State */
	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EEquipState EquipState = EEquipState::EES_Equipped;

	/** Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	float CameraBoomLength;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	/** Weapon */
	// VisibleInstanceOnly : BP에서 볼 수 없고 Instance에서만 확인 가능 
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	/** Montage */
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	/** Attributes */
	UPROPERTY()
	UPlayerOverlay* PlayerOverlay;
};
