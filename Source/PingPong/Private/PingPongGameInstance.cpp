// All rights by IsGris


#include "PingPongGameInstance.h"
#include "SettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

void UPingPongGameInstance::Init()
{
	InitUserSettings();
}

void UPingPongGameInstance::InitUserSettings()
{
	SettingsSave = Cast<USettingsSaveGame>( UGameplayStatics::LoadGameFromSlot( DEFAULT_SETTINGS_SAVE_SLOT, 0 ) );
	if ( !SettingsSave )
	{
		SettingsSave = Cast<USettingsSaveGame>( UGameplayStatics::CreateSaveGameObject( USettingsSaveGame::StaticClass() ) );
		SettingsSave->SaveData();
	}

	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->SetVSyncEnabled( SettingsSave->IsVSyncEnabled );
	Settings->SetFrameRateLimit( SettingsSave->FPS );
	Settings->ApplySettings( true );
}

void UPingPongGameInstance::SetEffectsVolume( const float& NewVolume )
{
	if ( !SettingsSave )
		return;

	SettingsSave->EffectsVolume = NewVolume;
	SettingsSave->SaveData();
}

float UPingPongGameInstance::GetEffectsVolume() const
{
	if ( !SettingsSave )
		return 1.0f;

	return SettingsSave->EffectsVolume;
}

void UPingPongGameInstance::SetVSyncEnabled( const bool& IsEnabled )
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if ( !SettingsSave || !Settings )
		return;
	Settings->SetVSyncEnabled( IsEnabled );
	Settings->ApplySettings( true );
	SettingsSave->IsVSyncEnabled = IsEnabled;
	SettingsSave->SaveData();
}

bool UPingPongGameInstance::GetVSyncEnabled() const
{
	if ( !SettingsSave )
		return false;
	else
		return SettingsSave->IsVSyncEnabled;
}

void UPingPongGameInstance::SetFPSLimit( const float& NewFPSLimit )
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if ( !SettingsSave || !Settings )
		return;
	Settings->SetFrameRateLimit( NewFPSLimit );
	Settings->ApplySettings( true );
	SettingsSave->FPS = NewFPSLimit;
	SettingsSave->SaveData();
}

float UPingPongGameInstance::GetFPSLimit() const
{
	if ( !SettingsSave )
		return false;
	else
		return SettingsSave->FPS;
}
