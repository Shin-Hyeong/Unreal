#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Setting.generated.h"

class UTextBlock;
class USlider;
class UButton;
class AShooterCharacter;
class UGameUserSettings;

UCLASS()
class SHOOTER_API USetting : public UUserWidget
{
	GENERATED_BODY()

public:


protected:
	virtual void NativeConstruct() override;

	void SaveData(const float& MouseX, const float& MouseY, const float& MouseXAim, const float MouseYAim,
		const float& GamepadX, const float& GamepadY, const float& GamepadXAim, const float& GamepadYAim, const int32 Idx);

private:
	// 설정 저장
	UFUNCTION()
	void OnSaveButtonClick();

	// 설정 취소
	UFUNCTION()
	void OnCancelButtonClick();

	// 게임 종료
	UFUNCTION()
	void OnQuitGameButtonClick();

	// MouseX 감도 조절
	UFUNCTION()
	void OnValueChangedMouseX(float Value);

	// MouseY 감도 조절
	UFUNCTION()
	void OnValueChangeMouseY(float Value);

	// MouseX Aiming 감도 조절
	UFUNCTION()
	void OnValueChangeMouseXAiming(float Value);

	// MouseY Aiming 감도 조절
	UFUNCTION()
	void OnValueChangeMouseYAiming(float Value);

	// GamepadX 감도 조절
	UFUNCTION()
	void OnValueChangeGamepadX(float Value);

	// GamepadY 감도 조절
	UFUNCTION()
	void OnValueChangeGamepadY(float Value);

	UFUNCTION()
	void OnValueChangeGamepadXAiming(float Value);

	UFUNCTION()
	void OnValueChangeGamepadYAiming(float Value);

	// 해상도 크게
	UFUNCTION()
	void OnClickResolutionBiggerButton();

	// 해상도 작게
	UFUNCTION()
	void OnClickResolutionSmallerButton();

private:
	UPROPERTY()
	AShooterCharacter* Player;

	UPROPERTY()
	APlayerController* PlayerController;

	// 기타 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* SaveButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	// 마우스 감도
	UPROPERTY(meta = (BindWidget))
	USlider* MouseXSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* MouseYSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* MouseXAimingSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* MouseYAimingSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MouseXText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MouseYText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MouseXAimingText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MouseYAimingText;


	// 게임패드 감도
	UPROPERTY(meta = (BindWidget))
	USlider* GamepadXSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* GamepadYSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* GamepadXAimingSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* GamepadYAimingSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GamepadXText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GamepadYText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GamepadXAimingText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GamepadYAimingText;

	// 해상도
	UGameUserSettings* ScreenResolution;

	UPROPERTY(meta = (BindWidget))
	UButton* ResolutionSmallerButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ResolutionBiggerButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Resolution;

	int32 ResolutionIdx{ 1 };
	int32 ResolutionX[3] = { 1280, 1920, 2560 };
	int32 ResolutionY[3] = { 720, 1080, 1440 };
	FString ResolutionString[3] =
	{
		"1280 * 720",
		"1920 * 1080",
		"2560 * 1440"
	};

	// SaveDate
	float SaveMouseXValue{ 1.f };
	float SaveMouseYValue{ 1.f };
	float SaveMouseXAimingValue{ 0.2f };
	float SaveMouseYAimingValue{ 0.2f };
	float SaveGamepadXValue{ 90.f };
	float SaveGamepadYValue{ 90.f };
	float SaveGamepadXAimingValue{ 20.f };
	float SaveGamepadYAimingValue{ 20.f };
	int32 SaveResolutionIdx{ 1 };

public:
};

