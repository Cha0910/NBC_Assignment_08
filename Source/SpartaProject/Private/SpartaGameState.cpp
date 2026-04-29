// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameState.h"
#include <Kismet/GameplayStatics.h>
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "PlatformSpawner.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	ItemToSpawnPerLevel = { 20, 25, 30 };
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	LevelGoalScores = { 1500, 2500, 4000 };
	WaveDurations = { 60.0f, 45.0f, 30.0f };	
	CurrentWaveIndex = 0;
	WaveSpawnCounts = { 3, 2, 1 };
	MaxWaves = WaveDurations.Num();
	PlatformSpawnWave = 1;
	PoisonPlatformSpawnWave = 2;
	PlatformSpawner = nullptr;
	PoisonPlatformSpawner = nullptr;
	
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();
	StartLevel();

	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
	GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this, &ASpartaGameState::UpdateHUD, 0.1f, true);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Points)
{
	if (UGameInstance* GameInstance = GetGameInstance()) 
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if(SpartaGameInstance) 
		{
			SpartaGameInstance->AddToScore(Points);
		}
	}
}

void ASpartaGameState::StartLevel()
{
	CurrentWaveIndex = 0;

	if(APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHud();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlatformSpawner::StaticClass(), FoundSpawners);

	for (AActor* Actor : FoundSpawners)
	{
		APlatformSpawner* Spawner = Cast<APlatformSpawner>(Actor);

		if (Spawner->ActorHasTag(FName("Platform")))
		{
			PlatformSpawner = Spawner;
			PlatformSpawner->DisableSpawning();
		}

		else if (Spawner->ActorHasTag(FName("PoisonPlatform")))
		{
			PoisonPlatformSpawner = Spawner;
			PoisonPlatformSpawner->DisableSpawning();
		}
	}


	StartNextWave();
}

void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}


void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Log, TEXT("Coin collected %d / %d"), CollectedCoinCount, SpawnedCoinCount);

	if(SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		UE_LOG(LogTemp, Log, TEXT("All coins collected! Ending wave early."));
		
		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
		int BonusPoints = FMath::RoundToInt(RemainingTime * 15);
		AddScore(BonusPoints);

		UE_LOG(LogTemp, Log, TEXT("Bonus: Ending wave early! Get %d points"), BonusPoints);

		EndWave();
	}
}

void ASpartaGameState::EndLevel()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);

			if(SpartaGameInstance->TotalScore < (LevelGoalScores.IsValidIndex(CurrentLevelIndex) ? LevelGoalScores[CurrentLevelIndex] : 0))
			{
				OnGameOver();
				return;
			}

			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
			
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
	}

	if(LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if(APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if(ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			
			if(UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if(UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if(UGameInstance* GameInstance = GetGameInstance())
					{
						if(USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if(UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if(UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d-%d"), CurrentLevelIndex+1, CurrentWaveIndex+1)));
				}

				if (UTextBlock* CoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Coin"))))
				{
					CoinText->SetText(FText::FromString(FString::Printf(TEXT("Coins: %d/%d"), CollectedCoinCount, SpawnedCoinCount)));
				}

				if (UTextBlock* GoalText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Goal"))))
				{
					int32 GoalScore = LevelGoalScores.IsValidIndex(CurrentLevelIndex) ? LevelGoalScores[CurrentLevelIndex] : 0;
					GoalText->SetText(FText::FromString(FString::Printf(TEXT("Goal: %d"), GoalScore)));
				}
			}
		}
	}
}

void ASpartaGameState::StartNextWave()
{
	float WaveDuration = WaveDurations.IsValidIndex(CurrentWaveIndex) ? WaveDurations[CurrentWaveIndex] : 30.0f;

	if(CurrentWaveIndex >= PlatformSpawnWave)
	{
		if(PlatformSpawner)
		{
			PlatformSpawner->EnableSpawning();
		}
	}

	if(CurrentWaveIndex >= PoisonPlatformSpawnWave)
	{
		if(PoisonPlatformSpawner)
		{
			PoisonPlatformSpawner->EnableSpawning();
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	ResetItemsForCurrentWave();

	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	float SpawnInterval = WaveDuration / (WaveSpawnCounts.IsValidIndex(CurrentWaveIndex) ? WaveSpawnCounts[CurrentWaveIndex] : 3);

	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ASpartaGameState::OnWaveTimeUp, WaveDuration, false);

	SpawnItemsForWave();
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpartaGameState::SpawnItemsForWave, SpawnInterval, true);
}

void ASpartaGameState::SpawnItemsForWave()
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	int32 ItemToSpawn = ItemToSpawnPerLevel.IsValidIndex(CurrentLevelIndex) ? ItemToSpawnPerLevel[CurrentLevelIndex] : 20;

	for (int32 i = 0; i < ItemToSpawn; ++i)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);

			if (SpawnVolume)
			{
				AActor* SpawnedItem = SpawnVolume->SpawnRandomItem();
				SpawnedItems.Add(SpawnedItem);

				if (SpawnedItem && SpawnedItem->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Level %d Wave %d started! Spawned %d coins."), CurrentLevelIndex + 1, CurrentWaveIndex + 1, SpawnedCoinCount);
}

void ASpartaGameState::EndWave()
{
	UE_LOG(LogTemp, Log, TEXT("Level %d Wave %d ended!"), CurrentLevelIndex + 1, CurrentWaveIndex + 1);
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	CurrentWaveIndex++;

	if(CurrentWaveIndex >= MaxWaves)
	{
		EndLevel();
	}
	else
	{
		StartNextWave();
	}
}

void ASpartaGameState::ResetItemsForCurrentWave()
{
	for (AActor* Item : SpawnedItems)
	{
		if (IsValid(Item))
		{
			Item->Destroy();
		}
	}
	SpawnedItems.Empty();
}
