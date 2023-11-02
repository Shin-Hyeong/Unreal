#include "ThirdPesronCppCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AThirdPesronCppCharacter

AThirdPesronCppCharacter::AThirdPesronCppCharacter()
{
	/*캡슐 형태로 콜리전 생성*/
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);//가로, 세로

	//bUseControllerRotation : 폰의 컨트롤 회전을 관장하는 속성.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	//true로 설정하면 컨트롤 회전에 따라 폰 메시 자체가 회전한다.
	//해당 코드에서 false로하여 컨트롤 회전이 주어지면 캐릭터를 회전하지 않지만 카메라는 회전하도록 만듦.

	/*캐릭터 이동 설정.*/
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//bOrientRotationToMovement : 폰을 움직일때 바라보고 있는 방향으로 폰을 회전시킬지 여부를 결정.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	//Camera Boom 생성. (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	//bUsePawnControlRotation : 컨트롤 회전에 따라 실시간으로 폰의 스프링 암의 트랜스폼이 회전함.
	//SpringArm에는 카메라가 연결되어 있기 문에 회전하면 카메라가 폰을 공전하듯이 거리를 유지하면서 회전한 것 처럼 보인다.

	// Camera 생성.
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	//Camera를 따로 컨트롤 회전 제어를 주어 SpringArm의 영향에서 벗어나도록 한다.

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AThirdPesronCppCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//입력 매핑 컨텍스트 생성
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPesronCppCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		//Trigger : Modifier를 통해 입력 받는 값을 어떻게 활용할지 정함.
		//JumpAction 변수와 입력 값을 누르고 있으면 Jump와 바인딩
		//ACharacter::Jump()는 엔진에서 이미 구현된 함수를 사용.
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		//JumpAction 변수와 입력 값을 땟을 때 InputAction StopJumping과 바인딩

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPesronCppCharacter::Move);
		//MoveAction 변수와 입력값을 누르고 있을 때 InputAction Move 바인딩

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPesronCppCharacter::Look);
		//LookAction 변수와 입력값을 누르고 있을 때 InputAction Look 바인딩
	}

}

void AThirdPesronCppCharacter::Move(const FInputActionValue& Value)
{
	/*입력값을 Vector2D 형태로 받아온다.*/
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)// 컨트롤러가 존재한다면
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		//컨트롤 회전을 받아온다.
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		//컨트롤 회전값에서 Z축 기준으로 회전하는 값이 Yaw값만 추출한다.
		//Roll, Pitch, Yaw

		/*앞뒤 이동 값 구하기*/
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//FRotationMatrix : 추출한 Yaw값을 동차좌표계를 이용한 행렬로 변환
		//변환된 행렬에서 Pawn 기준으로 X값을 추출한다.
	
		/*좌우 이동 값 구하기*/
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		/*앞뒤으로 이동 계산*/
		AddMovementInput(ForwardDirection, MovementVector.Y);
		//AddMovementInput(입력값, 어떻게 움직일지 계산 - 양수 이면 진행방향, 음수면 반대 방향)
		// UPawnMovementComponent 계열 컴포넌트를 포함시켜야한다.

		/*좌우 이동 계산*/
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AThirdPesronCppCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




