// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"

ASlashPlayer::ASlashPlayer() {
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->SetupAttachment(RootComponent);


	MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
}

void ASlashPlayer::BeginPlay() {
	Super::BeginPlay();

	MovementComponent->MaxWalkSpeed = NormalSpeed;
}

void ASlashPlayer::Move(FVector2D MoveValue) {
	if (!Controller) {
		return;
	}

	FRotator Rotation = Controller->GetControlRotation();
	Rotation.Pitch = 0.0f;
	Rotation.Roll = 0.0f;

	AddMovementInput(Rotation.Vector(), MoveValue.Y);
	AddMovementInput(Rotation.RotateVector(FVector::RightVector), MoveValue.X);
}

void ASlashPlayer::Look(FVector2D LookValue) {
	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void ASlashPlayer::StartJump() {
	Jump();
}

void ASlashPlayer::StopJump() {

}

void ASlashPlayer::StartSprint() {
	if (bIsSprinting) {
		return;
	}
	bIsSprinting = true;
	MovementComponent->MaxWalkSpeed = SprintSpeed;
	ServerSetSprinting(true);
}

void ASlashPlayer::StopSprint() {
	if (!bIsSprinting) {
		return;
	}
	bIsSprinting = false;
	MovementComponent->MaxWalkSpeed = NormalSpeed;
	ServerSetSprinting(false);
}

void ASlashPlayer::Interact() {

}

void ASlashPlayer::ServerSetSprinting_Implementation(bool bNewSprinting) {
	bIsSprinting = bNewSprinting;
	if(bIsSprinting){
		MovementComponent->MaxWalkSpeed = NormalSpeed;
	}
	else {
		MovementComponent->MaxWalkSpeed = SprintSpeed;
	}
}