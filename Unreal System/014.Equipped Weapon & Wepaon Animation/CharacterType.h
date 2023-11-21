#pragma once

// Player의 상태를 나타내는 enum class
// uint 8 : 해당 enum class의 크기를 8bit로 조정하여 최적화함. (기본 4byte)
// BlueprintType : BP에서도 Enum를 사용할 수 있도록함.
// UMETA(DisplayName = "") : BP에서 보이는 Enum 리스트 이름.
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	// ECS : Enum Class State
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Hand Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Hand Weapon")
};