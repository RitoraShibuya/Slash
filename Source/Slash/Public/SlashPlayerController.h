// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SlashPlayerController.generated.h"

class ASlashPlayer;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 *
 */
UCLASS()
class SLASH_API ASlashPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASlashPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn)override;

	virtual void OnRep_Pawn() override;

	virtual void SetupInputComponent()override;

private:
	void Input_Move(const FInputActionValue &Value);
	void Input_Look(const FInputActionValue &Value);
	void Input_Jump();
	void Input_Sprint();
	void Released_Sprint();
	void Input_Interact();

	UPROPERTY(EditDefaultsOnly, Category = "Slash|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Slash|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Slash|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Slash|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Slash|Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Slash|Input")
	TObjectPtr<UInputAction> InteractAction;

	ASlashPlayer* Player = nullptr;

	FVector2D MoveInput = FVector2D::ZeroVector;
	FVector2D LookInput = FVector2D::ZeroVector;
};
