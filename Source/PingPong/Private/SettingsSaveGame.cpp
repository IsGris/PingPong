// All rights by IsGris


#include "SettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"

void USettingsSaveGame::SaveData()
{
	UGameplayStatics::SaveGameToSlot( Cast<USaveGame>( this ), SaveSlotName, UserIndex );
}
