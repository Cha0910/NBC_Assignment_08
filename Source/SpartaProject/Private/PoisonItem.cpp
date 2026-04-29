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
			float Damage = PoisonDamage;

			GetWorld()->GetTimerManager().ClearTimer(PlayerCharacter->PoisonTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(PlayerCharacter->PoisonDurationHandle);
			
			GetWorld()->GetTimerManager().SetTimer(PlayerCharacter->PoisonTimerHandle, [PlayerCharacter, Damage]()
			{
				if (IsValidChecked(PlayerCharacter))
				{
					 UGameplayStatics::ApplyDamage(PlayerCharacter, Damage, nullptr, nullptr, UDamageType::StaticClass());
				}
			}, 1.0f, true);

			GetWorld()->GetTimerManager().SetTimer(PlayerCharacter->PoisonDurationHandle, [PlayerCharacter]()
			{
				 if (IsValidChecked(PlayerCharacter))
				 {
					 PlayerCharacter->GetWorld()->GetTimerManager().ClearTimer(PlayerCharacter->PoisonTimerHandle);
				 }
			}, PoisonDuration, false);
		}
	}

	DestroyItem();
}