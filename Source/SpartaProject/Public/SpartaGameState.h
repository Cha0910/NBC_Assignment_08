// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

class APlatformSpawner;
/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<int32> ItemToSpawnPerLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<int32> LevelGoalScores;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<float> WaveDurations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<int32> WaveSpawnCounts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentWaveIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxWaves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	int32 PlatformSpawnWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	int32 PoisonPlatformSpawnWave;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	FString AnnouncementMessage;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;

	UPROPERTY()
	TArray<AActor*> SpawnedItems;

	UPROPERTY()
	APlatformSpawner* PlatformSpawner;

	UPROPERTY()
	APlatformSpawner* PoisonPlatformSpawner;

	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	FTimerHandle SpawnTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Points);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnLevelComplete();

	void StartLevel();

	void LoadNextLevel();

	void OnWaveTimeUp();

	void OnCoinCollected();

	void EndLevel();

	void UpdateHUD();

	void StartNextWave();

	void SpawnItemsForWave();

	void EndWave();

	void ResetItemsForCurrentWave();
};
