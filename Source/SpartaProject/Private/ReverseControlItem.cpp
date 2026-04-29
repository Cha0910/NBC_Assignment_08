// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseControlItem.h"
#include "SpartaCharacter.h"

AReverseControlItem::AReverseControlItem()
{
	ReverseDuration = 8.0f;
}

void AReverseControlItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if(Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->bIsReverseControls = true;
			GetWorld()->GetTimerManager().ClearTimer(PlayerCharacter->ReverseControlsTimerHandle);

			GetWorld()->GetTimerManager().SetTimer(PlayerCharacter->ReverseControlsTimerHandle, [PlayerCharacter]()
			{
				if (IsValidChecked(PlayerCharacter))
				{
					PlayerCharacter->bIsReverseControls = false;
				}
			}, ReverseDuration, false);
		}
	}

	DestroyItem();
}