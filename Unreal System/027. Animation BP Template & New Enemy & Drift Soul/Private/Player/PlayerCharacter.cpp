#include "Player/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Mesh의 Collision을 변경하기 위해서
#include "Components/StaticMeshComponent.h"

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

// HUD를 통해 PlayerOverlay를 제어하기 위해
#include "HUD/PlayerHUD.h"
#include "HUD/PlayerOverlay.h"

// Player의 체력, 스테미나, 골드, 경험치를 알아내기 위함
#include "Components/AttributeComponent.h"
// 골드와 경험치를 획득하기 위해
#include "Actor/Soul.h"
#include "Actor/Treasure.h"


APlayerCharacter::APlayerCharacter()
{
	// Tick 함수
	PrimaryActorTick.bCanEverTick = true;
	// 캐릭터의 움직일때 이동 방향을 바라보도록 함.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 캐릭터가 회전할 때 속도 (Pitch, Yaw, Roll)
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	// Enemy의 Mesh가 Trigger의 Overlap 이벤트가 일어나도록 설정함.
	GetMesh()->SetGenerateOverlapEvents(true); 

	// RootComponent가 ControllerRotation의 입력으로 회전하지 않도록 함.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoomLength = CameraBoom->TargetArmLength;

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

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString FPS = FString::Printf(TEXT("FPS : %f"), 1 / DeltaTime);
	GEngine->AddOnScreenDebugMessage(
		1,
		1.0f,
		FColor::Blue,
		FPS
	);
	if (Attributes && PlayerOverlay && ActionState != EActionState::EAS_Dead)
	{
		Attributes->RegenStamina(DeltaTime);
		Attributes->RegenHealth(DeltaTime);
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHeatlh();

	return DamageAmount;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Press_EKey);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Zoom);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Dodge);
	}

}

void APlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollision(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void APlayerCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void APlayerCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		PlayerOverlay->SetSouls(Attributes->GetSouls());
	}
}

void APlayerCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		PlayerOverlay->SetGold(Attributes->GetGold());
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Enemy의 PawnSensing에서 Tag를 통해 Player를 감지하기 위해
	Tags.Add(FName("EngageableTarget"));

	// HUD를 접근하기 위해 PlayerController가 필요함
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	InitializePlayerOverlay(PlayerController);

	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	const FVector2D MovementValue = value.Get<FVector2D>();

	// 만약 Move 함수에 if() 밖에 기능이 있을 떄 실행하지 못하도록 예외 처리
	if (!IsUnoccupied()) return;

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

void APlayerCharacter::Jump(const FInputActionValue& value)
{
	if (IsUnoccupied())
	{
		// ACharacter가 가지고 있는 Jump()
		Super::Jump();
	}
}

void APlayerCharacter::Press_EKey(const FInputActionValue& value)
{	
	// Item.h에 있는 OverlappingItem를 AWeapon 형태로 변환. 실패하면 null
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	// 무기가 있는지 확인
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}

		// Weapon.h이 가지고 있는 Equip() 호출
		// Player의 Mesh의 RightHandSocket에 Attach함.
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

		// 무기를 획득하면 Player의 상태 변경
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;

		// 무기를 획득하고 있으면 Item의 Sphere 안에 캐릭터가 계속 존재하기 때문에 비워주어야함.
		OverlappingItem = nullptr;

		// 획득한 무기를 장착된 무기 변수에 넣음.
		EquippedWeapon = OverlappingWeapon;
		GetCharacterMovement()->MaxWalkSpeed = One_HandWalkSpeed;
	}
	else 
	{
		// 공격하고 있지 않는 상태 && 무기를 손에 들고 있는 상태 = true
		// 무장해제
		if (CanDisarm()) 
		{
			EquipState = EEquipState::EES_Equipping;
			PlayEquipMontage(FName("Unequip")); 
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}

		// 공격하고 있지 않는 상태 && 무기가 손에 없는 상태 = true
		// 무장
		else if (CanArm())
		{
			EquipState = EEquipState::EES_Equipping;
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
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

void APlayerCharacter::Zoom(const FInputActionValue& value)
{
	float Zoom = value.Get<float>();

	if (Zoom < 0 && CameraBoomLength < 800)
	{
		CameraBoom->TargetArmLength += ZoomSpeed;
	}
	else if (Zoom > 0 && CameraBoomLength > 200)
	{
		CameraBoom->TargetArmLength -= ZoomSpeed;
	}

	CameraBoomLength = CameraBoom->TargetArmLength;
}

void APlayerCharacter::Dodge(const FInputActionValue& value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	if (Attributes  && PlayerOverlay && Attributes->GetStamina() > Attributes->GetDodgeCost())
	{
		PlayDodgeMontage();
		ActionState = EActionState::EAS_Dodge;
		Attributes->UseStamina(Attributes->GetDodgeCost());
	}
}

// ABP에서 Attack이 끝날 쯤 notify를 통해서  AttackEnd 함수를 호출하도록 함
void APlayerCharacter::AttackEnd()
{	
	// ActionState를 공격 가능한 상태로 되돌림.
	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

// 공격 가능 상태 확인
bool APlayerCharacter::CanAttack()
{
	// Player의 상태가 Unoccupied(비 공격 상태) 이고 캐릭터가 빈 손이 아닐 때만 공격 가능.
	return (ActionState == EActionState::EAS_Unoccupied) &&
		(CharacterState != ECharacterState::ECS_Unequipped);
}

void APlayerCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMehsCollision();
}

// 무장해재 가능 확인
bool APlayerCharacter::CanDisarm()
{
	// 공격하고 있지 않는 상태 && 무기를 손에 들고 있는 상태 && 무장 모션이 완료 상태(진행 X) = true
	return (ActionState == EActionState::EAS_Unoccupied) &&
		(CharacterState != ECharacterState::ECS_Unequipped) &&
		(EquipState == EEquipState::EES_Equipped);
}

// 무장 가능확인
bool APlayerCharacter::CanArm()
{
	// 공격하고 있지 않는 상태 && 무기가 손에 없는 상태 && 
	// 장착할 무기가 있는지 확인 && 무장 모션이 완료 상태(진행 X)= true
	return (ActionState == EActionState::EAS_Unoccupied) &&
		(CharacterState == ECharacterState::ECS_Unequipped) &&
		(EquipState == EEquipState::EES_Equipped) &&
		EquippedWeapon;
}

// 무장해제/무장 모션 재생
void APlayerCharacter::PlayEquipMontage(const FName& SectionName)
{
	// AnimInstance를 호출하여 Montage 관련 함수를 사용한다.
	// GetAnimInstance() : UAnimInstance*를 반환
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// AnimInstance와 AttackMontage가 Null인지 확인
	if (AnimInstance && EquipMontage) 
	{
		// Montage_Play(재생할 Montage, 재생 속도(1.f), Montage_Play()의 반환타입 설정(float Montage 재생길이),
		// Montage의 몇 초부터 재생할지(0), 기존에 재생하고 있던 Montage를 정지시키고 재생시킬지(true) )
		AnimInstance->Montage_Play(EquipMontage); 

		// Montage_JumpToSelection(FName SectionName, UAnimMontage)
		// 재생할 Montage와 Section를 변경함.
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);;
	}
}

void APlayerCharacter::Equipped()
{
	EquipState = EEquipState::EES_Equipped;
	ActionState = EActionState::EAS_Unoccupied;
}

// 무기를 손 -> 등 뒤로 이동
void APlayerCharacter::Disarm() 
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMashToSocket(GetMesh(), FName("SpineSocket"));
	}
}

// 무기를 등 뒤 -> 손으로 이동
void APlayerCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMashToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void APlayerCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool APlayerCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void APlayerCharacter::InitializePlayerOverlay(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
		if (PlayerHUD)
		{
			PlayerOverlay = PlayerHUD->GetPlayerOverlay();
			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetStaminaBarPercent(1.0f);
				PlayerOverlay->SetGold(0);
				PlayerOverlay->SetSouls(0);
			}
		}
	}
}

void APlayerCharacter::SetHUDHeatlh()
{
	if (PlayerOverlay && Attributes)
	{
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}