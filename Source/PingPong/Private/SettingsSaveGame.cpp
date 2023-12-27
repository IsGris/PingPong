// All rights by IsGris


#include "SettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"

void USettingsSaveGame::SetEffectsVolume( const float& NewVolume )
{
	EffectsVolume = NewVolume;
}

float USettingsSaveGame::GetEffectsVolume() const
{
	return EffectsVolume;
}

void USettingsSaveGame::SaveData()
{
	UGameplayStatics::SaveGameToSlot( Cast<USaveGame>( this ), SaveSlotName, UserIndex );
}
