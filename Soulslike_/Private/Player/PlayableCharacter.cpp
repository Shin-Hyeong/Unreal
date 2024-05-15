#include "Player/PlayableCharacter.h"

#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "Components/AttributesComponent.h"
#include "HUD/PlayerOverlay.h"
#include "HUD/PlayerHUD.h"
#include "HUD/SettingUI.h"
#include "HUD/GameOver.h"
#include "Items/BaseItem.h"
#include "Items/Weapon.h"
#include "Items/Exp.h"
#include "Enemy/Enemy.h"

#include "DrawDebugHelpers.h"

APlayableCharacter::APlayableCharacter()
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 900.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = CameraArmLength;
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
}

void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bShowFPS)
		PrintFPS(DeltaTime);
	if (IsDead() == false)
	{
		PlayBoringMontage(DeltaTime);
		CameraTrace();
		RegenAttributes(DeltaTime);
		LockOn();

		if (PowerAttackCool >= 0.0f)
			PowerAttackCoolDown(DeltaTime);

		// Player Stop and RunTrigger is Toggle, Change Walk
		ChangeStateToWalk();
	}
}

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayableCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayableCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayableCharacter::Jump);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &APlayableCharacter::Zoom);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &APlayableCharacter::Interaction);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayableCharacter::Dodge);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayableCharacter::Attack);
		EnhancedInputComponent->BindAction(RMAttackAction, ETriggerEvent::Triggered, this, &APlayableCharacter::RMAttack);
		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &APlayableCharacter::EnemyTargeting);
		EnhancedInputComponent->BindAction(SettingAction, ETriggerEvent::Started, this, &APlayableCharacter::OnSettingUI);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayableCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayableCharacter::Walk);
		EnhancedInputComponent->BindAction(ComboAction, ETriggerEvent::Started, this, &APlayableCharacter::PlayComboAttackMontage);
	}
}

void APlayableCharacter::Die(const FName& HitDirection)
{
	Tags.Add(FName("Dead"));
	CharacterState = EPlayerState::EPS_Death;

	int32 SectionNum = PlayRandomMontageSection(DeathMontage, DeathName);

	TEnumAsByte<EDeathPose> Pose(SectionNum);
	if (Pose < EDeathPose::EDP_DeathPoseMax)
		DeathPose = Pose;
}

void APlayableCharacter::GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType)
{
	if (IsDead() == true) return;
	Super::GetHit(GetHitter, ImpactPoint, Hitter, DamageType);

	SetWeaponBoxCollision(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		CharacterState = EPlayerState::EPS_HitReact;
	}
}

void APlayableCharacter::SetOverlappingItem(ABaseItem* Item)
{
	OverlappingItem = Item;
}

void APlayableCharacter::AddExp(AExp* Exp)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddExp(Exp->GetExp());
		PlayerOverlay->SetExp(Attributes->GetExp());
	}
}

void APlayableCharacter::SetSettingValue(float CameraValueX, float CameraValueY, bool RunToggle)
{
	CameraSensitivityX = CameraValueX;
	CameraSensitivityY = CameraValueY;
	bRunToggle = RunToggle;
}

void APlayableCharacter::SetCameraArmLength(float Length)
{
	CameraBoom->TargetArmLength = Length;
	CameraArmLength = Length;
}

float APlayableCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();

	return DamageAmount;
}

void APlayableCharacter::EnemyIsDead(AEnemy* Enemy)
{
	UE_LOG(LogTemp, Warning, TEXT("Call EnemyIsDead"));
	if (OverlapEnemy == Enemy)
		OverlapEnemy = nullptr;
	if (TargetEnemy == Enemy)
	{
		TargetEnemy = nullptr;
		bIsLockOnTarget = false;
		LockOffState();
	}
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayWakeUpMontage();

	Tags.Add(FName("Player"));
	PlayerController = Cast<APlayerController>(GetController());
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	InitializePlayerUI();

	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}

	GEngine->UEngine::SetMaxFPS(90.f);
}

void APlayableCharacter::Move(const FInputActionValue& value)
{
	MovementValue = value.Get<FVector2D>();
	if (!IsUnoccupied() || IsJumping()) return;

	StopBoring();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementValue.Y);
		AddMovementInput(RightDirection, MovementValue.X);

		BoringEnd();
	}
}

void APlayableCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * CameraSensitivityX);
		AddControllerPitchInput(LookAxisVector.Y * CameraSensitivityY);
	}
}

void APlayableCharacter::Jump()
{
	if (!IsUnoccupied()) return;

	StopBoring();

	Super::Jump();
	BoringEnd();
}

void APlayableCharacter::Zoom(const FInputActionValue& value)
{
	float ZoomIO = value.Get<float>();

	if (ZoomIO > 0 && CameraBoom->TargetArmLength > MinCameraArmLength)
	{
		CameraBoom->TargetArmLength -= ZoomSpeed;
	}
	else if (ZoomIO < 0 && CameraBoom->TargetArmLength < MaxCameraArmLength)
	{
		CameraBoom->TargetArmLength += ZoomSpeed;
	}
}

void APlayableCharacter::Interaction()
{
	StopBoring();

	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
			EquippedWeapon->Destroy();

		OverlappingWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		SetEquippedState();
		GetCharacterMovement()->MaxWalkSpeed = EquippedWalkSpeed;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}
}

void APlayableCharacter::Run()
{
	// Run Trigger
	if (bRunToggle == false)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		bRunning = true;
		UE_LOG(LogTemp, Warning, TEXT("Run"));

		if (bIsLockOnTarget)
			LockOffState();
	}
	// Run Toggle
	else if (bRunToggle == true)
	{
		// Walk -> Run
		if (bRunning == false)
		{
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			bRunning = true;
			UE_LOG(LogTemp, Warning, TEXT("Toggle Run"));

			if (bIsLockOnTarget)
				LockOffState();
		}
		// Run -> Walk
		else if (bRunning == true)
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			bRunning = false;
			UE_LOG(LogTemp, Warning, TEXT("Toggle Walk"));

			if (bIsLockOnTarget)
				LockOnState();
		}
	}
}

void APlayableCharacter::Walk()
{
	// Run Toggle
	if (bRunToggle == true) return;

	// Run Trigger
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bRunning = false;
	UE_LOG(LogTemp, Warning, TEXT("Walk"));

	if (bIsLockOnTarget)
		LockOnState();
}

void APlayableCharacter::Dodge()
{
	StopBoring();

	if (CanDodge() == false || IsJumping()) return;

	if (bIsLockOnTarget || CharacterState == EPlayerState::EPS_Attacking)
		DodgeDirection();

	CharacterState = EPlayerState::EPS_Dodge;
	UseStaminaCost(Attributes->GetDodgeCost());
	bCanRegenStamina = false;

	PlayMontageSection(DodgeMontage, "Dodge");
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
}

void APlayableCharacter::Attack()
{
	if (!Attributes) return;
	const bool bCanAttack = CanAttack(Attributes->GetAttackCost());

	if (bCanAttack)
	{
		CharacterState = EPlayerState::EPS_Attacking;
		bCanRegenStamina = false;
		UseStaminaCost(Attributes->GetAttackCost());
		PlayerDamageType = 0; // 일반 공격

		if (bIsLockOnTarget)
		{
			FRotator TargetRotation = RotationToTarget(GetActorLocation());
			LookAtTarget(TargetRotation);
		}

		PlayAttackMontage();
	}
}

void APlayableCharacter::RMAttack()
{
	if (!Attributes || PowerAttackCool > 0) return;
	if (CanAttack(Attributes->GetPwAttackCost()) && !IsJumping())
	{
		CharacterState = EPlayerState::EPS_PWAttacking;
		bCanRegenStamina = false;
		UseStaminaCost(Attributes->GetPwAttackCost());
		PlayerDamageType = 1; // 강 공격

		PowerAttackCool = PowerAttackMaxCool;
		PlayerOverlay->SetCoolDownPercent(1.0f);

		// 락온 상태 일때 타겟을 바라보도록 함
		if (bIsLockOnTarget)
		{
			FRotator TargetRotation = RotationToTarget(GetActorLocation());
			LookAtTarget(TargetRotation);
		}
		// 달리기 형태가 토글일때 걷는 속도로 변환시킴.
		if (bRunToggle)
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			bRunning = false;
			UE_LOG(LogTemp, Warning, TEXT("Toggle Walk"));

			if (bIsLockOnTarget)
				LockOnState();
		}

		// 무기 마다 다른 강 공격 모션
		FName Weapon = WeaponName[EquippedWeapon->GetWeaponNumber()];
		PlayMontageSection(RMAttackMontage, Weapon);
	}
}

void APlayableCharacter::LockOn()
{
	if (bIsLockOnTarget)
	{
		LockOnTarget();
	}
}

void APlayableCharacter::CameraTrace()
{
	// if (TargetEnemy) return;
	UWorld* World = GetWorld();
	if (World && CameraManager)
	{
		const FVector Start = CameraManager->GetCameraLocation();
		const FVector End = Start + (CameraManager->GetCameraRotation().Vector() * TargetingLenght);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery3); // Pawn

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		FHitResult HitResult;

		UKismetSystemLibrary::BoxTraceSingleForObjects(
			this,
			Start,
			End,
			BoxTraceExtent,
			CameraManager->GetCameraRotation(),
			ObjectTypes,
			false,
			ActorsToIgnore,
			bShowCameraTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			HitResult,
			true
		);

		AEnemy* LookTarget = Cast<AEnemy>(HitResult.GetActor());

		if (CanLockOnEnemy(LookTarget))
		{
			OverlapEnemy = LookTarget;
		}
	}
}

void APlayableCharacter::EnemyTargeting()
{
	// Lock On
	if (OverlapEnemy && TargetEnemy == nullptr && bIsLockOnTarget == false)
	{
		TargetEnemy = OverlapEnemy;
		TargetEnemy->ShowHealthBar();
		OverlapEnemy = nullptr;
		bIsLockOnTarget = true;
		LockOnState();
	}

	// Lock Off
	else if (TargetEnemy && bIsLockOnTarget == true)
	{
		TargetEnemy->HideHealthBar();
		TargetEnemy = nullptr;
		bIsLockOnTarget = false;
		LockOffState();
	}
}

void APlayableCharacter::LockOnTarget()
{
	FRotator LockOnCameraRotation = RotationToTarget(CameraManager->GetCameraLocation());
	FRotator LockOnCharacterRotation = RotationToTarget(GetActorLocation());

	// 카메라 고정
	PlayerController->SetControlRotation(LockOnCameraRotation);

	// 캐릭터 시점 고정
	// 회피 & 뛸 때는 예외
	if (CharacterState != EPlayerState::EPS_Dodge && bRunning == false)
		LookAtTarget(LockOnCharacterRotation);
}

void APlayableCharacter::LookAtTarget(FRotator Look)
{
	GetRootComponent()->SetWorldRotation(FRotator(0.f, Look.Yaw, 0.f));
}

FRotator APlayableCharacter::RotationToTarget(FVector Start)
{
	FRotator RotationToTarget = UKismetMathLibrary::FindLookAtRotation(
		Start,
		TargetEnemy->GetActorLocation()
	);

	return RotationToTarget;
}

void APlayableCharacter::LockOnState()
{
	if (CharacterWeaponState == ECharacterState::ECS_Unequipped)
		CharacterWeaponState = ECharacterState::ECS_LockOnUnequipped;
	else if (CharacterWeaponState == ECharacterState::ECS_EquippedOneHandedWeapon)
		CharacterWeaponState = ECharacterState::ECS_LockOnEquippedOneHandedWeapon;
	else if (CharacterWeaponState == ECharacterState::ECS_EquippedTwoHandedWeapon)
		CharacterWeaponState = ECharacterState::ECS_LockOnEquippedTwoHandedWeapon;
}

void APlayableCharacter::LockOffState()
{
	if (CharacterWeaponState == ECharacterState::ECS_LockOnUnequipped)
		CharacterWeaponState = ECharacterState::ECS_Unequipped;
	else if (CharacterWeaponState == ECharacterState::ECS_LockOnEquippedOneHandedWeapon)
		CharacterWeaponState = ECharacterState::ECS_EquippedOneHandedWeapon;
	else if (CharacterWeaponState == ECharacterState::ECS_LockOnEquippedTwoHandedWeapon)
		CharacterWeaponState = ECharacterState::ECS_EquippedTwoHandedWeapon;
}

void APlayableCharacter::PlayWakeUpMontage()
{
	if (bPlayWakeUp)
	{
		CharacterState = EPlayerState::EPS_WakeUp;
		PlayMontageSection(StartWakeUpMontage, "WalkUp");
	}
}

void APlayableCharacter::PlayBoringMontage(float DeltaTime)
{
	BoringTime += DeltaTime;
	if (BoringTime >= MaxBoringTime && bPlayingBoringMontage == false)
	{
		PlayMontageSection(BoringMontage, "Boring");
		bPlayingBoringMontage = true;

		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
	}
}

void APlayableCharacter::PlayComboAttackMontage()
{
	if (!Attributes || PowerAttackCool > 0) return;
	if (CanAttack(Attributes->GetComboAttackCost()) && !IsJumping())
	{
		CharacterState = EPlayerState::EPS_Attacking;
		bCanRegenStamina = false;
		UseStaminaCost(Attributes->GetComboAttackCost());
		PlayerDamageType = 0; // 일반 공격 연속

		PowerAttackCool = PowerAttackMaxCool;
		PlayerOverlay->SetCoolDownPercent(1.0f);

		// 락온 상태 일때 타겟을 바라보도록 함
		if (bIsLockOnTarget)
		{
			FRotator TargetRotation = RotationToTarget(GetActorLocation());
			LookAtTarget(TargetRotation);
		}
		// 달리기 형태가 토글일때 걷는 속도로 변환시킴.
		if (bRunToggle)
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			bRunning = false;
			UE_LOG(LogTemp, Warning, TEXT("Toggle Walk"));

			if (bIsLockOnTarget)
				LockOnState();
		}

		// 무기 마다 다른 강 공격 모션
		FName Weapon = WeaponName[EquippedWeapon->GetWeaponNumber()];
		FString Name = Weapon.ToString();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Name);
		PlayMontageSection(ComboAttackMontage, Weapon);
	}
}

void APlayableCharacter::ChangeStateToWalk()
{
	if (bRunToggle == true && UKismetMathLibrary::VSizeXY(GetCharacterMovement()->Velocity) == 0 && bRunning == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player stop, Change WalkMode"));
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		bRunning = false;

		if (bIsLockOnTarget)
			LockOnState();
	}
}

void APlayableCharacter::WakeUpEnd()
{
	CharacterState = EPlayerState::EPS_Unoccupied;
}

void APlayableCharacter::BoringEnd()
{
	UWorld* World = GetWorld();

	if (World && bPlayingBoringMontage && CharacterWeaponState != ECharacterState::ECS_Unequipped)
	{
		AWeapon* Weapon = World->SpawnActor<AWeapon>(
			WeaponClass[EquippedWeapon->GetWeaponNumber()],
			GetActorLocation(),
			GetActorRotation()
		);
		Weapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = Weapon;
	}

	BoringTime = 0.0f;
	bPlayingBoringMontage = false;
}

void APlayableCharacter::DodgeDirection()
{
	if (bRunning == true) return;

	float YawRotation = GetActorRotation().Yaw;

	// 1, 2 사분면은 x의 lerp:(0,90) 을 통해 값을 얻고
	// 3, 4 사분면은 y가 - 1일때만 적용시켜 회전 기준으로 후방을 기준으로 회전시킨다.

	if (MovementValue.Y < 0)
	{
		YawRotation += 180.f;
		YawRotation -= (MovementValue.X * 90.f);
	}
	else
	{
		YawRotation += (MovementValue.X * 90.f);
	}

	GetRootComponent()->SetWorldRotation(FRotator(0.f, YawRotation, 0.f));
	MovementValue = FVector2D(0.f, 0.f);
}

void APlayableCharacter::DodgeEnd()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	StopMontage(DodgeMontage);
	bCanRegenStamina = true;
	CharacterState = EPlayerState::EPS_Unoccupied;
}

void APlayableCharacter::ReactEnd()
{
	CharacterState = EPlayerState::EPS_Unoccupied;
	bCanRegenStamina = true;
}

void APlayableCharacter::StopBoring()
{
	if (bPlayingBoringMontage)
	{
		StopMontage(BoringMontage);
	}
}

void APlayableCharacter::AttackEnd()
{
	Super::AttackEnd();

	bCanRegenStamina = true;
	CharacterState = EPlayerState::EPS_Unoccupied;
}

void APlayableCharacter::InitializePlayerUI()
{
	if (PlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
		if (PlayerHUD)
		{
			// Player Overlay
			PlayerOverlay = PlayerHUD->GetPlayerOverlay();
			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetStaminaBarPercent(1.0f);
			}
			// Setting UI
			SettingUI = PlayerHUD->GetPlayerSettingUI();
			if (SettingUI)
			{
				ESlateVisibility OffSlateVisibility = ESlateVisibility::Collapsed;
				SettingUI->SetVisibility(OffSlateVisibility);
			}
			// Game Over UI
			GameOverUI = PlayerHUD->GetPlayerGameOver();
			if (GameOverUI)
			{
				ESlateVisibility OffSlateVisibility = ESlateVisibility::Collapsed;
				GameOverUI->SetVisibility(OffSlateVisibility);
			}
		}
	}
}

void APlayableCharacter::RegenAttributes(float DeltaTime)
{
	if (Attributes && PlayerOverlay && CharacterState != EPlayerState::EPS_Death)
	{
		if (bCanRegenHealth)
			RegenHealth(DeltaTime);
		if (bCanRegenStamina)
			RegenStamina(DeltaTime);
	}
}

void APlayableCharacter::SetHUDHealth()
{
	if (PlayerOverlay && Attributes)
	{
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void APlayableCharacter::SetHUDStamina()
{
	if (PlayerOverlay && Attributes)
	{
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void APlayableCharacter::UseStaminaCost(float Cost)
{
	Attributes->UseStamina(Cost);
	PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
}

void APlayableCharacter::RegenHealth(float DeltaTime)
{
	Attributes->RegenHealth(DeltaTime);
	PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
}

void APlayableCharacter::RegenStamina(float DeltaTime)
{
	Attributes->RegenStamina(DeltaTime);
	PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
}

void APlayableCharacter::OnSettingUI()
{
	if (SettingUI)
	{
		UE_LOG(LogTemp, Warning, TEXT("On Setting UI"));
		ESlateVisibility OnSlateVisibility = ESlateVisibility::Visible;
		SettingUI->SetVisibility(OnSlateVisibility);
		SettingUI->SetSaveDate(CameraSensitivityX, CameraSensitivityY, bRunToggle, CameraArmLength, FPS);

		PlayerController->SetPause(true);
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}
}

void APlayableCharacter::PowerAttackCoolDown(float DeltaTime)
{
	PowerAttackCool -= DeltaTime;

	float Percent = FMath::GetRangePct(0.0f, PowerAttackMaxCool, PowerAttackCool);
	PlayerOverlay->SetCoolDownPercent(Percent);
}

void APlayableCharacter::VisibleGameOverUI()
{
	if (GameOverUI)
	{
		ESlateVisibility OnSlateVisibility = ESlateVisibility::Visible;
		GameOverUI->SetVisibility(OnSlateVisibility);

		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}
}

void APlayableCharacter::PrintFPS(float DeltaTime)
{
	FPS = 1 / DeltaTime;
	FString FPSStirng = FString::Printf(TEXT("FPS : %f"), FPS);
	GEngine->AddOnScreenDebugMessage(
		1,
		1.0f,
		FColor::Cyan,
		FPSStirng
	);
}

bool APlayableCharacter::IsUnoccupied()
{
	return CharacterState == EPlayerState::EPS_Unoccupied;
}

bool APlayableCharacter::IsEquipped()
{
	return CharacterWeaponState >= ECharacterState::ECS_EquippedOneHandedWeapon;
}

bool APlayableCharacter::CanAttack(float AttackType)
{
	return (Attributes->IsStaminaCost(AttackType)) && IsEquipped() && IsUnoccupied();
}

bool APlayableCharacter::CanDodge()
{
	return (Attributes->IsStaminaCost(Attributes->GetDodgeCost())) && (CharacterState == EPlayerState::EPS_Unoccupied || CharacterState == EPlayerState::EPS_Attacking);
}

void APlayableCharacter::SetEquippedState()
{
	if (bIsLockOnTarget == false)
	{
		CharacterWeaponState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
	else if (bIsLockOnTarget == true)
	{
		CharacterWeaponState = ECharacterState::ECS_LockOnEquippedOneHandedWeapon;
	}
}

bool APlayableCharacter::IsDead()
{
	return CharacterState == EPlayerState::EPS_Death;
}

bool APlayableCharacter::CanLockOnEnemy(AEnemy* LookTarget)
{
	return LookTarget && TargetEnemy == nullptr && LookTarget->IsDead() == false;
}