#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
//InputAction를 위해서 전처리
#include "ThirdPesronCppCharacter.generated.h"

UCLASS(config=Game) //.ini 파일로 일부 프로퍼티를 환경설정 파일에 저장
class AThirdPesronCppCharacter : public ACharacter
{
	GENERATED_BODY()

	/*Camera Boom 생성*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//AllowPrivateAccess -> private 접근지시자를 가진 요소를 에디터 디테일 패널에서 접근할 수 있게 할지 여부 결정.
	class USpringArmComponent* CameraBoom;
	//SpringArm -> 카메라를 자식으로 하여 게임 상 카메라의 위치를 편하게 설정 가능하게끔 해줌.
	//삼인칭 시점에서 카메라가 레벨의 다른 오브젝트에 가려지는 상황을 자동으로 처리할 수 있다.

	/*Follow Camera 생성*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/*입력 매핑 컨텍스트 생성 */
	//입력 매핑 컨텍스트 : 입력 액션(행동이나 역할에 대한 정보, 문을 연다, 장비를 착용한다)와 사용자의 입력값을 연결한다.
	//각각 우선순위를가 있어서 같은 키를 입력해도 어떤 액션을 나갈지 정할 수 있다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/*Input Action(Jump) 생성 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/*Input Action(Move) 생성 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/*Input Action(Look) 생성 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	AThirdPesronCppCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

