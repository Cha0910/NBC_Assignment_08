// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "PoisonItem.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT_API APoisonItem : public ABaseItem
{
	GENERATED_BODY()
public:

	APoisonItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float PoisonDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float PoisonDuration;
	
	virtual void ActivateItem(AActor* Activator) override;
};
