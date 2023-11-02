#include "TestEnhancedInput.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "InputAction.h"
#include "InputMappingContext.h"
/*AddOnScreenDebugMessage*/
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATestEnhancedInput::ATestEnhancedInput()
{	
	/*Variable Setting*/
	ZoomVariable = 15.0f;

	//PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	/*Character Setting*/
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	/*Found InputMappingContext*/
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContext(TEXT("/Game/Input/IMC_Test"));
	if (MappingContext.Succeeded()){ TestMappingContext = MappingContext.Object; }

	/*Found InputAction*/
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump(TEXT("/Game/Input/Action/IA_Jump"));
	if (IA_Jump.Succeeded()){ JumpAction = IA_Jump.Object; }
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Move(TEXT("/Game/Input/Action/IA_Move"));
	if (IA_Move.Succeeded()){ MoveAction = IA_Move.Object; }
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Look(TEXT("/Game/Input/Action/IA_Look"));
	if (IA_Look.Succeeded()){ LookAction = IA_Look.Object; }
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Zoom(TEXT("/Game/Input/Action/IA_Zoom"));
	if (IA_Zoom.Succeeded()){ ZoomAction = IA_Zoom.Object; }
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Distance(TEXT("/Game/Input/Action/IA_Distance"));
	if (IA_Distance.Succeeded()){ DistanceAction = IA_Distance.Object; }
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Attack(TEXT("/Game/Input/Action/IA_Attack"));
	if (IA_Attack.Succeeded()) { AttackAction = IA_Attack.Object; }
	static ConstructorHelpers::FObjectFinder<UAnimMontage> IA_SD_Attack1(TEXT("/Game/Character/Animation/Montage/AM_Close_Range_Attack_UP-Down"));
	if (IA_SD_Attack1.Succeeded()) { SD_Attack1 = IA_SD_Attack1.Object; }

	/*CameraBoom*/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;

	/*Camera*/
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	/*SkeletalMesh*/
	// SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("USkeletalMeshComponent"));
	// SkeletalMesh->SetupAttachment(RootComponent);
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Character/StaticMesh/Skeletal/X_Bot"));
	// if (MeshAsset.Succeeded() && MeshAsset.Object != NULL){
	// 	SkeletalMesh->SetSkeletalMesh(MeshAsset.Object);
	// 	SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -89.0f));
	// 	SkeletalMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	// 	SkeletalMesh->SetWorldScale3D(FVector(1.0f));
	// }

	/*Found AnimClass(Anim BluePrint)*/
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(TEXT("/Game/Character/Animation/ABP_Xbot"));
	if (AnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
	}

	/*Weapon Distance Change*/
	WeaponChange = true;
	WeaponChangeCoolDown = true;
	
	/*Attack-ing*/
	Attacking = false;
}

void ATestEnhancedInput::BeginPlay()
{
	Super::BeginPlay();
	/*Controller Cast PlayerController*/
	if (APlayerController *PlayerController = Cast<APlayerController>(GetController())){
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())){
			Subsystem->AddMappingContext(TestMappingContext, 0);
			
			// 게임모드를 게임 플레이와 UI를 띄울 수 있는 모드로 설정.
			PlayerController->SetInputMode(FInputModeGameAndUI());
		}
	}

}

//void ATestEnhancedInput::Tick(float DeltaTime){
//	Super::Tick(DeltaTime);
//}

void ATestEnhancedInput::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	/*BindAction*/
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)){
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Look);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Zoom);
		EnhancedInputComponent->BindAction(DistanceAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Distance);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Attack);
	}
}

/*Character Move*/
void ATestEnhancedInput::Move(const FInputActionValue &Value)
{
	/*MoveAction이 Bool형식임으로 MoveAction 값을 CurrentValue에 넣음.*/
	const FVector2D MovementValue = Value.Get<FVector2D>();
	if (Controller != nullptr && Attacking == false)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementValue.Y);
		AddMovementInput(RightDirection, MovementValue.X);

		if (MovementValue.Y > 0) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Forward"));
		else if (MovementValue.Y < 0) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Back"));
		if (MovementValue.X > 0) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Right"));
		else if (MovementValue.X < 0) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Left"));
	}
}

/*Camera Movement*/
void ATestEnhancedInput::Look(const FInputActionValue &Value){
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		if (LookAxisVector.Y > 0) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("LookUp"));
		else if (LookAxisVector.Y < 0) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("LookDown"));
		if (LookAxisVector.X > 0) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Turn Right"));
		else if (LookAxisVector.X < 0) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Turn Left"));

	}
}

/*Character Jump*/
void ATestEnhancedInput::Jump(){
	if (Attacking == false)
		Super::Jump();
	
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Jump"));
}

/*Camera Zoon In and Out*/
void ATestEnhancedInput::Zoom(const FInputActionValue &Value)
{
	float ZoomValue = Value.Get<float>();

	if (ZoomValue > 0 && CameraBoom->TargetArmLength > 250)
	{
		CameraBoom->TargetArmLength -= ZoomVariable;
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Zoom In"));
	}
	else if (ZoomValue < 0 && CameraBoom->TargetArmLength < 700)
	{
		CameraBoom->TargetArmLength += ZoomVariable;
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, TEXT("Zoom Out"));
	}
}

/*Weapon Distance Change*/
void ATestEnhancedInput::Distance(const FInputActionValue &Value)
{
	bool UIOnOff = Value.Get<bool>();
	if(WeaponChange && WeaponChangeCoolDown == true)
	{	
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, TEXT("Weapon Distance : Long"));
		WeaponChange = false;
		WeaponChangeCoolDown = false;
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ATestEnhancedInput::AdvanceTimer, 1.5f, true);
	}
	else if (!WeaponChange && WeaponChangeCoolDown == true)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, TEXT("Weapon Distance : Short"));
		WeaponChange = true;
		WeaponChangeCoolDown = false;
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ATestEnhancedInput::AdvanceTimer, 1.5f, true);
	}
}

/*CoolDown*/
void ATestEnhancedInput::AdvanceTimer()
{
	/*Weapon Distance Change CoolDown*/
	if (!WeaponChangeCoolDown)
	{
		WeaponChangeCoolDown = true;
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}

	/*Standing during Attack*/
	if (Attacking)
	{
		Attacking = false;
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}
}

/*Attack*/
void ATestEnhancedInput::Attack(const FInputActionValue &Value)
{	
	// Short Distance Attack
	if (WeaponChange && Attacking == false)
	{
		Attacking = true;
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ATestEnhancedInput::AdvanceTimer, 1.0f, true);
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, TEXT("Short Attack!!!!!!"));
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(SD_Attack1);
		}
	}
	// Long Distance Attack
	else if (!WeaponChange && Attacking == false)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, TEXT("Long Attack!!!!!!"));
	}
}
