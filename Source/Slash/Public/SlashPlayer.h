// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlashCharacterBase.h"
#include "SlashPlayer.generated.h"

class UCharacterMovementComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class SLASH_API ASlashPlayer : public ASlashCharacterBase
{
	GENERATED_BODY()
	
public:
	ASlashPlayer();

	void Move(FVector2D MoveValue);

	void Look(FVector2D LookValue);

	void StartJump();
	void StopJump();

	void StartSprint();
	void StopSprint();

	void Interact();

private:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bNewSprinting);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	float NormalSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	float SprintSpeed = 900.0f;


	UPROPERTY(EditDefaultsOnly, Category = "Slash")
	TObjectPtr<UCameraComponent> CameraComponent;

	UCharacterMovementComponent* MovementComponent;

	bool bIsSprinting = false;
};
