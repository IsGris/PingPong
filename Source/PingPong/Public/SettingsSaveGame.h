// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingsSaveGame.generated.h"

#define DEFAULT_SETTINGS_SAVE_SLOT "DefaultSettings"

/**
 * Save Game for settings
 */
UCLASS()
class PINGPONG_API USettingsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY( SaveGame, EditDefaultsOnly, Category = "Audio" )
	float EffectsVolume = 1.0f;
	UPROPERTY( SaveGame, EditDefaultsOnly, Category = "Screen" )
	bool IsVSyncEnabled = false;
	UPROPERTY( SaveGame, EditDefaultsOnly, Category = "Screen" )
	float FPS = 60.0f;

	UPROPERTY( VisibleAnywhere, Category = "Data" )
	FString SaveSlotName = DEFAULT_SETTINGS_SAVE_SLOT;
	UPROPERTY( VisibleAnywhere, Category = "Data" )
	uint32 UserIndex = 0;

	UFUNCTION( BlueprintCallable, Category = "Data" )
	void SaveData();
};
