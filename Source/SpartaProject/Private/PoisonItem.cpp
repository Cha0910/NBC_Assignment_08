// Fill out your copyright notice in the Description page of Project Settings.


#include "PoisonItem.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"

APoisonItem::APoisonItem()
{
	PrimaryActorTick.bCanEverTick = false;
	PoisonDamage = 5.0f;
	PoisonDuration = 10.0f;
	ItemType = "Poison";
}

void APoisonItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			if (UWorld* World = GetWorld()) 
			{
				TWeakObjectPtr<ASpartaCharacter> PlayerPtr(PlayerCharacter);

				float Damage = PoisonDamage;
				float Duration = PoisonDuration;

				World->GetTimerManager().ClearTimer(PlayerCharacter->PoisonTimerHandle);
				World->GetTimerManager().ClearTimer(PlayerCharacter->PoisonDurationHandle);

				World->GetTimerManager().SetTimer(PlayerCharacter->PoisonTimerHandle, [PlayerPtr, Damage]()
				{
					if (PlayerPtr.IsValid())
					{
						UGameplayStatics::ApplyDamage(PlayerPtr.Get(), Damage, nullptr, nullptr, UDamageType::StaticClass());
					}
				}, 1.0f, true);

				World->GetTimerManager().SetTimer(PlayerCharacter->PoisonDurationHandle, [PlayerPtr, Duration]()
				{
					if (PlayerPtr.IsValid())
					{
						UWorld* PWorld = PlayerPtr->GetWorld();
						if (PWorld)
						{
							PWorld->GetTimerManager().ClearTimer(PlayerPtr->PoisonTimerHandle);
							PWorld->GetTimerManager().ClearTimer(PlayerPtr->PoisonDurationHandle);
						}
					}
				}, Duration, false);
			}
		}
	}

	DestroyItem();
}