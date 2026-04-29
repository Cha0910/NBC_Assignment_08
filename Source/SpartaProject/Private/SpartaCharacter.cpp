#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include <EnhancedInputSubsystems.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "SpartaGameState.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f;
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	OverheadDebuffWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadDebuffWidget"));
	OverheadDebuffWidget->SetupAttachment(GetMesh());
	OverheadDebuffWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	bIsSprinting = false;
	bIsReverseControls = false;

	NormalSpeed = 600.f;
	SprintSpeedMultiplier = 1.7f;
	SpeedMultiplier = 1.0f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier * SpeedMultiplier;
	UpdateMovementSpeed();

	MaxHealth = 100.f;
	Health = MaxHealth;

	SlowTimerHandle.Invalidate();
	PoisonTimerHandle.Invalidate();
	PoisonDurationHandle.Invalidate();
	ReverseControlsTimerHandle.Invalidate();
	DebuffWidgetTimerHandle.Invalidate();
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();

	GetWorldTimerManager().ClearTimer(DebuffWidgetTimerHandle);
	GetWorldTimerManager().SetTimer(DebuffWidgetTimerHandle, this, &ASpartaCharacter::UpdateOverheadDebuffs, 0.1f, true);
}

void ASpartaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if(ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartJump);
				EnhancedInputComponent->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Look);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartSprint);
				EnhancedInputComponent->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopSprint);
			}
		}
	}
}


void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), bIsReverseControls ? -MoveInput.X : MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), bIsReverseControls ? -MoveInput.Y : MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if(value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if(!value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	
	AddControllerYawInput(bIsReverseControls ? -LookInput.X : LookInput.X);
	AddControllerPitchInput(bIsReverseControls ? -LookInput.Y : LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	if(GetCharacterMovement())
	{
		bIsSprinting = true;
		UpdateMovementSpeed();
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		bIsSprinting = false;
		UpdateMovementSpeed();
	}
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float HealthAmount)
{
	Health += HealthAmount;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);
	UpdateOverheadHP();
}

float ASpartaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= ActualDamage;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);
	UpdateOverheadHP();

	if(Health <= 0.f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SparataGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if(SparataGameState)
	{
		SparataGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if(!OverheadWidget)
	{
		return;
	}

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if(!OverheadWidgetInstance)
	{
		return;
	}

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP")))) 
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("HP: %.0f / %.0f"), Health, MaxHealth)));
	}
}

void ASpartaCharacter::UpdateMovementSpeed()
{
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed * SpeedMultiplier;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * SpeedMultiplier;
	}
}

void ASpartaCharacter::SetSpeedMultiplier(float NewSpeedMultiplier)
{
	SpeedMultiplier = NewSpeedMultiplier;
	UpdateMovementSpeed();
}

void ASpartaCharacter::UpdateOverheadDebuffs()
{
	if(!OverheadDebuffWidget)
	{
		return;
	}

	UUserWidget* OverheadDebuffWidgetInstance = OverheadDebuffWidget->GetUserWidgetObject();
	if(!OverheadDebuffWidgetInstance)
	{
		return;
	}

	UFunction* Func = OverheadDebuffWidgetInstance->FindFunction(TEXT("UpdateDebuffs"));

	FDebuffData DebuffsToUpdate[] = {
		{ TEXT("Slow"), SlowTimerHandle, FLinearColor::Blue },
		{ TEXT("Poison"), PoisonDurationHandle, FLinearColor::Green },
		{ TEXT("ReverseControls"), ReverseControlsTimerHandle, FLinearColor::Red }
	};

	for (const FDebuffData& Debuff : DebuffsToUpdate)
	{
		FUpdateDebuffParams Params;
		Params.ID = Debuff.ID;
		Params.Color = Debuff.Color;
		Params.Percent = 0.0f;

		if (GetWorldTimerManager().IsTimerActive(Debuff.Handle))
		{
			float Remaining = GetWorldTimerManager().GetTimerRemaining(Debuff.Handle);
			float Total = GetWorldTimerManager().GetTimerRate(Debuff.Handle);

			Params.Percent = (Total > 0.f) ? FMath::Clamp(Remaining / Total, 0.f, 1.f) : 0.f;
		}

		OverheadDebuffWidgetInstance->ProcessEvent(Func, &Params);
	}
}