// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformSpawner.generated.h"

class UBoxComponent;
class AMovingPlatform;

UCLASS()
class SPARTAPROJECT_API APlatformSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlatformSpawner();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Spawner|Components")
	UBoxComponent* SpawnArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Spawner|Properties")
	TSubclassOf<class AMovingPlatform> Platform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Spawner|Properties")
	bool bIsSpawning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Spawner|Properties")
	float SpawnInterval;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform Spawner|Properties")
	int32 SpawnedPlatformCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Spawner|Properties")
	int32 MaxPlatforms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Spawner|Properties")
	FVector2D TargetCountRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Spawner|Properties")
	FVector2D MovementSpeedRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Spawner|Properties")
	FVector2D RotationSpeedRange;

	FTimerHandle TimerHandle;

	FVector GetRandomLocation() const;

	void SpawnPlatform();

public:
	void EnableSpawning();

	void DisableSpawning();

	UFUNCTION()
	void OnPlatformDestroyed(AActor* DestroyedActor);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
