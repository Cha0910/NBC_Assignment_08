// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MovingPlatform.h"

// Sets default values
APlatformSpawner::APlatformSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bIsSpawning = false;
	SpawnInterval = 5.0f;
	SpawnedPlatformCount = 0;
	MaxPlatforms = 10;
	TargetCountRange = FVector2D(1.0f, 3.0f);
	MovementSpeedRange = FVector2D(50.0f, 150.0f);
	RotationSpeedRange = FVector2D(0.0f, 90.0f);
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Area"));
	RootComponent = SpawnArea;
}

// Called when the game starts or when spawned
void APlatformSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnPlatform();
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APlatformSpawner::SpawnPlatform, SpawnInterval, true);
}

FVector APlatformSpawner::GetRandomLocation() const
{
	FVector Center = SpawnArea->GetComponentLocation();
	FVector Extent = SpawnArea->GetScaledBoxExtent();

	return UKismetMathLibrary::RandomPointInBoundingBox(Center, Extent);
}

void APlatformSpawner::SpawnPlatform()
{
	if (!bIsSpawning || SpawnedPlatformCount >= MaxPlatforms || !Platform)
	{
		return;
	}

	FVector SpawnLocation = GetRandomLocation();

	AMovingPlatform* SpawnedPlatform = GetWorld()->SpawnActor<AMovingPlatform>(Platform, SpawnLocation, FRotator::ZeroRotator);

	if (SpawnedPlatform)
	{
		SpawnedPlatformCount++;

		int32 RandomTargetCount = FMath::RandRange(
			FMath::FloorToInt(TargetCountRange.X),
			FMath::FloorToInt(TargetCountRange.Y)
		);

		SpawnedPlatform->TargetLocations.Empty();
		SpawnedPlatform->TargetLocations.Add(SpawnLocation);

		for (int32 i = 0; i < RandomTargetCount; i++)
		{
			FVector RandomTargetLocation = GetRandomLocation();
			SpawnedPlatform->TargetLocations.Add(RandomTargetLocation);
		}

		if (SpawnedPlatform->TargetLocations.Num() > 1)
		{
			SpawnedPlatform->CurrentTargetIndex = 1;
			SpawnedPlatform->CurrentTargetLocation = SpawnedPlatform->TargetLocations[1];
		}

		SpawnedPlatform->MovementSpeed = FMath::RandRange(MovementSpeedRange.X, MovementSpeedRange.Y);
		SpawnedPlatform->RotationSpeed = FMath::RandRange(RotationSpeedRange.X, RotationSpeedRange.Y);
		SpawnedPlatform->bIsMoving = true;
		SpawnedPlatform->bIsRotating = FMath::RandBool();

		SpawnedPlatform->EndAction = EPlatformEndAction::Destroy;
		SpawnedPlatform->OnDestroyed.AddDynamic(this, &APlatformSpawner::OnPlatformDestroyed);
	}
}

void APlatformSpawner::OnPlatformDestroyed(AActor* DestroyedActor)
{
	SpawnedPlatformCount--;
}

void APlatformSpawner::EnableSpawning()
{
	bIsSpawning = true;
}

void APlatformSpawner::DisableSpawning()
{
	bIsSpawning = false;
}

