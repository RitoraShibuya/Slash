// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashTitleWidget.generated.h"

class UEditableTextBox;
class UButton;

/**
 * 
 */
UCLASS()
class SLASH_API USlashTitleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void OnJoinButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> RoomIDTextBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;
};
