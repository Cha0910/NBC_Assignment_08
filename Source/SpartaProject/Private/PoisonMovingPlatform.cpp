// Fill out your copyright notice in the Description page of Project Settings.


#include "PoisonMovingPlatform.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"

APoisonMovingPlatform::APoisonMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	PoisonDamage = 5.0f;
	PoisonDuration = 10.0f;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
}

void APoisonMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APoisonMovingPlatform::OnOverlapBegin);
}

void APoisonMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APoisonMovingPlatform::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(OtherActor))
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
}