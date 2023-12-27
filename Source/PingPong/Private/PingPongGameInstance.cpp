// All rights by IsGris


#include "PingPongGameInstance.h"
#include "SettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UPingPongGameInstance::Init()
{
	SettingsSave = Cast<USettingsSaveGame>( UGameplayStatics::LoadGameFromSlot( DEFAULT_SETTINGS_SAVE_SLOT, 0 ) );
	if ( !SettingsSave )
	{
		SettingsSave = Cast<USettingsSaveGame>( UGameplayStatics::CreateSaveGameObject( USettingsSaveGame::StaticClass() ) );
		SettingsSave->SaveData();
	}
}

void UPingPongGameInstance::SetEffectsVolume( const float& NewVolume )
{
	if ( !SettingsSave )
		return;

	SettingsSave->SetEffectsVolume( NewVolume );
	SettingsSave->SaveData();
}

float UPingPongGameInstance::GetEffectsVolume() const
{
	if ( !SettingsSave )
		return 1.0f;

	return SettingsSave->GetEffectsVolume();
}
