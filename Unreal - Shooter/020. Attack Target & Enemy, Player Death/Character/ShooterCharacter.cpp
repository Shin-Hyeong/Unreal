#include "Character/ShooterCharacter.h"

// Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Movement
#include "GameFramework/CharacterMovementComponent.h"

// Camera
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sound && Particle
#include "Kismet/GameplayStatics.h"

// Socket && Particle
#include "Engine/SkeletalMeshSocket.h"

// Particle
#include "Particles/ParticleSystemComponent.h"

// AnimMontage
#include "Animation/AnimMontage.h"

// AItem & Weapon & Ammo
#include "Items/Item.h"
#include "Items/Weapon.h"
#include "Items/Ammo.h"

// Collision
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Widget
#include "Components/WidgetComponent.h"

// Capsule
#include "Components/CapsuleComponent.h"

// Physical Material
#include "PhysicalMaterials/PhysicalMaterial.h"

// Surface
#include "Shooter/Shooter.h"

// HitInterface
#include "Character/BulletHitInterface.h"

// Enemy
#include "Character//Enemy.h"

// EnemyController
#include "Character/EnemyController.h"

// BlackboardComponent
#include "BehaviorTree/BlackboardComponent.h"

// Debug
#include "DrawDebugHelpers.h"


AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	// 공중에서 이동할 수 있는 비율(0 : 이동 못함. 1: 지상의 이동속도 동일)
	GetCharacterMovement()->AirControl = AirControlRate;
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	// 컨트롤러에 의해 회전 허용
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 75.f, 75.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// USpringArmComponent::SocketName : SpringArm의 시작 부분
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 컨트롤러에 의해 회전이 아닌 카메라붐이 움직이면서 같이 움직여야 함.
	Camera->bUsePawnControlRotation = false;

	// 장전할때 탄창을 들고 있을 손의 위치를 나타낼 SceneComponent
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));

	/** Create Interpolation Components*/
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(GetCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component1"));
	InterpComp1->SetupAttachment(GetCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component2"));
	InterpComp2->SetupAttachment(GetCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component3"));
	InterpComp3->SetupAttachment(GetCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component4"));
	InterpComp4->SetupAttachment(GetCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component5"));
	InterpComp5->SetupAttachment(GetCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component6"));
	InterpComp6->SetupAttachment(GetCamera());
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FPS = 1 / DeltaTime;

	// 카메라 부드럽게 움직임
	CameraInterp(DeltaTime);

	// 조준 시 마우스 감도 변경
	SetLookRate();

	// Crosshair 퍼짐 계산
	CalculateCrosshiarSpread(DeltaTime);
	
	// OverlappedCount를 확인해서 Item를 LineTrace로 감지함
	TraceForItems();

	// 일어날 때 or 웅크릴때 Capsule Height 변경
	InterpCapsuleHalfHeight(DeltaTime);
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EvenetInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
		AEnemyController* EnemyController = Cast<AEnemyController>(EvenetInstigator);
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerDeath"), true);
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
		EnhancedInputComponent->BindAction(GamepadLookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::GamepadLook);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::MouseLook);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterCharacter::Jump);
		EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AShooterCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Started, this, &AShooterCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AShooterCharacter::AimingButtonReleased);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &AShooterCharacter::SelectButtonPressed);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Completed, this, &AShooterCharacter::SelectButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AShooterCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AShooterCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(FAction, ETriggerEvent::Started, this, &AShooterCharacter::FKeyPressed);
		EnhancedInputComponent->BindAction(Swap1Action, ETriggerEvent::Started, this, &AShooterCharacter::OneKeyPressed);
		EnhancedInputComponent->BindAction(Swap2Action, ETriggerEvent::Started, this, &AShooterCharacter::TwoKeyPressed); 
		EnhancedInputComponent->BindAction(Swap3Action, ETriggerEvent::Started, this, &AShooterCharacter::ThreeKeyPressed); 
		EnhancedInputComponent->BindAction(Swap4Action, ETriggerEvent::Started, this, &AShooterCharacter::FourKeyPressed); 
		EnhancedInputComponent->BindAction(Swap5Action, ETriggerEvent::Started, this, &AShooterCharacter::FiveKeyPressed);

	}
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	if (bShouldPlayEquipSound)
	{
		StartEquipSoundTimer();
		if (Item->GetEquipSound())
		{
			PlaySound(Item->GetEquipSound());
		}
	}

	// 들고 있는 무기와 교체
	AWeapon* Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else // Inventory가 가득 차서 장착중인 무기와 교체해야함.
		{
			SwapWeapon(Weapon);
		}
	}
	else
	{
		// 총알 획득
		AAmmo* Ammo = Cast<AAmmo>(Item); 
		if (Ammo)
		{
			PickupAmmo(Ammo);
		}
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	// OverlappedItemCount가 음수가 되지 않도록 함
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else 
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	// 요구한 Index가 유효하면 해당 Index 정보 Get
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}
	// 유효하지 않으면 빈 FinterpLocation Get
	return FInterpLocation();
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	// 가장 적은 ItemCount을 가지고 있는 Index를 찾기 위한 
	// 처음 비교 대상 
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	// i = 0 -> WeaponInterp
	// 다 동일한 Item Count를 가지고 있으면 1번부터 순차적으로 나옴
	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	// Amount = -1 or 0 or 1
	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(
		PickupSoundTimer, 
		this, 
		&AShooterCharacter::ResetPickupSoundTimer,
		PickupSoundResetTime
	);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(
		EquipSoundTimer,
		this,
		&AShooterCharacter::ResetEquipSoundTimer,
		EquipSoundResetTime
	);
}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
}

void AShooterCharacter::Stun()
{
	if (Health <= 0.f) return;
	CombatState = ECombatState::ECS_Stunned;

	const int32 RandomSection = FMath::FRandRange(0.f, HitReactSectionName.Num() - 1);
	PlayMontage(HitReactMontage, HitReactSectionName[RandomSection]);
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetController());

	// Input Mapping Context 설정
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}

	// 애님인스턴스 설정
	AnimInstance = GetMesh()->GetAnimInstance();
	
	// 카메라 설정값 적용
	if (Camera)
	{
		CameraDefaultFOV = GetCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	// 기본 무기 생성 및 장착, Inventory 추가
	EquipWeapon(SpawnDefaultWeapon(), true);
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);

	// CustomDepth & Glow 비활성화
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();
	EquippedWeapon->SetCharacter(this);

	// 기본 총알 갯수 지급
	InitializeAmmoMap();
	// Interpolation을 위한 배열 초기화
	InitializeInterpLocations();
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{ 0.0f };

	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f) };

	// Capsule이 작아지면서 Character Mesh도 같이 내려가기 때문에 올려야함.
	// Delta : 웅크릴때 음수값, 일어날때 양수
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset{ 0.f, 0.f, -DeltaCapsuleHalfHeight };

	GetMesh()->AddLocalOffset(MeshOffset);
	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}

}

void AShooterCharacter::Die()
{
	if (bDead == false)
	{
		PlayMontage(DeathMontage, TEXT("DeathA"));
		bDead = true;
	}
}

void AShooterCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		// 해당 컨트롤러의 입력 기능 해제
		DisableInput(PC);
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	// Equipped Weapon의 Slot에 새로운 Weapon를 넣음.
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap, false);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	EAmmoType AmmoType = EquippedWeapon->GetAmmoType();
	// AmmoMap에서 AmmoType의 키값이 있는지 확인
	if (AmmoMap.Contains(AmmoType))
	{
		// AmmoType키에 맞는 값을 찾아서 비교
		return AmmoMap[AmmoType] > 0;
	}

	// AmmoMap에 없는 키값을 가지고 있으면
	return false;
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
	// AmmoType으로 AmmoMap에 검색
	// TMap.Find = 키값이 있으면 해당 키값의 pointer , 없으면 nullptr
	// TMap.Contains = 키값이 있으면 true , 없으면 false
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		// AmmoMap에서 해당 해당 AmmoType이 갖고 있는 총알 갯수를 가져옴
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetAmmoCount();

		// 총알 얻은 만큼 TMap에 업데이트
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	// 만약 들고 있는 총에 총알이 완전 비어 있다면 바로 충전
	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{ WeaponInterpComp, 0 };
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{ InterpComp1, 0 };
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{ InterpComp2, 0 }; 
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{ InterpComp3, 0 }; 
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{ InterpComp4, 0 }; 
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{ InterpComp5, 0 }; 
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{ InterpComp6, 0 }; 
	InterpLocations.Add(InterpLoc6);
}

EPhysicalSurface AShooterCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.f, 0.f, -400.f) };
	// 충돌이 있을 경우 Physical Material을 반환하는 것과 관련됨.
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);

	// 해당 Physical Material의 SurfaceType을 가져옴.
	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

/** Input */
void AShooterCharacter::Move(const FInputActionValue& value)
{
	const FVector2D MovementValue = value.Get<FVector2D>();
	// 게임패드 스틱 쏠림 현상 방지
	bool bInputX = MovementValue.X > GamepadSticklean || MovementValue.X < -GamepadSticklean;
	bool bInputY = MovementValue.Y > GamepadSticklean || MovementValue.Y < -GamepadSticklean;

	if (Controller != nullptr && (bInputX || bInputY))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDir, MovementValue.Y);
		AddMovementInput(RightDir, MovementValue.X);
	}
}

/** Input */
void AShooterCharacter::Jump()
{
	if (Controller != nullptr)
	{
		ACharacter::Jump();
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

/** input */
void AShooterCharacter::MouseLook(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * MouseTurnRate);
		AddControllerPitchInput(LookAxisVector.Y * MouseLookUpRate );
	}
}

/** Input */
void AShooterCharacter::GamepadLook(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	// 게임패드 스틱 쏠림 현상 방지
	bool bInputX = LookAxisVector.X > GamepadSticklean || LookAxisVector.X < -GamepadSticklean;
	bool bInputY = LookAxisVector.Y > GamepadSticklean || LookAxisVector.Y < -GamepadSticklean;

	if (Controller != nullptr && (bInputX || bInputY))
	{
		AddControllerYawInput(LookAxisVector.X * GamepadTurnRate * GetWorld()->GetDeltaSeconds());
		AddControllerPitchInput(LookAxisVector.Y * GamepadLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

/** Input */
void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		// AnimMontage의 Notify로 FireBullet를 호출함
		PlayMontage(HipFireMontage, FName("StartFire"));

		/* Call in AnimBlueprint */
		// FireBullet();

		// 발사로 인해 Crosshair 퍼짐 확장
		StartCrosshairBulletFire();

		// 총을 발사하여 총알 1개 소모
		EquippedWeapon->DecrementAmmo();

		StartFireTimer();

		PistolSlide();
	}

}

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading && 
		CombatState != ECombatState::ECS_Equipping &&
		CombatState != ECombatState::ECS_Stunned)
	{
		Aim();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
	if (bCrouching == false)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (TraceHitItem && TraceHitItem->GetIsOwner() == false)
	{
		TraceHitItem->SetIsOnwer(true);
		TraceHitItem->StartItemCurve(this);
		TraceHitItem = nullptr;
	}
}

void AShooterCharacter::SelectButtonReleased()
{

}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (GetCharacterMovement()->IsFalling() == false)
	{
		// true -> false, false -> true
		bCrouching = !bCrouching;
		if (bCrouching)
		{
			GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
			GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
			GetCharacterMovement()->GroundFriction = BaseGroundFriction;
		}
	}
}

void AShooterCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void AShooterCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AShooterCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AShooterCharacter::FourKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void AShooterCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

void AShooterCharacter::ExchangeInventoryItems(int32 CurrentItemIdex, int32 NewItemIndex)
{
	// if ((CurrentItemIdex == NewItemIndex) || (NewItemIndex >= Inventory.Num()) || (CombatState != ECombatState::ECS_Unoccupied) ) return;

	// 무기 교체 도중에도 다른 무기로 교체할 수 있도록 조건 변경
	const bool bCanExchangeItems = (CurrentItemIdex != NewItemIndex) && 
		(NewItemIndex < Inventory.Num()) && 
		((CombatState == ECombatState::ECS_Unoccupied) || (CombatState == ECombatState::ECS_Equipping));

	if (bCanExchangeItems)
	{
		// 조준 취소
		if (bAiming)
		{
			StopAiming();
		}

		// 무기 교체
		AWeapon* OldEquippedWeapon = EquippedWeapon;
		AWeapon* NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewWeapon, true);
		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);

		// AnimMontage 재생
		CombatState = ECombatState::ECS_Equipping;
		PlayMontage(EquipMontage, FName("Equip"));

		// 무기 교체 Sound 재생
		PlaySound(NewWeapon->GetEquipSound());
	}
}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	// TArray에 Size 내에서 빈 곳이 있으면 그 곳을 지목
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	// SIze가 다 차있으면 맨 뒷 자리 지목
	if (Inventory.Num() < INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}

	// Inventory가 가득 참
	return -1;
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot{ GetEmptyInventorySlot() };
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}

/** Function can call in Blueprint */
void AShooterCharacter::FireBullet()
{
	if (EquippedWeapon == nullptr) return;

	// 총소리
	PlaySound(EquippedWeapon->GetFireSound());

	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	
	if (BarrelSocket && EquippedWeapon->GetMuzzleFlash())
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		// 발사 이펙트
		SpawnParticle(EquippedWeapon->GetMuzzleFlash(), SocketTransform);

		// Line Trace
		FHitResult BeamHitResult;

		// BeamEnd set in Function GetBeamEndLocation
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(), 
			BeamHitResult
		);

		if (bBeamEnd)
		{
			bool bSpawnImpact = true;
			// 총알 궤도 Effect
			BulletBeam(SocketTransform, BeamHitResult.Location);

			// BulletHitInterface를 상속 받고 있는 Actor가 맞으면
			// Enemy
			if (BeamHitResult.GetActor())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.GetActor());

				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitResult);
					bSpawnImpact = false;
				}

				AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.GetActor());
				if (HitEnemy)
				{
					int32 Damage{};
					// 헤드샷 인지
					bool bHeadShot{ false };

					// 헤드샷 일때
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						Damage = EquippedWeapon->GetHeadShotDamage();

						float DamageMin = Damage / 0.8f;
						float DamageMax = Damage / 1.2f;
						Damage = static_cast<int32>(FMath::FRandRange(DamageMin, DamageMax));
						bHeadShot = true;
					}
					// 몸 샷일떄
					else
					{
						Damage = EquippedWeapon->GetDamage();

						float DamageMin = Damage / 0.8f;
						float DamageMax = Damage / 1.2f;
						Damage = static_cast<int32>(FMath::FRandRange(DamageMin, DamageMax));
					}

					UGameplayStatics::ApplyDamage( 
						BeamHitResult.GetActor(), 
						Damage, 
						GetController(), 
						this,
						UDamageType::StaticClass() 
					);
					// 데미지 표시
					HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location, bHeadShot);
				}
			}
			// 탄착 흔적 Effect
			if (bSpawnImpact)
			{
				SpawnParticle(ImpactParticles, BeamHitResult.Location); 
			} 
		}
	}
}

/** Play Montage */
void AShooterCharacter::PlayMontage(UAnimMontage* Montage, const FName& SectionName)
{
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

/** Camera smoothly Zoom In/Out & When Crouching, Camera Up/Down */
void AShooterCharacter::CameraInterp(float DeltaTime)
{
	// 조준 했을 때 카메라 줌 / 아웃
	if (bAiming)
	{
		// FInterpTo(A, B, DeltaTime, Speed) : A에서 B까지의 사이값 내에서 DeltaTime * Speed 백분율 만큼 이동한 값
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomSpeed);
	}
	GetCamera()->SetFieldOfView(CameraCurrentFOV);
}

/** Set BaseTurnRate and BaseLookUpRate based on Aiming */
void AShooterCharacter::SetLookRate()
{
	if (bAiming)
	{
		GamepadTurnRate = GamepadAimingTurnRate;
		GamepadLookUpRate = GamepadAimingLookUpRate;

		MouseTurnRate = MouseAimingTurnRate;
		MouseLookUpRate = MouseAimingLookUpRate;
	}
	else
	{
		GamepadTurnRate = GamepadHipTurnRate;
		GamepadLookUpRate = GamepadHipLookUpRate;

		MouseTurnRate = MouseHipTurnRate;
		MouseLookUpRate = MouseHipLookUpRate;
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}

	return nullptr;
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());

			if (EquippedWeapon == nullptr)
			{
				// -1 == 아직 무기를 장착하지 않음. DefaultWeaponAnimation만 재생
				EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
			}
			else if (bSwapping == true)
			{
				// DefaultWeaponAnimation과 교체하는 Weapon Slot의 Animation 재생
				EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
			}


			EquippedWeapon = WeaponToEquip;
			// 아이템을 현재 장착중인 상태로 변경
			// 변경된 상태에 맞게 Collision 설정
			EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
		}
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->SetIsOnwer(false);
		EquippedWeapon->ThrowWeapon();
		EquippedWeapon->SetCharacter(nullptr);
	}
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;
	
	// 여분의 총알이 있는지 확인, 현재 탄창이 가득 차 있는지 확인
	if (CarryingAmmo() && !(EquippedWeapon->ClipIsFull()))
	{
		CombatState = ECombatState::ECS_Reloading;
		// 재장전 모션 재생
		PlayMontage(ReloadMontage, EquippedWeapon->GetReloadMontageSection());

		if (bAiming)
		{
			StopAiming();
		}
	}
}

// ReloadMontage의 Notify에 의해서 호출
void AShooterCharacter::FinishReloading()
{
	// 재장전 중에 기절하면 장전 취소
	if (CombatState == ECombatState::ECS_Stunned) return;
	CombatState = ECombatState::ECS_Unoccupied; 

	if (EquippedWeapon == nullptr) return;
	const EAmmoType AmmoType{ EquippedWeapon->GetAmmoType() };

	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (AmmoMap.Contains(AmmoType))
	{
		// Player가 가지고 있는 여분 총알 갯수
		int32 CarriedAmmo = AmmoMap[AmmoType];
		
		// 빈 탄창을 채울 양
		const int32 MagEmptySpace = 
			EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		// 채워야 하는 양이 가지고 있는 양보다 많으면
		if (MagEmptySpace > CarriedAmmo)
		{
			// 가지고 있는 양 모두 사용
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			//  AmmoMap 업데이트
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			// 필요한 양만 사용
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			//  AmmoMap 업데이트
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AShooterCharacter::FinishEquipping() 
{
	if (CombatState == ECombatState::ECS_Stunned) return;

	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
}

// ReloadMontage의 Notify에서 호출
void AShooterCharacter::GrapClip()
{
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;
	
	// 장착중인 무기의 탄창 Bone 번호 
	int32 ClipBoneIndex{EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName())};

	// 탄창의 처음 Transform 값 저장
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	// ShooterCharacter의 Hand_L에 HandSceneComponent를 부착함.
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
	// HandSceneComponent의 위치는 탄창의 처음 위치로 함.
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);
}

// ReloadMontage의 Notify에서 호출
void AShooterCharacter::ReplaceClip()
{
	if (EquippedWeapon == nullptr) return;

	EquippedWeapon->SetMovingClip(false);
}

/** Spawn Particle by Transform */
UParticleSystemComponent* AShooterCharacter::SpawnParticle(UParticleSystem* ParticleName, const FTransform& SpawnTransform)
{
	if (ParticleName)
	{
		return UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleName, SpawnTransform);
	}
	return nullptr;
}

/** Spawn Particle by FVector*/
UParticleSystemComponent* AShooterCharacter::SpawnParticle(UParticleSystem* ParticleName, const FVector& SpawnVector)
{
	if (ParticleName)
	{
		return UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleName, SpawnVector);
	}
	return nullptr;
}

/** Bullet Trajectory*/
void AShooterCharacter::BulletBeam(const FTransform& SocketTransform, const FVector& BeamEndPoint)
{
	UParticleSystemComponent* Beam = SpawnParticle(BeamParticles, SocketTransform);
	if (Beam)
	{
		// BeamParticle에 있는 Target FVector변수를 BeamEndPoint로 업데이트함.
		Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
	}
}

/** Play Sound */
void AShooterCharacter::PlaySound(USoundBase* SoundName)
{
	if (SoundName)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundName, GetActorLocation());
	}
}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

/** Line Trace, Screen To World*/
bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector OutBeamLocation;

	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		// Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else // no crosshair trace hit
	{
		// OutBeamLocation is the End Location for the line trace
	}


	/** Line Trace by Gun Barrel */
		// Perform a second Trace, this time from the gun barrel
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	// Start To End : 총구부터 CrosshairLineTrace에서 충돌 지점까지의 거리
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };

	// MuzzleSocketLocation에서 OutBeamLocation으로 LineTrace를 하면 
	// 가끔씩 거리가 부족하여 충돌이 일어나지 않을 수 있음

	// 총구에서 StartToEnd의 1.25배를 더하여 확실하게 해당 물체에 닺도록 함
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility
	);
	// 총구로 부터 End까지 LineTrace를 통해 충돌이 없으면
	if (OutHitResult.bBlockingHit == false)
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}

	return true;
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// Get Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get Screen space location of crosshair
	FVector2D CrosshairLocation{ ViewportSize.X / 2.f, ViewportSize.Y / 2.f };
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;


	// Get World positon and Direction of Crosshairs
	// WorldPosition, WorldDirection : Output
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld( 
		PlayerController,
		CrosshairLocation,
		CrosshairWorldPosition, // Output
		CrosshairWorldDirection // Output
	);

	// LineTrace
	if (bScreenToWorld)
	{
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * WeaponRange };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		// 아이템을 바라보면 Widget이 보이도록 함
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());

			// Inventory Slot에 HighlightAnimation를 재생 On/Off
			const AWeapon* TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			if (TraceHitWeapon)
			{
				if (HighlightedSlot == -1)
				{
					// Highlight 중인 Slot이 없으면 Hightlight를 킴.
					HighlightInventorySlot();
				}
			}
			// Weapon이 감지가 안되면
			else
			{
				// Highlight된 Slot이 있다면
				if (HighlightedSlot != -1)
				{
					// Highlight를 끔.
					UnHighlightInventorySlot();
				}
			}
			
			// Interpolation 중인 Item를 감지햇으면 무시하도록 함.
			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				if (Inventory.Num() >= INVENTORY_CAPACITY)
				{
					// Inventory 가득 참
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					// Inventory 여유 있음
					TraceHitItem->SetCharacterInventoryFull(false);
				}

				// Show Item's piuk up Widget
				// AreaSphere 안에 있는 Item만 보이도록 함
				TraceHitItem->SetWidgetVisibility(true);
				// Custom Depth 활성화
				TraceHitItem->EnableCustomDepth();
			}

			// 이전 frame에서 item를 감지한 상태이면
			if (TraceHitItemLastFrame)
			{
				// 현재 frame과 이전 frame과 감지하고 있는 대상이 다르면
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// 이전 frame에서 감지했던 Item의 Widget를 비활성화 함.
					// 혹은 현재 frame에 감지 실패하여 HitItem이 Null이면
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					// Custom Depth 비활성화
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}

			// Cast가 실패하면 HitItem에 Null이 들어감
			// Cast가 성공해서 item이 들어가면 다음 Frame과 비교하기 위해서 저장
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	// Item를 감지하여 Widget를 활성화한 상태에서 AreaSphere를 벗어나 bShouldTraceForItems가 false가 되어
	// LineTrace를 못하는 상황에서 이미 감지 하고 있던 Item의 Widget를 비활성화 시킴.
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		// Custom Depth 비활성화
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

void AShooterCharacter::CalculateCrosshiarSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.0f, GetCharacterMovement()->MaxWalkSpeed};
	FVector2D VelocityMultiplierRange{ 0.0f, 1.0f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	// WalkSpeedRange(0 ~ 600) Clamp Range (0 ~ 1)
	// Calculate Crosshair velocity factor
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// Calculate Crosshair in air factor
	// CrosshairInAirFactor를 Target까지 DeltaTime * InterpSpead 만큼 변화
	if (GetCharacterMovement()->IsFalling())
	{
		// Spread the crosshairs slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else // Character is on the ground
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// Calculate crosshair Aim factor
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else // Not Aiming
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.f);
	}

	// True ShootTimeDuration(0.05s) after firing
	if (bFiringBullet)
	{
		CrosshairShooingFactor = FMath::FInterpTo(CrosshairShooingFactor, 0.5f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShooingFactor = FMath::FInterpTo(CrosshairShooingFactor, 0.0f, DeltaTime, 60.f);
	}

	// 0.5f : 기본 Crosshair 퍼짐정도
	// CrosshairSpreadMultiplier : 0.5f + (0 ~ 1.0f) + (0 ~ 2.25f) - (0 ~ 0.6f)
	CrosshairSpreadMultiplier = 0.5f
		+ CrosshairVelocityFactor // 이동
		+ CrosshairInAirFactor // 공중
		- CrosshairAimFactor // 조준
		+ CrosshairShooingFactor; // 사격
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	// ShootTimeDuration(0.05s) 이후 FinishCrossHairBulletFire()를 1번 호출 함
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, 
		this, 
		&AShooterCharacter::FinishCrosshairBulletFire, 
		ShootTimeDuration
	);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(
		AutoFireTimer,
		this,
		&AShooterCharacter::AutoFireReset,
		EquippedWeapon->GetAutoFireRate()
	);
}

void AShooterCharacter::AutoFireReset()
{
	if (CombatState == ECombatState::ECS_Stunned) return;
	CombatState = ECombatState::ECS_Unoccupied;

	if (EquippedWeapon == nullptr) return;
	// 총알이 남아 있으면
	if (WeaponHasAmmo())
	{
		// 발사 버튼을 누르고 있는 상태이면
		if (bFireButtonPressed && EquippedWeapon->GetAutomatic())
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
	}
}

void AShooterCharacter::PistolSlide()
{
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
	{
		EquippedWeapon->StartSlideTimer();
	}
}

void AShooterCharacter::SetMouseX(float Sensitivity)
{
	MouseTurnRate = Sensitivity;
}

void AShooterCharacter::SetMouseY(float Sensitivity)
{
	MouseLookUpRate = Sensitivity;
}