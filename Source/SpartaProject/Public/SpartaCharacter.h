#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UWidgetComponent;
struct FInputActionValue;

struct FUpdateDebuffParams
{
	FName ID;
	double Percent;
	FLinearColor Color;
};

struct FDebuffData { FName ID; FTimerHandle& Handle; FLinearColor Color; };

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadDebuffWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsReverseControls;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float HealthAmount);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetSpeedMultiplier(float NewSpeedMultiplier);

	FTimerHandle SlowTimerHandle;
	FTimerHandle PoisonTimerHandle;
	FTimerHandle PoisonDurationHandle;
	FTimerHandle ReverseControlsTimerHandle;
	FTimerHandle DebuffWidgetTimerHandle;
	

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void BeginPlay() override;

	UFUNCTION()
	void Move(const FInputActionValue& value);

	UFUNCTION()
	void StartJump(const FInputActionValue& value);

	UFUNCTION()
	void StopJump(const FInputActionValue& value);

	UFUNCTION()
	void Look(const FInputActionValue& value);

	UFUNCTION()
	void StartSprint(const FInputActionValue& value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void OnDeath();

	void UpdateOverheadHP();

	void UpdateMovementSpeed();

	void UpdateOverheadDebuffs();

	void Tick(float DeltaTime) override;

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	float SpeedMultiplier;
};
