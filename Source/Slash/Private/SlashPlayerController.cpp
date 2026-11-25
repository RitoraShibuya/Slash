// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "SlashPlayer.h"

ASlashPlayerController::ASlashPlayerController() {

}

void ASlashPlayerController::BeginPlay() {
	Super::BeginPlay();

	if (!IsLocalController()) {
		return;
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer()) {

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {

			if (DefaultMappingContext) {

				Subsystem->AddMappingContext(
					DefaultMappingContext,
					0
				);
			}
		}
	}
}

void ASlashPlayerController::OnPossess(APawn* InPawn) {

	Super::OnPossess(InPawn);
	Player = Cast<ASlashPlayer>(GetPawn());
}

void ASlashPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	Player = Cast<ASlashPlayer>(GetPawn());
}


void ASlashPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(InputComponent);

	if (!EnhancedInput) {
		return;
	}

	if (MoveAction) {
		EnhancedInput->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&ASlashPlayerController::Input_Move
		);
	}

	if (LookAction) {
		EnhancedInput->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&ASlashPlayerController::Input_Look
		);
	}

	if (JumpAction) {
		EnhancedInput->BindAction(
			JumpAction,
			ETriggerEvent::Started,
			this,
			&ASlashPlayerController::Input_Jump
		);
	}

	if (SprintAction) {
		EnhancedInput->BindAction(
			SprintAction,
			ETriggerEvent::Started,
			this,
			&ASlashPlayerController::Input_Sprint
		);
	}

	if (SprintAction) {
		EnhancedInput->BindAction(
			SprintAction,
			ETriggerEvent::Completed,
			this,
			&ASlashPlayerController::Released_Sprint
		);
	}

	if (InteractAction) {
		EnhancedInput->BindAction(
			InteractAction,
			ETriggerEvent::Started,
			this,
			&ASlashPlayerController::Input_Interact
		);
	}
}

void ASlashPlayerController::Input_Move(const FInputActionValue &Value) {
	if (!Player) {
		return;
	}

	MoveInput = Value.Get<FVector2D>();

	Player->Move(MoveInput);
}

void ASlashPlayerController::Input_Look(const FInputActionValue &Value) {
	if (!Player) {
		return;
	}

	LookInput = Value.Get<FVector2D>();
	
	Player->Look(LookInput);
}

void ASlashPlayerController::Input_Jump() {
	if (!Player) {
		return;
	}

	Player->StartJump();
}

void ASlashPlayerController::Input_Sprint() {
	if (!Player) {
		return;
	}

	Player->StartSprint();
}

void ASlashPlayerController::Released_Sprint() {
	if (!Player) {
		return;
	}

	Player->StopSprint();
}

void ASlashPlayerController::Input_Interact() {
	if (!Player) {
		return;
	}

	Player->Interact();
}