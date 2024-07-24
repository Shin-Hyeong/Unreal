#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"

class AnimInstance;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class USoundBase;
class UParticleSystem;
class UAnimMontage; 
class AItem;
class AWeapon;
class AAmmo;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),

	ECS_MAX UMETA(DisplayName = "Default MAX")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	// Interpolation할 때 Location을 알기 위한 Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	// 해당 Scene Comp으로 Interp되고 있는 아이템 갯수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

// Delegate 이름, 파라미터 이름 2개
// FEquipItemDelegate, CurrentSlotIndex, NewSlotIndex
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();

	/* <AActor>*/
	virtual void Tick(float DeltaTime) override;
	/* </AActor>*/

	/* <ACharacter>*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/* </ACharacter>*/

	/**
		Weapon
	*/
	// AItem에서 사용
	void GetPickupItem(AItem* Item);

	/**
		Crosshair
	*/
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
	
	/**
		OverlapItems
	*/
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	// OverlappedItemCount를 더하거나 빼서 0이 되면 bShouldTraceForItems를 false로 변경한다
	void IncrementOverlappedItemCount(int8 Amount);

	/**
		Interpolation
	*/
	// 해당 인덱스가 유효한지
	FInterpLocation GetInterpLocation(int32 Index); 

	// FInterpLocation 배열에서 ItemCount가 가장 작은 Index 값 전달
	int32 GetInterpLocationIndex(); 

	// 해당 인덱스에 Amount만큼 ItemCount 증가/감소
	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	/**
		Sound
	*/
	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

	/**
		Inventory
	*/
	// Inventory Slot의 Hightlight를 끔
	// Item에서 FinishInterp에서 사용하기 위해서
	void UnHighlightInventorySlot(); 

	/**
		Get / Set
	*/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FORCEINLINE bool GetShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool GetShouldPlayEquipSound() const { return bShouldPlayEquipSound; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

protected:
	/* <AActor>*/
	virtual void BeginPlay() override;
	/* </AActor>*/

	// 웅크릴떄 <-> 일어날 때 Capsule 높이 조절
	void InterpCapsuleHalfHeight(float DeltaTime);

	/**
		Weapon
	*/
	// EquippedWeapon를 버리고 TraceHitItem를 대신 장착함.
	void SwapWeapon(AWeapon* WeaponToSwap);

	// 들고 있는 무기의 장전된 총알이 있는지 확인
	bool WeaponHasAmmo();

	// 동일한 총알의 여분을 가지고 있는지
	bool CarryingAmmo();

	// 총알 획득했을 때
	void PickupAmmo(AAmmo* Ammo);

	/**
		Interpolation
	*/
	// Interpolation을 위한 SceneComponent를 InterpLactions 구조체 배열에 넣는 작업
	void InitializeInterpLocations();

	/**
		Foot Step
	*/
	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

private:
	/** Input to Controller */
	void Move(const FInputActionValue& value);
	void Jump();
	void MouseLook(const FInputActionValue& value);
	void GamepadLook(const FInputActionValue& value);

	// 발사(LMB)
	void FireButtonPressed();
	void FireButtonReleased();

	// 조준(RMB)
	void AimingButtonPressed();
	void AimingButtonReleased();
	void Aim();
	void StopAiming();

	// 아이템 획득(E)
	void SelectButtonPressed();
	void SelectButtonReleased();

	// 재장전(R)
	void ReloadButtonPressed();

	// 웅크리기(L-Ctrl)
	void CrouchButtonPressed();

	// Weapon Swap
	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	/**
		Inventory
	*/
	void ExchangeInventoryItems(int32 CurrentItemIdex, int32 NewItemIndex);

	// 다음 Inventory에 들어갈 Slot Index 가져옴
	int32 GetEmptyInventorySlot();

	// Inventory Slot를 Highlight 시킴
	void HighlightInventorySlot();

	/**
		Montage
	*/
	void PlayMontage(UAnimMontage* Montage, const FName& SectionName);

	/**
		Camera
	*/
	// 카메라 부르럽게 움직임
	void CameraInterp(float DeltaTime);
	// 카메라 회전율 변경
	void SetLookRate(); 

	/**
		Weapon
	*/
	// 기본 무기 생성
	AWeapon* SpawnDefaultWeapon();

	// 총알 기본 지급
	void InitializeAmmoMap();

	// 무기 장착
	void EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping = false);

	// 무기 버리기
	void DropWeapon();

	// 총알 재장전
	void ReloadWeapon();

	// 재장전 완료
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	// 무기 교체 완료
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	// ReloadMontage에서 탄창을 빼는 모션에서 호출
	UFUNCTION(BlueprintCallable)
	void GrapClip();

	// ReloadMontage에서 탄창을 다시 집어 넣는 모션에서 호출
	UFUNCTION(BlueprintCallable)
	void ReplaceClip();

	/**
		Particle
	*/
	// Location by Transform
	UParticleSystemComponent* SpawnParticle(UParticleSystem* ParticleName, const FTransform& SpawnTransform);

	// Location by FVector
	UParticleSystemComponent* SpawnParticle(UParticleSystem* ParticleName, const FVector& SpawnVector);

	// 총알 궤도 표시
	void BulletBeam(const FTransform& SocketTransform, const FVector& BeamEndPoint);

	/**
		Sound
	*/
	void PlaySound(USoundBase* SoundName);

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	/**
		Trace
	*/
	// OutBeamLocation : Output
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation, bool& bHitObject);

	// Crosshair 밑에 Item이 있는지 감지
	// OutHitResult, OutHitLocation, bShot : Output
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation, bool& bShot);

	// Tick에서 사용하는 Item Widget를 보여주는 함수
	// Item의 AreaSphere에 Overlap 될 때만 보여줌.
	void TraceForItems();

	/**
		Crosshair
	*/
	// Crosshair 퍼짐 계산
	void CalculateCrosshiarSpread(float DeltaTime);

	/**
		Fire
	*/
	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireWeapon();

	UFUNCTION(BlueprintCallable)
	void FireBullet();

	// 발사 Timer -> AutoFireRest 호출
	void StartFireTimer();

	// AutoFireRest -> StartFireTimer 호출 or 그냥 끝
	UFUNCTION()
	void AutoFireReset();

	// Pistol Slide 움직임
	void PistolSlide();

private:
	/**
		Properties
	*/
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FPS{ 0.f };

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere)
	float GamepadSticklean = 0.1f;

	// 점프 높이
	UPROPERTY(EditAnywhere, Category = "Properties | Move")
	float JumpVelocity = 600.f;

	// 공중에서 제어 속도
	UPROPERTY(EditAnywhere, Category = "Properties | Move")
	float AirControlRate = 0.2;

	// 서 있을떄 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties | Move", meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed{ 600.f };

	// 웅크릴때 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties | Move", meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed{ 350.f };

	// 현재 Capsule 반절 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties | Move", meta = (AllowPrivateAccess = "true"))
	float CurrentCapsuleHalfHeight{ 88.f };

	// 서 있을때 Capsule 반절 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties | Move", meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight{ 88.f };

	// 웅크릴때 Capsuel 반절 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties | Move", meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight{ 49.f };

	// 서 있을 때 마찰
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties | Move", meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction{ 2.0f };

	// 웅크리고 있을 때 마찰
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties | Move", meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction{ 100.f };

	/**
		Input
	*/
	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputMappingContext* IMC;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* GamepadLookAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* FireWeaponAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* AimingAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* SelectAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* FAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* Swap1Action;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* Swap2Action;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* Swap3Action;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* Swap4Action;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* Swap5Action;

	/**
		Crouching
	*/
	// 캐릭터가 웅크리고 있을 떄 True
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Crouching", meta = (AllowPrivateAccess = "true"))
	bool bCrouching{ false };

	/**
		Camera
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// 게임패드 기본 X 감도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadTurnRate{ 45.0f };
	// 게임패드 기본 Y 감도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadLookUpRate{ 45.0f };

	// 마우스 기본 X감도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MouseTurnRate{ 1.0f };
	// 마우스 기본 Y감도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MouseLookUpRate{ 1.0f };

	// 조준 버튼이 눌리고 있는지
	bool bAimingButtonPressed{ false };

	// 줌 인/아웃 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Camera", meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed{ 20.f };

	// 게임패드 줌 아웃 상태 X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadHipTurnRate{ 90.f };
	// 게임패드 줌 아웃 상태 Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadHipLookUpRate{ 90.f };

	// 게임패드 줌 인 상태 X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadAimingTurnRate{ 20.f };
	// 게임패드 줌 인 상태 Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadAimingLookUpRate{ 20.f };

	// 마우스 줌 아웃 상태 X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate{ 1.0f };
	// 마우스 줌 아웃 상태 Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate{ 1.0f };

	// 마우스 줌 인 상태 X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate{ 0.2 };
	// 마우스 줌 인 상태 Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate{ 0.2 };
	
	// 줌을 하고 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Camera", meta = (AllowPrivateAccess = "true"))
	bool bAiming{false};

	// 기본 카메라 FOV 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV{ 0.f };

	// 줌 했을 때(조준 했을 때) 카메라 FOV값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV{ 30.f };

	// 현재 프레임의 FOV값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraCurrentFOV{ 0.f };

	/**
		Weapon
	*/
	// Combat State, Unoccupied 상태일때만 발사 혹은 재장전 가능
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState{ ECombatState::ECS_Unoccupied };

	// 현재 장착한 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;
	
	// map를 사용하여 총알의 종류와 갯수를 맵핑해서 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true")) 
	TMap<EAmmoType, int32> AmmoMap;

	// 시작시 기본 지급 9mm 총알
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo{ 90 };

	// 시작시 기본 지금 5.56mm 총알
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo{ 120 };

	// 기본 무기
	// 스폰하기 위해선 UClass 타입으로 필요함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// TraceForItems를 통해서 감지된 마지막 Item(Null일 수도 있음)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	// 총 사정거리
	UPROPERTY(EditAnywhere, Category = "Combat | Weapon")
	float WeaponRange = 50000.f;

	// 무기 획득 효과를 위한 카메라와의 바깥쪽 보간 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance{ 150.f };	 // X

	// 무기 획득 효과를 위한 카메라와의 윗쪽 보간 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation{ 45.f }; // Y

	// 탄창의 Transform 값, 장전 시작할 때 탄창의 처음 위치(되돌와야 할 위치)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	// 재장전 동안 탄창을 들고 있을 손의 SceneComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	/**
		Interpolation
	*/
	// Weapon Interpolation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

	// Interp Location 구조체 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;

	/**
		AnimMontage
	*/
	// 발사 모션 Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HipFireMontage;

	// 재장전 모션 Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	// 무기 교체 모션 Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	/**
		Sounds 
	*/

	// PickupSound가 동시 다발적으로 재생되지 않도록 하는 TimerHandle
	FTimerHandle PickupSoundTimer;

	// true일 때 PickupSound 재생 가능
	bool bShouldPlayPickupSound{ true };

	// TimerHandle이 ResetPickupSoundTimer() 호출 할 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Sounds", meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime{ 0.2f };

	// EquipSound가 동시 다발적으로 재생되지 않도록 하는 TimerHandle
	FTimerHandle EquipSoundTimer;

	// true일 때 EquipSound 재생 가능
	bool bShouldPlayEquipSound{ true };

	// TimerHandle이 ResetEquipSoundTimer() 호출 할 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Sounds", meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime{ 0.2 };

	/**
		Particles
	*/

	// 총알 날라기는 궤도를 연기처럼 보여주는 Particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Particle", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// 총알 탄착 지점 Particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Particle", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;
	
	/**
		Crosshair
	*/
	// Crosshair 퍼짐 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier{ 0.0f };

	// Crosshair 이동중 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor{ 0.0f };

	// Crosshair 점프 중 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor{ 0.0f };

	// Crosshair 조준 중 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor{ 0.0f };

	// Crosshair 사격중 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairShooingFactor{ 0.0f };

	/**
		Fire
	*/
	// 사격시 Crosshair 퍼지고 줄어드는걸 제어하는 Timer
	FTimerHandle CrosshairShootTimer;

	// 사격후 다시 줄어들 시간
	UPROPERTY(EditAnywhere, Category = "Combat | Crosshair", meta = (AllowPrivateAccess = "true"))
	float ShootTimeDuration{ 0.05f };

	// 자동 발사 제어 타이머
	FTimerHandle AutoFireTimer;

	bool bFiringBullet{ false };

	// 발사 버튼 누르고 있는지
	bool bFireButtonPressed{ false };

	// 발사 할지 말지, true : 발사 가능, false : 발사 불가능
	bool bShouldFire{ true };

	/**
		Trace
	*/
	// 참이면 모든 프레임때 Item를 감지하는 LineTrace를 사용
	bool bShouldTraceForItems{ false };

	// 겹친 Items 수
	int8 OverlappedItemCount;

	// 마지막 Frame에 감지된 Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItemLastFrame;

	/**
		Inventory
	*/
	// Inventory 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	const int32 INVENTORY_CAPACITY{ 6 };

	// 최근 Highlight slot를 한 Slot Index
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot{ -1 };

	/**
		Delegate
	*/
	// MULTI_CAST_DELEGATE
	// BlueprintAssignable : BP에서 할당 가능
	// BlueprintCallable : BP에서 호출 가능

	// Inventory에 저장된 Item을 장착할때 Slot에 있는 Weapon과 EquippedWeapon의 정보를 교환하는 Delegate
	// EquipWeapon()에서 Broadcast되고, InventoryBar에서 이벤트를 바인딩함.
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	// Inventory 빈 Slot에 HighlightAnimation를 재생에 사용할 Delegate
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;
};
