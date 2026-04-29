// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowingItem.h"
#include "SpartaCharacter.h"

// Sets default values
ASlowingItem::ASlowingItem()
{
	PrimaryActorTick.bCanEverTick = false;

	SlowAmount = 0.5f;
	SlowDuration = 5.0f;
	ItemType = "Slowing";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if(Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->SetSpeedMultiplier(SlowAmount);

			GetWorld()->GetTimerManager().SetTimer(PlayerCharacter->SlowTimerHandle, [PlayerCharacter]()
			{
				if(IsValidChecked(PlayerCharacter))
				{
					PlayerCharacter->SetSpeedMultiplier(1.0f);
				}
			}, SlowDuration, false);
		}
		
		DestroyItem();
	}
}

