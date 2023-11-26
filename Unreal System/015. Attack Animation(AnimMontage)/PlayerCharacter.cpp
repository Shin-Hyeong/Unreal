#include "Player/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// build.cs에 "EnhancedInput" 모듈 추가해야함.
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// CharacterMovement와 관련된 요소를 수정하기 위해서
#include "GameFramework/CharacterMovementComponent.h"
// Hair(Groom)를 사용하기 위해서 -> build.cs에 "HairStrandsCore", "Niagara" 모듈 추가해야함.
#include "GroomComponent.h"

// Item이 가지고 있는 OverlappingItem를 확인하기 위해
#include "Actor/Item.h"
// Weapon이 가지고 있는 Equip()를 호출하기 위해
#include "Actor/Weapons/Weapon.h"

// AnimMontage를 사용하기 위해
#include "Animation/AnimMontage.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// 캐릭터의 움직일때 이동 방향을 바라보도록 함.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 캐릭터가 회전할 때 속도 (Pitch, Yaw, Roll)
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// RootComponent가 ControllerRotation의 입력으로 회전하지 않도록 함.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 500.f;

	// SpringArm이 ControllerRotation의 입력값을 받을 수 있도록함.
	// -> RootComponent는 가만히 있지만 SpringArm은 회전할 수 있음.
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	// Hair 생성
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	// Mesh는 비공개(private)된 변수이기 떄문에 GetMesh()를 통해 접근해야한다.
	Hair->SetupAttachment(GetMesh());
	// head라는 소켓(무언가 부착할수 있음)을 만든다.
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
	}

}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	const FVector2D MovementValue = value.Get<FVector2D>();

	if (Controller != nullptr && ActionState == EActionState::EAS_Unoccupied)
	{
		// 카메라 시점 기준으로 앞/뒤, 왼/오를 계산하지 않고 객체의 기준으로 이동함.
		// FVector Forward = GetActorForwardVector();
		// FVector Right = GetActorRightVector();

		// Controller가 바라보는 방향을 가져옴.
		const FRotator ControlRotation = GetControlRotation();
		// Pitch, Yaw, Roll
		// 캐릭터의 방향을 영향을 주는 것은 Yaw만 영향을 줌.
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		
		// GetUnitAxis.X : X -> 앞으로 바라보는 방향으로 단위 벡터를 구함.
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDir, MovementValue.Y);
		AddMovementInput(RightDir, MovementValue.X);

	}
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void APlayerCharacter::Equip(const FInputActionValue& value)
{	
	// Item.h에 있는 OverlappingItem를 AWeapon 형태로 변환. 실패하면 null
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	// 무기가 있는지 확인
	if (OverlappingWeapon)
	{
		// Weapon.h이 가지고 있는 Equip() 호출
		// Player의 Mesh의 RightHandSocket에 Attach함.
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));

		// 무기를 획득하면 Player의 상태 변경
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		GetCharacterMovement()->MaxWalkSpeed = One_HandWalkSpeed;
	}
}

void APlayerCharacter::Attack(const FInputActionValue& value)
{	
	// Player의 상태가 Unoccupied 이고 캐릭터가 빈 손이 아닐 때만 공격 가능.
	const bool bCanAttack = CanAttack();

	if (bCanAttack)
	{
		PlayAttackMontage();
		// 공격중에는 Player 상태를 Attacking 상태로 변경
		ActionState = EActionState::EAS_Attacking;
	}
}

bool APlayerCharacter::CanAttack()
{
	// Player의 상태가 Unoccupied(비 공격 상태) 이고 캐릭터가 빈 손이 아닐 때만 공격 가능.
	return (ActionState == EActionState::EAS_Unoccupied) &&
		(CharacterState != ECharacterState::ECS_Unequipped);
}

// Montage를 통해서 공격하는 기능을 따로 만든 함수
void APlayerCharacter::PlayAttackMontage()
{
	// AnimInstance를 호출하여 Montage 관련 함수를 사용한다.
	// GetAnimInstance() : UAnimInstance*를 반환
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// AnimInstance와 AttackMontage가 Null인지 확인
	if (AnimInstance && AttackMontage)
	{
		// Montage_Play(재생할 Montage, 재생 속도(1.f), Montage_Play()의 반환타입 설정(float Montage 재생길이),
		// Montage의 몇 초부터 재생할지(0), 기존에 재생하고 있던 Montage를 정지시키고 재생시킬지(true) )
		AnimInstance->Montage_Play(AttackMontage);

		// FMath::RandRange(Min, Max) : Min ~ Max 중 난수 반환.
		// 반환값으로 double, float, int64, int32를 반환함.
		const int8 Selection = static_cast<int8>(FMath::RandRange(0, 1));

		// switch 문에서 SelectionName를 채울 것이기 때문에 비워둠.
		FName SectionName = FName();

		// 난수에 따라 재생할 Montage Section를 지정함.
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		}
		// Montage_JumpToSelection(FName SectionName, UAnimMontage)
		// 재생할 Montage와 Section를 변경함.
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APlayerCharacter::AttackEnd()
{	
	// ActionState를 공격 가능한 상태로 되돌림.
	ActionState = EActionState::EAS_Unoccupied;
}
