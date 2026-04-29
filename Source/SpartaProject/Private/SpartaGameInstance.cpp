// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameInstance.h"

USpartaGameInstance::USpartaGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
	bIsGameOver = false;
}

void USpartaGameInstance::AddToScore(int32 ScoreToAdd)
{
	TotalScore += ScoreToAdd;
	UE_LOG(LogTemp, Log, TEXT("Total Score: %d"), TotalScore);
}