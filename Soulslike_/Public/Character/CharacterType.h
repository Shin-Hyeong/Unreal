#pragma once

UENUM(BlueprintType)
// CharacterState
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_LockOnUnequipped UMETA(DisplayName = "Lock On Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Hand Weapon"),
	ECS_LockOnEquippedOneHandedWeapon UMETA(DisplayName = "Lock On Equipped One-Hand Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Hand Weapon"),
	ECS_LockOnEquippedTwoHandedWeapon UMETA(DisplayName = "Lock On Equipped Two-Hand Weapon")
};

UENUM(BlueprintType)
// PlayableState
enum class EPlayerState : uint8
{
	// 리스폰 되었을 때
	EPS_WakeUp UMETA(DisplayName = "WakeUp"),
	// 아무 것도 안하고 있는 상태
	EPS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	// 공격을 받은 상태
	EPS_HitReact UMETA(DisplayName = "HitReact"),
	// 무기를 장착/ 집어 넣고 있는 상태
	EPS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	// 공격하고 있는 상태
	EPS_Attacking UMETA(DisplayName = "Attacking"),
	// 강 공격하고 있는 상태
	EPS_PWAttacking UMETA(DisplayName = "Power Attacking"),
	// 회피하는 상태
	EPS_Dodge UMETA(DisplayName = "Dodge"),
	// 사망한 상태
	EPS_Death UMETA(DisplayName = "Death")
};

UENUM(BlueprintType)
// EquipState
enum class EEquippState : uint8
{
	EES_Equipping UMETA(DisplayName = "Equipping"),
	EES_Equipped UMETA(DisplayName = "Equipped")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_DeathPoseFront UMETA(DisplayName = "DeathPoseFront"),
	EDP_DeathPoseBack UMETA(DisplayName = "DeathPoseBack"),
	EDP_DeathPoseRight UMETA(DisplayName = "DeathPoseRight"),
	EDP_DeathPoseLeft UMETA(DisplayName = "DeathPoseLeft"),

	EDP_DeathPoseMax UMETA(DisplayName = "DeathPoseMax")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),

	// 사망 상태
	EES_Dead UMETA(DisplayName = "Dead"),
	// 순찰 상태
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	// 추격 상태
	EES_Chasing UMETA(DisplayName = "Chasing"),
	// 공격 상태
	EES_Attacking UMETA(DisplayName = "Attacking"),
	// 교전 중
	EES_Engaged UMETA(DisplayName = "Engaged")
};