// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(Root);

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (TargetLocations.Num() < 1)
	{
		StartLocation = GetActorLocation();
		TargetLocations.Add(StartLocation);
	}

	for (AActor* Point : TargetPoints)
	{
		if (Point)
		{
			TargetLocations.Add(Point->GetActorLocation());
		}
	}

	if (TargetLocations.Num() > 1)
	{
		CurrentTargetIndex = 1;
		CurrentTargetLocation = TargetLocations[CurrentTargetIndex];
	}
	else
	{
		bIsMoving = false;
	}
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();

	if (bIsMoving)
	{
		FVector Direction = (CurrentTargetLocation - CurrentLocation).GetSafeNormal();
		FVector NewLocation = CurrentLocation + Direction * MovementSpeed * DeltaTime;
		SetActorLocation(NewLocation);

		if (FVector::Dist(NewLocation, CurrentTargetLocation) <= DistanceThreshold)
		{
			SetActorLocation(CurrentTargetLocation);
			bIsMoving = false;

			if (TargetLocations.Num() > 1)
			{
				GetWorldTimerManager().SetTimer(TimerHandle, this, &AMovingPlatform::SetupNextTargetLocation, MovingDelay, false);
			}
		}
	}

	if (bIsRotating)
	{
		FRotator CurrentRotation = GetActorRotation();
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	}
}

void AMovingPlatform::SetupNextTargetLocation()
{
	if (TargetLocations.Num() <= 1)
	{
		return;
	}

	if (EndAction == EPlatformEndAction::Return)
	{
		switch (ReturnMode)
		{
		case EPlatformReturnMode::Loop:
			CurrentTargetIndex = (CurrentTargetIndex + IndexDirection) % TargetLocations.Num();
			break;

		case EPlatformReturnMode::PingPong:
			CurrentTargetIndex += IndexDirection;

			if (CurrentTargetIndex == TargetLocations.Num() - 1 || CurrentTargetIndex == 0)
			{
				IndexDirection *= -1;
			}
			break;
		}
	}

	else
	{
		CurrentTargetIndex += IndexDirection;

		if (CurrentTargetIndex == TargetLocations.Num() || CurrentTargetIndex < 0)
		{
			switch (EndAction)
			{
			case EPlatformEndAction::None:
				bIsMoving = false;
				break;

			case EPlatformEndAction::Reset:
				CurrentTargetIndex = 1;
				SetActorLocation(StartLocation);
				SetActorRotation(FRotator::ZeroRotator);
				break;

			case EPlatformEndAction::Destroy:
				Destroy();
				return;
			}
		}
	}

	if (TargetLocations.IsValidIndex(CurrentTargetIndex))
	{
		CurrentTargetLocation = TargetLocations[CurrentTargetIndex];
		bIsMoving = true;
	}
	else
	{
		CurrentTargetIndex = 0;
		CurrentTargetLocation = TargetLocations[CurrentTargetIndex];
	}
}

