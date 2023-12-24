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
class AWeapon;

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

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomSpeed = 20.f;

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
	UInputAction* ZoomAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputMappingContext* IMC;

	/*
	* 캐릭터 상태 기본값 설정
	*/

	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EEquipState EquipState = EEquipState::EES_Equipped;

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

	// 장착한 무기 변수
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	/*
	* Animation Montage
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	/*
	*  Debug
	*/

	UPROPERTY(VisibleAnywhere, Category = Zoom)
	float CameraBoomLength;

protected:
	virtual void BeginPlay() override;

	/*
	*  Callbacks for input
	*/
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Press_EKey(const FInputActionValue& value);
	void Attack(const FInputActionValue& value);
	void Zoom(const FInputActionValue& value);

	/*
	*  Play Montage functions
	*/
	// 공격 모션 재생
	void PlayAttackMontage();

	// 공격 가능 상태 확인
	bool CanAttack();

	// 무장해제/무장 모션 재생
	void PlayEquipMontage(const FName& SectionName);

	// 무장해재 가능 확인
	bool CanDisarm();

	// 무장 가능확인
	bool CanArm();

	// 무기를 손 -> 등 뒤로 이동
	UFUNCTION(BlueprintCallable)
	void Disarm();

	// 무기를 등 뒤 -> 손으로 이동
	UFUNCTION(BlueprintCallable)
	void Arm();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ABP에서 Attack이 끝날 쯤 notify를 통해서  AttackEnd 함수를 호출하도록 함
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void Equipped();
	
	// OverlappingItem이 private이기 때문에 접근 할수 있는 Set 함수를 구현한다.
	// inline : 함수가 호출 되었을 때 함수 본체를 찾는 간접적인 작업을 줄일 수 있다
	// 단, Set, Get같은 단순한 작업만 가능하다.
	// 하지만 컴파일러가 최종적으로 inline를 시킬지 결정한다.
	// FORCEINLINE : 프로그래머가 강제적으로 inline를 결정한다.
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	// private인 CharacterState를 접근할 수 있도록 Set 함수를 구현한다.
	// const를 사용하여 이 함수를 통해 어떠한 값도 변경하지 않겠다는 것을 인지시킴.
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	// 무기의 Box Collision을 비활성화/활성화 시키는 함수
	// ECollisionEnabled::Type : 충돌 활성화 유형(Query Only, NoCollision, ...)을 
	// 매개변수로 받을 수 있고 ECollisionEnabled Enum를 호출 가능
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	
};
