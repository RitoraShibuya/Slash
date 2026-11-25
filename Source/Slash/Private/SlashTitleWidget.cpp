// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashTitleWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "SlashOnlineSubsystem.h"

void USlashTitleWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	JoinButton->OnClicked.AddDynamic(this, &USlashTitleWidget::OnJoinButtonClicked);
}

void USlashTitleWidget::OnJoinButtonClicked() {
	USlashOnlineSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USlashOnlineSubsystem>();
	if (Subsystem) {
		const FString RoomID = RoomIDTextBox->GetText().ToString();

		Subsystem->CreateRoom(RoomID);
	}
}
