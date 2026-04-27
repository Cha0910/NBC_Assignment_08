// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameMode.h"
#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"

ASpartaGameMode::ASpartaGameMode()
{
	// set default pawn class to our character
	DefaultPawnClass = ASpartaCharacter::StaticClass();

	PlayerControllerClass = ASpartaPlayerController::StaticClass();

	GameStateClass = ASpartaGameState::StaticClass();
}

