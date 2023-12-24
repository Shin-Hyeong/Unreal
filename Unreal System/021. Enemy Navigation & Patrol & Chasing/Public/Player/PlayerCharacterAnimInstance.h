#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

// Enum CharacterState를 사용하기 위해서
#include "CharacterType.h"

#include "PlayerCharacterAnimInstance.generated.h"

/*
AnimInstance는 Animation BP를 부모로 하는 클래스이다.
AnimationBP는 게임이 실행되지 않더라도 계속 실시간으로 반응하기 한다.
AnimInstance를 작업 중 nullptr같은 에러 사항을 실시간으로 반응하기 때문에
UE 에디터가 크래시를 일으킬 수 있기 때문에 AnimInstance를 작업할 때는 에디터를 끄고 작업한다.

AnimInstance 변경은 ABP - ClassSetting - Detail에서 변경가능
 */

UCLASS()
class UDEMYCPP_API UPlayerCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// 플레이어의 class이름으로 생성해야함.
	UPROPERTY(BlueprintReadOnly)
	class APlayerCharacter* PlayerCharacter;

	// 플레이어의 Movement 
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* PlayerCharacterMovement;

	// 플레이어의 이동속도
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	// 플레이어가 공중에 있는지 확인
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	// Category에 |를 사용하여 하위 카테고리를 생성할 수 있다.
	UPROPERTY(BlueprintReadOnly, Category = "Movement | State")
	ECharacterState CharacterState;

public:
	// beginplay()
	virtual void NativeInitializeAnimation() override;
	// Tick()
	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
