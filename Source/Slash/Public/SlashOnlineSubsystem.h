// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "SlashOnlineSubsystem.generated.h"

class FUniqueNetId;
/**
 * 
 */
UCLASS()
class SLASH_API USlashOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize() override;

	void StartLogin();

	bool IsLoggedIn() const { return bIsLoggedIn; }

	void CreateRoom(const FString& RoomID);

	void FindRoom(const FString& RoomID);
private:
	void OnLoginComplete(
		int32 LocalUserNum,
		bool bWasSuccessful,
		const FUniqueNetId& UserId,
		const FString& Error
	);

	bool IsValidRoomID(const FString& RoomID);

	void OnCreateSessionComplete(
		FName SessionName, 
		bool bWasSuccessful 
	);

	void OnFindSessionsComplete(bool bWasSuccessful);
private:
	TSharedPtr<IOnlineIdentity> IdentityInterface;
	FDelegateHandle LoginCompleteDelegateHandle;

	IOnlineSessionPtr SessionInterface;
	FDelegateHandle CreateSessionCompleteHandle;

	FString CurrentRoomID;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FDelegateHandle FindSessionsCompleteHandle;

	FString SearchingRoomID;

	bool bIsLoggedIn = false;
	bool bLoginRequestInProgress = false;
};
