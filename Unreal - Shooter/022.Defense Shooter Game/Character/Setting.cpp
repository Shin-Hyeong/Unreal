#include "Character/Setting.h"

// Player
#include "Character/ShooterCharacter.h"

// Button
#include "Components/Button.h"

// TextBlock
#include "Components/TextBlock.h"

// Slider
#include "Components/Slider.h"

// KismetSystemLibrary
#include "Kismet/KismetSystemLibrary.h"

// GameUserSetting
#include "GameFramework/GameUserSettings.h"

void USetting::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = World->GetFirstPlayerController();
		Player = Cast<AShooterCharacter>(PlayerController->GetPawn());
	}

	ScreenResolution = UGameUserSettings::GetGameUserSettings();

	// Delegate
	SaveButton->OnClicked.AddDynamic(this, &USetting::OnSaveButtonClick);
	CancelButton->OnClicked.AddDynamic(this, &USetting::OnCancelButtonClick);
	QuitButton->OnClicked.AddDynamic(this, &USetting::OnQuitGameButtonClick);

	MouseXSlider->OnValueChanged.AddDynamic(this, &USetting::OnValueChangedMouseX);
	MouseYSlider->OnValueChanged.AddDynamic(this, &USetting::OnValueChangeMouseY);
	MouseXAimingSlider->OnValueChanged.AddDynamic(this, &USetting::OnValueChangeMouseXAiming);
	MouseYAimingSlider->OnValueChanged.AddDynamic(this, &USetting::OnValueChangeMouseYAiming);
	GamepadXSlider->OnValueChanged.AddDynamic(this, &USetting::OnValueChangeGamepadX);
	GamepadYSlider->OnValueChanged.AddDynamic(this, &USetting::OnValueChangeGamepadY);
	GamepadXAimingSlider->OnValueChanged.AddDynamic(this, &USetting::OnValueChangeGamepadXAiming);
	GamepadYAimingSlider->OnValueChanged.AddDynamic(this, &USetting::OnValueChangeGamepadYAiming);


	ResolutionSmallerButton->OnClicked.AddDynamic(this, &USetting::OnClickResolutionSmallerButton);
	ResolutionBiggerButton->OnClicked.AddDynamic(this, &USetting::OnClickResolutionBiggerButton);
}

void USetting::SaveData(const float& MouseX, const float& MouseY, const float& MouseXAim, const float MouseYAim, const float& GamepadX, const float& GamepadY, const float& GamepadXAim, const float& GamepadYAim, const int32 Idx)
{
	SaveMouseXValue = MouseX;
	SaveMouseYValue = MouseY;
	SaveMouseXAimingValue = MouseXAim;
	SaveMouseYAimingValue = MouseYAim;

	SaveGamepadXValue = GamepadX;
	SaveGamepadYValue = GamepadY;
	SaveGamepadXAimingValue = GamepadXAim;
	SaveGamepadYAimingValue = GamepadYAim;

	SaveResolutionIdx = Idx;
}

// 설정 저장 및 끔
void USetting::OnSaveButtonClick()
{
	Player->GetSettingUIDelegate().Broadcast(false);

	PlayerController->SetPause(false);
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;

	// Slider에서 값 추출
	float MouseXValue = MouseXSlider->GetValue();
	float MouseYValue = MouseYSlider->GetValue();
	float MouseXAimValue = MouseXAimingSlider->GetValue();
	float MouseYAimValue = MouseYAimingSlider->GetValue();

	float GamepadXValue = GamepadXSlider->GetValue();
	float GamepadYValue = GamepadYSlider->GetValue();
	float GamepadXAimValue = GamepadXAimingSlider->GetValue();
	float GamepadYAimValue = GamepadYAimingSlider->GetValue();

	// Setting에 값 저장.
	SaveData(MouseXValue, MouseYValue, MouseXAimValue, MouseYAimValue, GamepadXValue, GamepadYValue, GamepadXAimValue, GamepadYAimValue, ResolutionIdx);

	// Player에게 값 전달
	Player->Setting(MouseXValue, MouseYValue, MouseXAimValue, MouseYAimValue, GamepadXValue, GamepadYValue, GamepadXAimValue, GamepadYAimValue);

	// 해상도 조절
	FIntPoint Screen(ResolutionX[ResolutionIdx], ResolutionY[ResolutionIdx]);
	ScreenResolution->SetScreenResolution(Screen);
	ScreenResolution->ApplySettings(true);
}

// 설정 창 초기화 및 끔
void USetting::OnCancelButtonClick()
{
	Player->GetSettingUIDelegate().Broadcast(false);

	PlayerController->SetPause(false);
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;

	FString ValueString{};

	// Mouse X
	MouseXSlider->SetValue(SaveMouseXValue);
	ValueString = FString::Printf(TEXT("%.2f"), SaveMouseXValue);
	MouseXText->SetText(FText::FromString(ValueString));

	// Mouse Y
	MouseYSlider->SetValue(SaveMouseYValue);
	ValueString = FString::Printf(TEXT("%.2f"), SaveMouseYValue);
	MouseYText->SetText(FText::FromString(ValueString));

	// Mouse Aim X
	MouseXAimingSlider->SetValue(SaveMouseXAimingValue);
	ValueString = FString::Printf(TEXT("%.2f"), SaveMouseXAimingValue);
	MouseXAimingText->SetText(FText::FromString(ValueString));

	// Mouse Aim Y
	MouseYAimingSlider->SetValue(SaveMouseYAimingValue);
	ValueString = FString::Printf(TEXT("%.2f"), SaveMouseYAimingValue);
	MouseYAimingText->SetText(FText::FromString(ValueString));

	// Gamepad X
	GamepadXSlider->SetValue(SaveGamepadXValue);
	ValueString = FString::Printf(TEXT("%.0f"), SaveGamepadXValue);
	GamepadXText->SetText(FText::FromString(ValueString));

	// Gamepad Y
	GamepadYSlider->SetValue(SaveGamepadYValue);
	ValueString = FString::Printf(TEXT("%.0f"), SaveGamepadYValue);
	GamepadYText->SetText(FText::FromString(ValueString));

	// Gamepad Aim X
	GamepadXAimingSlider->SetValue(SaveGamepadXAimingValue);
	ValueString = FString::Printf(TEXT("%.0f"), SaveGamepadXAimingValue);
	GamepadXAimingText->SetText(FText::FromString(ValueString));

	// Gamepad Aim Y
	GamepadYAimingSlider->SetValue(SaveGamepadYAimingValue);
	ValueString = FString::Printf(TEXT("%.0f"), SaveGamepadYAimingValue);
	GamepadYAimingText->SetText(FText::FromString(ValueString));

	// Resolution
	Resolution->SetText(FText::FromString(ResolutionString[SaveResolutionIdx]));
}

// 게임 종료
void USetting::OnQuitGameButtonClick()
{
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, true);
}

// MouseX값 변경
void USetting::OnValueChangedMouseX(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);
	MouseXText->SetText(FText::FromString(ValueString));
}

// Mouse Y값 변경
void USetting::OnValueChangeMouseY(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);
	MouseYText->SetText(FText::FromString(ValueString));
}

// Mouse Aim X 값 변경
void USetting::OnValueChangeMouseXAiming(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);
	MouseXAimingText->SetText(FText::FromString(ValueString));
}

// Mouse Aim Y 값 변경
void USetting::OnValueChangeMouseYAiming(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);
	MouseYAimingText->SetText(FText::FromString(ValueString));
}

// Gamepad X값 변경
void USetting::OnValueChangeGamepadX(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.0f"), Value);
	GamepadXText->SetText(FText::FromString(ValueString));
}

// Gamepad Y값 변경
void USetting::OnValueChangeGamepadY(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.0f"), Value);
	GamepadYText->SetText(FText::FromString(ValueString));
}

// Gamepad Aim X 값 변경
void USetting::OnValueChangeGamepadXAiming(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);
	GamepadXAimingText->SetText(FText::FromString(ValueString));
}

// Gamepad Aim Y 값 변경
void USetting::OnValueChangeGamepadYAiming(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);
	GamepadYAimingText->SetText(FText::FromString(ValueString));
}

// 해상도 크게 변경
void USetting::OnClickResolutionBiggerButton()
{
	ResolutionIdx = FMath::Clamp(ResolutionIdx + 1, 0, 2);
	Resolution->SetText(FText::FromStringView(ResolutionString[ResolutionIdx]));
}

// 해상도 작게 변경
void USetting::OnClickResolutionSmallerButton()
{
	ResolutionIdx = FMath::Clamp(ResolutionIdx - 1, 0, 2);
	Resolution->SetText(FText::FromStringView(ResolutionString[ResolutionIdx]));
}