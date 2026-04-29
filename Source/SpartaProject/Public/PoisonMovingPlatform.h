// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovingPlatform.h"
#include "Components/BoxComponent.h"
#include "PoisonMovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API APoisonMovingPlatform : public AMovingPlatform
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform|Component")
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float PoisonDamage;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float PoisonDuration;

public:
	APoisonMovingPlatform();

	
	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult
	);
	
};
