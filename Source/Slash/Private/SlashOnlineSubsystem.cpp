// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashOnlineSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

void USlashOnlineSubsystem::Initialize(
	FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//UE_LOG(LogTemp, Log,TEXT("[SlashOnline] Initializing"));

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem)
	{
		return;
	}

	//UE_LOG(LogTemp, Log,TEXT("[SlashOnline] Subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString());

	IdentityInterface = OnlineSubsystem->GetIdentityInterface();

	if (!IdentityInterface.IsValid())
	{
		return;
	}

	LoginCompleteDelegateHandle =
		IdentityInterface->AddOnLoginCompleteDelegate_Handle(
			0,
			FOnLoginCompleteDelegate::CreateUObject(
				this,
				&USlashOnlineSubsystem::OnLoginComplete
			)
		);

	StartLogin();
}

void USlashOnlineSubsystem::Deinitialize()
{
	if (IdentityInterface.IsValid() &&
		LoginCompleteDelegateHandle.IsValid())
	{
		IdentityInterface->ClearOnLoginCompleteDelegate_Handle(
			0,
			LoginCompleteDelegateHandle
		);
	}

	IdentityInterface.Reset();

	Super::Deinitialize();
}

void USlashOnlineSubsystem::StartLogin()
{
	if (!IdentityInterface.IsValid())
	{
		//UE_LOG(LogTemp, Error,TEXT("[SlashOnline] IdentityInterface invalid"));
		return;
	}

	if (bLoginRequestInProgress)
	{
		return;
	}

	if (IdentityInterface->GetLoginStatus(0) ==
		ELoginStatus::LoggedIn)
	{
		bIsLoggedIn = true;
		return;
	}

	//UE_LOG(LogTemp, Warning,TEXT("[SlashOnline] CommandLine: %s"),FCommandLine::Get());

	bLoginRequestInProgress = true;

	//UE_LOG(LogTemp, Warning,TEXT("[SlashOnline] Starting AutoLogin"));

	const bool bStarted = IdentityInterface->AutoLogin(0);

	//UE_LOG(LogTemp, Warning,TEXT("[SlashOnline] AutoLogin Started: %s"),bStarted ? TEXT("true") : TEXT("false"));

	if (!bStarted)
	{
		bLoginRequestInProgress = false;

		//UE_LOG(LogTemp, Error,TEXT("[SlashOnline] AutoLogin could not start"));
	}
}

void USlashOnlineSubsystem::CreateRoom(const FString& RoomID) {
	if (!IsValidRoomID(RoomID)) {
		return;
	}



	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem) {
		return;
	}

	///

	IdentityInterface =
		OnlineSubsystem->GetIdentityInterface();

	if (!IdentityInterface.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SlashOnline] IdentityInterface not found.")
		);
		return;
	}

	// ログイン状態確認
	const ELoginStatus::Type LoginStatus =
		IdentityInterface->GetLoginStatus(0);

	if (LoginStatus != ELoginStatus::LoggedIn)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SlashOnline] User 0 is not logged in. Status: %d"),
			static_cast<int32>(LoginStatus)
		);
		return;
	}

	// ユーザーID確認
	const TSharedPtr<const FUniqueNetId> UserID =
		IdentityInterface->GetUniquePlayerId(0);

	if (!UserID.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SlashOnline] User 0 has no valid UserID.")
		);
		return;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[SlashOnline] Identity verified. UserID: %s"),
		*UserID->ToString()
	);

	///

	SessionInterface = OnlineSubsystem->GetSessionInterface();

	if (!SessionInterface.IsValid()) {
		return;
	}

	const FName SessionName = FName(TEXT("GameSession"));

	if (SessionInterface->GetNamedSession(SessionName)) {
		return;
	}

	CurrentRoomID = RoomID;

	FOnlineSessionSettings SessionSettings;

	SessionSettings.bIsLANMatch = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bAllowInvites = true;
	SessionSettings.bUseLobbiesIfAvailable = true;

	SessionSettings.NumPublicConnections = 4;
	SessionSettings.NumPrivateConnections = 0;

	SessionSettings.Set(
		FName(TEXT("ROOM_ID")),
		RoomID,
		EOnlineDataAdvertisementType::ViaOnlineService
	);

	CreateSessionCompleteHandle =
		SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
			FOnCreateSessionCompleteDelegate::CreateUObject(
				this,
				&USlashOnlineSubsystem::OnCreateSessionComplete
			)
		);
	if (!SessionInterface->CreateSession(0, SessionName, SessionSettings)) {
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
			CreateSessionCompleteHandle
		);
		CreateSessionCompleteHandle.Reset();
		CurrentRoomID.Empty();
		return;
	}
}

void USlashOnlineSubsystem::FindRoom(const FString& RoomID) {
	if (!IsValidRoomID(RoomID))
	{
		UE_LOG(LogTemp, Error, TEXT("[SlashOnline] Invalid RoomID: %s"), *RoomID);
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[SlashOnline] OnlineSubsystem is invalid."));
		return;
	}

	IdentityInterface = OnlineSubsystem->GetIdentityInterface();

	if (!IdentityInterface.IsValid() ||
		IdentityInterface->GetLoginStatus(0) != ELoginStatus::LoggedIn)
	{
		UE_LOG(LogTemp, Error, TEXT("[SlashOnline] User is not logged in."));
		return;
	}

	SessionInterface = OnlineSubsystem->GetSessionInterface();

	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[SlashOnline] SessionInterface is invalid."));
		return;
	}

	if (SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SlashOnline] Session search is already running."));
		return;
	}

	SearchingRoomID = RoomID;

	SessionSearch = MakeShared<FOnlineSessionSearch>();

	SessionSearch->MaxSearchResults = 100;
	SessionSearch->bIsLanQuery = false;

	SessionSearch->QuerySettings.Set(
		SEARCH_LOBBIES,
		true,
		EOnlineComparisonOp::Equals
	);

	SessionSearch->QuerySettings.Set(
		FName(TEXT("ROOM_ID")),
		RoomID,
		EOnlineComparisonOp::Equals
	);

	FindSessionsCompleteHandle =
		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
			FOnFindSessionsCompleteDelegate::CreateUObject(
				this,
				&USlashOnlineSubsystem::OnFindSessionsComplete
			)
		);

	const bool bStarted = SessionInterface->FindSessions(
		0,
		SessionSearch.ToSharedRef()
	);

	if (!bStarted)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
			FindSessionsCompleteHandle
		);

		FindSessionsCompleteHandle.Reset();
		SessionSearch.Reset();
		SearchingRoomID.Empty();

		UE_LOG(LogTemp, Error, TEXT("[SlashOnline] FindSessions request failed."));
		return;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[SlashOnline] FindSessions requested. RoomID: %s"),
		*SearchingRoomID
	);

}

void USlashOnlineSubsystem::OnLoginComplete(
	int32 LocalUserNum,
	bool bWasSuccessful,
	const FUniqueNetId& UserId,
	const FString& Error)
{
	bLoginRequestInProgress = false;
	bIsLoggedIn = bWasSuccessful;

	/* if (bWasSuccessful)
	 {
		 UE_LOG(LogTemp, Log,
			 TEXT("[SlashOnline] Login Success: %s"),
			 *UserId.ToString());
	 }
	 else
	 {
		 UE_LOG(LogTemp, Error,
			 TEXT("[SlashOnline] Login Failed: %s"),
			 *Error);
	 }*/
}

bool USlashOnlineSubsystem::IsValidRoomID(const FString& RoomID) {
	if (RoomID.Len() != 4) {
		return false;
	}

	for (const TCHAR Character : RoomID)
	{
		if (!(Character >= TEXT('0') && Character <= TEXT('9'))) {
			return false;
		}
	}

	return true;
}

void USlashOnlineSubsystem::OnCreateSessionComplete(
	FName SessionName,
	bool bWasSuccessful
) {
	if (SessionInterface.IsValid()) {
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
			CreateSessionCompleteHandle
		);
	}

	CreateSessionCompleteHandle.Reset();

	if (!bWasSuccessful) {
		CurrentRoomID.Empty();
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[SlashOnline]RoomID:%s"), *CurrentRoomID);

}

void USlashOnlineSubsystem::OnFindSessionsComplete(bool bWasSuccessful) {
	const FString RequestedRoomID = SearchingRoomID;

	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
			FindSessionsCompleteHandle
		);
	}

	FindSessionsCompleteHandle.Reset();

	if (!bWasSuccessful || !SessionSearch.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SlashOnline] Session search failed.")
		);

		SessionSearch.Reset();
		SearchingRoomID.Empty();
		return;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[SlashOnline] Search completed. Results: %d"),
		SessionSearch->SearchResults.Num()
	);

	bool bFoundMatchingRoom = false;

	for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
	{
		FString FoundRoomID;

		const bool bHasRoomID =
			SearchResult.Session.SessionSettings.Get(
				FName(TEXT("ROOM_ID")),
				FoundRoomID
			);

		if (!bHasRoomID)
		{
			continue;
		}

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[SlashOnline] Found RoomID: %s"),
			*FoundRoomID
		);

		if (FoundRoomID == RequestedRoomID)
		{
			bFoundMatchingRoom = true;

			UE_LOG(
				LogTemp,
				Log,
				TEXT("[SlashOnline] Matching room found: %s"),
				*FoundRoomID
			);

			// TODO: 次のステップでJoinRoom()を呼び出す
			break;
		}
	}

	SessionSearch.Reset();
	SearchingRoomID.Empty();

	if (!bFoundMatchingRoom)
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("[SlashOnline] Room not found. Creating new room: %s"),
			*RequestedRoomID
		);

		CreateRoom(RequestedRoomID);
	}
}