#include "HUD/SettingUI.h"

#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ScrollBox.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "GenericPlatform/GenericWindow.h"

#include "Player/PlayableCharacter.h"

void USettingUI::SetSaveDate(float ValueX, float ValueY, bool RunToggle, float CameraArm, float FPS)
{
	SaveMouseXValue = ValueX;
	SaveMouseYValue = ValueY;
	SaveRunCheckBox = RunToggle;
	SaveCameraArm = CameraArm;
	SaveFPS = FPS;

	FPSSlider->SetValue(FPS);
	FString ValueFPS = FString::Printf(TEXT("%.0f"), SaveFPS);
	FPSText->SetText(FText::FromString(ValueFPS));
}

void USettingUI::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = World->GetFirstPlayerController();
		Player = Cast<APlayableCharacter>(PlayerController->GetPawn());
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	}

	ScreenMode = UGameUserSettings::GetGameUserSettings();

	// 초기값
	RunCheckBox->SetIsChecked(false);
	WindowScroll->SetVisibility(ESlateVisibility::Collapsed);
	

	/** Delegate */
	SaveButton->OnClicked.AddDynamic(this, &USettingUI::OnSaveButtonClick);
	CancelButton->OnClicked.AddDynamic(this, &USettingUI::OnCancelButtonClick);

	QuitGameButton->OnClicked.AddDynamic(this, &USettingUI::OnQuitGameButtonClick);
	RestartButton->OnClicked.AddDynamic(this, &USettingUI::OnRestartButtonClick);

	MouseXSlider->OnValueChanged.AddDynamic(this, &USettingUI::OnValueChangedSliderX);
	MouseYSlider->OnValueChanged.AddDynamic(this, &USettingUI::OnValueChangedSliderY);
	CameraArmSlider->OnValueChanged.AddDynamic(this, &USettingUI::OnValueChangedSliderArm);
	FPSSlider->OnValueChanged.AddDynamic(this, &USettingUI::OnValueChangedSliderFPS);

	BTN_Bigger->OnClicked.AddDynamic(this, &USettingUI::OnClickResolutionBigger);
	BTN_Smaller->OnClicked.AddDynamic(this, &USettingUI::OnClickResolutionSmaller);

	BTN_WindowState->OnClicked.AddDynamic(this, &USettingUI::OnClickButtonWindowState);
	BTN_FullScreen->OnClicked.AddDynamic(this, &USettingUI::OnClickButtonFullScreenMode);
	BTN_Window->OnClicked.AddDynamic(this, &USettingUI::OnClickButtonWindowMode);
}

void USettingUI::OnSaveButtonClick()
{
	// UI Visibility
	UE_LOG(LogTemp, Warning, TEXT("Click SaveButton!"));
	ESlateVisibility OffSlateVisibility = ESlateVisibility::Collapsed;
	this->SetVisibility(OffSlateVisibility);

	// Set Setting Value
	float ValueX = MouseXSlider->GetValue();
	float ValueY = MouseYSlider->GetValue();

	float ValueArm = CameraArmSlider->GetValue();

	bool bRunToggle = RunCheckBox->IsChecked();
	// 값 전달
	Player->SetSettingValue(ValueX, ValueY, bRunToggle);
	Player->SetCameraArmLength(ValueArm);
	Player->Walk();

	// Set Max FPS
	// GEngine->UEngine::SetMaxFPS(FPSSlider->GetValue());
	ScreenMode->SetFrameRateLimit(FPSSlider->GetValue());

	// Resolution
	FIntPoint ScreenResolution(ResolutionX[ResolutionIdx], ResolutionY[ResolutionIdx]);
	ScreenMode->SetScreenResolution(ScreenResolution);

	ScreenModeString = (T_WindowState->GetText()).ToString();
	ScreenMode->ApplySettings(true);

	// Mouse Cursor
	PlayerController->SetPause(false);
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;
	PlayerController->bEnableMouseOverEvents = false;
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}

void USettingUI::OnCancelButtonClick()
{
	// UI Visibility
	UE_LOG(LogTemp, Warning, TEXT("Off Setting UI"));
	ESlateVisibility OffSlateVisibility = ESlateVisibility::Collapsed;
	this->SetVisibility(OffSlateVisibility);

	// Return Setting Value
	MouseXSlider->SetValue(SaveMouseXValue);
	MouseYSlider->SetValue(SaveMouseYValue);
	CameraArmSlider->SetValue(SaveCameraArm);
	FPSSlider->SetValue(SaveFPS);

	FString ValueXString = FString::Printf(TEXT("%.2f"), SaveMouseXValue);
	MouseXText->SetText(FText::FromString(ValueXString));

	FString ValueYString = FString::Printf(TEXT("%.2f"), SaveMouseYValue);
	MouseXText->SetText(FText::FromString(ValueYString));

	FString ValueArm = FString::Printf(TEXT("%.1f"), SaveCameraArm);
	CameraArmText->SetText(FText::FromString(ValueArm));

	FString ValueFPS = FString::Printf(TEXT("%.0f"), SaveFPS);
	FPSText->SetText(FText::FromString(ValueFPS));

	RunCheckBox->SetIsChecked(SaveRunCheckBox);

	ResolutionIdx = SaveResolutionIdx;
	ResolutionText->SetText(FText::FromString(Resolution[SaveResolutionIdx]));

	T_WindowState->SetText(FText::FromString(ScreenModeString));

	// Mouse Cursor
	PlayerController->SetPause(false);
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;
	PlayerController->bEnableMouseOverEvents = false;
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}

void USettingUI::OnQuitGameButtonClick()
{
	// Mouse Cursor
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;
	PlayerController->bEnableMouseOverEvents = false;

	// Game End
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, true);
}

void USettingUI::OnRestartButtonClick()
{
	UGameplayStatics::OpenLevel(this, OpenLevel, true);
}

void USettingUI::OnValueChangedSliderX(float Value)
{
	// Set Text Slider Value
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);
	MouseXText->SetText(FText::FromString(ValueString));
}

void USettingUI::OnValueChangedSliderY(float Value)
{
	// Set Text Slider Value
	FString ValueString = FString::Printf(TEXT("%.2f"), Value);
	MouseYText->SetText(FText::FromString(ValueString));
}

void USettingUI::OnValueChangedSliderArm(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.1f"), Value);
	CameraArmText->SetText(FText::FromString(ValueString));
}

void USettingUI::OnValueChangedSliderFPS(float Value)
{
	FString ValueString = FString::Printf(TEXT("%.0f"), Value);
	FPSText->SetText(FText::FromString(ValueString));
}

void USettingUI::OnClickResolutionBigger()
{
	ResolutionIdx = FMath::Clamp(ResolutionIdx + 1, 0, 2);
	ResolutionText->SetText(FText::FromString(Resolution[ResolutionIdx]));
}

void USettingUI::OnClickResolutionSmaller()
{
	ResolutionIdx = FMath::Clamp(ResolutionIdx - 1, 0, 2); 
	ResolutionText->SetText(FText::FromString(Resolution[ResolutionIdx]));
}

void USettingUI::OnClickButtonWindowState()
{
	if (WindowScroll->GetVisibility() == ESlateVisibility::Collapsed)
	{
		WindowScroll->SetVisibility(ESlateVisibility::Visible);
	}
	else if (WindowScroll->GetVisibility() == ESlateVisibility::Visible)
	{
		WindowScroll->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USettingUI::OnClickButtonWindowMode()
{
	ScreenMode = UGameUserSettings::GetGameUserSettings();
	ScreenMode->UGameUserSettings::SetFullscreenMode(EWindowMode::Windowed);
	T_WindowState->SetText(FText::FromString("Window Mode"));
	WindowScroll->SetVisibility(ESlateVisibility::Collapsed);
}

void USettingUI::OnClickButtonFullScreenMode()
{
	ScreenMode = UGameUserSettings::GetGameUserSettings(); 
	ScreenMode->UGameUserSettings::SetFullscreenMode(EWindowMode::Fullscreen); 
	T_WindowState->SetText(FText::FromString("FullScreen"));
	WindowScroll->SetVisibility(ESlateVisibility::Collapsed);
}