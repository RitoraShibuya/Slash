// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashCharacterBase.h"

// Sets default values
ASlashCharacterBase::ASlashCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);


}

// Called when the game starts or when spawned
void ASlashCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlashCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASlashCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

