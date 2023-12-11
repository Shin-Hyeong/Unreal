#pragma once

// Player의 상태를 나타내는 enum class
// uint 8 : 해당 enum class의 크기를 8bit로 조정하여 최적화함. (기본 4byte)
// BlueprintType : BP에서도 Enum를 사용할 수 있도록함.
// UMETA(DisplayName = "") : BP에서 보이는 Enum 리스트 이름.
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	// ECS : Enum Class State
	// 빈 손
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	// 한손 무기
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Hand Weapon"),
	// 두손 무기
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Hand Weapon")
};

// Player가 어떤 행동을 하고 있는지 나타내는 enum class
UENUM(BlueprintType)
enum class EActionState : uint8
{	
	// EAS : Enum Action State
	// 다른 행동(상호작용) 진행중
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	// 공격 진행중
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	// 무기 장착중
	EAS_EquippingWeapon UMETA(DisplayName = "EquippingWeapon"),
};

UENUM(BlueprintType)
enum class EEquipState : uint8
{
	// 무장해재 / 무장 중
	EES_Equipping UMETA(DisplayName = "Equipping"),
	// 무장해재 / 무장 완료
	EES_Equipped UMETA(DisplayName = "Equipped")
};