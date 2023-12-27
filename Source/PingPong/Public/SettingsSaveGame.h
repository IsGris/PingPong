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
	
private:
	UPROPERTY( SaveGame, EditDefaultsOnly, Category = "Audio" )
	float EffectsVolume = 1.0f;

public:
	UPROPERTY( VisibleAnywhere, Category = "data" )
	FString SaveSlotName = DEFAULT_SETTINGS_SAVE_SLOT;
	UPROPERTY( VisibleAnywhere, Category = "Data" )
	uint32 UserIndex = 0;

	UFUNCTION( BlueprintCallable, Category = "Audio" )
	void SetEffectsVolume( const float& NewVolume );
	UFUNCTION( BlueprintCallable, Category = "Audio" )
	float GetEffectsVolume() const;

	UFUNCTION( BlueprintCallable, Category = "Data" )
	void SaveData();
};
