#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformEndAction : uint8
{
	None          UMETA(DisplayName = "None (Stop at End)"),
	Reset         UMETA(DisplayName = "Reset (Teleport to Start)"),
	Return		  UMETA(DisplayName = "Return (Follow Return Mode)"),
	Destroy       UMETA(DisplayName = "Destroy"),
};

UENUM(BlueprintType)
enum class EPlatformReturnMode : uint8
{
	Loop          UMETA(DisplayName = "Loop (Continuous Cycle)"),
	PingPong      UMETA(DisplayName = "PingPong (Reverse Path)"),
};

UCLASS()
class SPARTAPROJECT_API AMovingPlatform : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMovingPlatform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving Platform|Components")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Components")
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Properties")
	bool bIsMoving = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Properties")
	bool bIsRotating = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Properties")
	EPlatformEndAction EndAction = EPlatformEndAction::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Properties")
	EPlatformReturnMode ReturnMode = EPlatformReturnMode::Loop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Properties")
	float MovementSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Properties")
	float RotationSpeed = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Properties")
	float MovingDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Platform|Properties")
	TArray<AActor*> TargetPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving Platform|Debug")
	FVector StartLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving Platform|Debug")
	TArray<FVector> TargetLocations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Moving Platform|Debug")
	float DistanceThreshold = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving Platform|Debug")
	FVector CurrentTargetLocation;

	int32 CurrentTargetIndex = 0;
	int32 IndexDirection = 1;

	FTimerHandle TimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupNextTargetLocation();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
