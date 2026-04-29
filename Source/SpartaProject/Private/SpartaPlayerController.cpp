#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "SpartaGameState.h"
#include "Components/TextBlock.h"
#include "SpartaGameInstance.h"
#include "Kismet/GameplayStatics.h"

ASpartaPlayerController::ASpartaPlayerController() 
	: 
	InputMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
    Super::BeginPlay();

	if(ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if(InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(EMenuState::Start);
	}
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowMainMenu(EMenuState MenuState)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if(MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		SetMainMenuText(MenuState);

		if (MenuState == EMenuState::NextLevel || MenuState == EMenuState::Restart)
		{
			UFunction* PlayAnimationFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimationFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimationFunc, nullptr);
			}
		}
	}
}

void ASpartaPlayerController::SetMainMenuText(EMenuState MenuState)
{
	if (MainMenuWidgetClass && MainMenuWidgetInstance)
	{
		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			FText TitleText;

			if (MenuState == EMenuState::Restart)
			{
				TitleText = FText::FromString(TEXT("Restart"));

			}
			else if (MenuState == EMenuState::NextLevel)
			{
				TitleText = FText::FromString(TEXT("Next Level"));
			}
			else
			{
				TitleText = FText::FromString(TEXT("Start"));
			}

			ButtonText->SetText(TitleText);
		}

		if (MenuState == EMenuState::NextLevel || MenuState == EMenuState::Restart)
		{
			if (UTextBlock* ResultText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("GameOverText"))))
			{
				FText ResultTextValue = MenuState == EMenuState::Restart ? FText::FromString("Game Over") : FText::FromString("Level Complete");
				ResultText->SetText(ResultTextValue);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("TotalScoreText"))))
			{
				if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)));
				}
			}
		}
	}
}

void ASpartaPlayerController::ShowGameHud()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if(HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if(HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if(SpartaGameState)
		{
			SpartaGameState->UpdateHUD();
		}
	}
}

void ASpartaPlayerController::StartGame()
{
	if(USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if(SpartaGameInstance->bIsGameOver)
		{
			SpartaGameInstance->bIsGameOver = false;
			SpartaGameInstance->CurrentLevelIndex = 0;
			SpartaGameInstance->TotalScore = 0;

			UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
		}
		
		else
		{
			if (ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr)
			{
				SpartaGameState->LoadNextLevel();
			}
		}
	}

	SetPause(false);
}

void ASpartaPlayerController::ExitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, true);
}