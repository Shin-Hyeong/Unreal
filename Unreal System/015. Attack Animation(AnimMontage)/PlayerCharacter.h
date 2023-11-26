// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
// Enum CharacterState를 사용하기 위해서
#include "CharacterType.h"
#include "PlayerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;

// 헤어와 눈썹을 부착하기 위한 Groom
class UGroomComponent;

// 아이템을 저장하기 위한 AItem
class AItem;

// 공격 모션을 사용하기 위한 AnimMontage
class UAnimMontage;

UCLASS()
class UDEMYCPP_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(EditAnywhere, Category = "Move")
	float One_HandWalkSpeed = 400.f;

private:
	/*
	*  Input Action
	*/
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
	UInputMappingContext* IMC;

	// 캐릭터 상태 기본값 설정
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	// Camera Setting
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

	// VisibleInstanceOnly : BP에서 볼 수 없고 Instance에서만 확인 가능 
	// 아이템을 획득하면 바로 장착하지 않고 저장하기 위해 따로 만듬.
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	/*
	* Animation Montage
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

protected:
	virtual void BeginPlay() override;

	/*
	*  Callbacks for input
	*/
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Equip(const FInputActionValue& value);
	void Attack(const FInputActionValue& value);

	/*
	*  Play Montage functions
	*/
	// 공격 모션 재생
	void PlayAttackMontage();

	// 공격 가능 상태 확인
	bool CanAttack();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ABP에서 Attack이 끝날 쯤 notify를 통해서  AttackEnd 함수를 호출하도록 함
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	
	// OverlappingItem이 private이기 때문에 접근 할수 있는 Set 함수를 구현한다.
	// inline : 함수가 호출 되었을 때 함수 본체를 찾는 간접적인 작업을 줄일 수 있다
	// 단, Set, Get같은 단순한 작업만 가능하다.
	// 하지만 컴파일러가 최종적으로 inline를 시킬지 결정한다.
	// FORCEINLINE : 프로그래머가 강제적으로 inline를 결정한다.
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	// private인 CharacterState를 접근할 수 있도록 Set 함수를 구현한다.
	// const를 사용하여 이 함수를 통해 어떠한 값도 변경하지 않겠다는 것을 인지시킴.
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
