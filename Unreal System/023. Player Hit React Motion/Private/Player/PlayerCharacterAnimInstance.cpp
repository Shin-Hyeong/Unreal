#include "Player/PlayerCharacterAnimInstance.h"

// PlayerCharacter에 플레이어가 조종하는 객체를 참조 시키기 위해
#include "Player/PlayerCharacter.h"
// C++의 수학라이브러리
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"


// beginPlay()
void UPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Dynamic_cast() : 상속의 안정성을 확인하는 Cast. 실패하면 nullptr
	// C++의 Dynamic_cast<>()은 UE에서 Cast<>()으로 이루어진다.
	// TryGetPawnOwner : APawn 타입을 반환함. 해당 Animation BP를 가지고 있는 Pawn를 알려줌.
	// Cast<APlayerCharacter>(TryGetPawnOwner()) : 받은 Pawn을 APlayerCharacter 타입으로 변환
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());

	// PlayerCharacter가 nullptr이 아니면(cast이 성공하면)
	if (PlayerCharacter)
	{
		// PlayerCharacter가 가지고 있는 이동구성요소를 가져옴.
		// PlayerCharacter.cpp에서 "GameFramework/CharacterMovementComponent.h"를 참조하기 때문에 사용가능.
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

// Tick
void UPlayerCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	// PlayerCharacterMovement이 nullptr이 아니면
	if (PlayerCharacterMovement)
	{
		// VSizeXY : X와 Y값만 가진 Vector의 Length를 반환함
		GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerCharacterMovement->Velocity);
		// 플레이어가 공중에 떠있는지(점프 중인지) 확인
		IsFalling = PlayerCharacterMovement->IsFalling();
		// CharacterState를 계속 확인함.
		CharacterState = PlayerCharacter->GetCharacterState();
	}
}
