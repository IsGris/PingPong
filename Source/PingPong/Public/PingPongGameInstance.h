// All rights by IsGris

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"
#include "SettingsSaveGame.h"

#include "PingPongGameInstance.generated.h"

/**
 * Game instance for ping pong
 */
UCLASS()
class PINGPONG_API UPingPongGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	// Init game instance
	virtual void Init() override;
	// Init settings
	void InitUserSettings();

	// ~ Settings
protected:
	TObjectPtr<USettingsSaveGame> SettingsSave;
public:
	// Save current game data to file
	UFUNCTION( BlueprintCallable, Category = "Data" )
	void SaveGame();
	// ~ ~ Audio
public:
	// Change current effects volume
	UFUNCTION( BlueprintCallable, Category = "Settings|Audio|Volume" )
	void SetEffectsVolume( const float& NewVolume, const bool& SaveSettings = false );
	// Get current effects volume
	UFUNCTION( BlueprintCallable, Category = "Settings|Audio|Volume" )
	float GetEffectsVolume() const;
	// ~ ~ Audio
	// ~ ~ Screen
public:
	// Set current Vsync state
	UFUNCTION( BlueprintCallable, Category = "Settings|Screen|V-Sync" )
	void SetVSyncEnabled( const bool& IsEnabled );
	// Get current Vsync state
	UFUNCTION( BlueprintCallable, Category = "Settings|Screen|V-Sync" )
	bool GetVSyncEnabled() const;
	// Set current FPS maximum
	UFUNCTION( BlueprintCallable, Category = "Settings|Screen|FPS" )
	void SetFPSLimit( const float& NewFPSLimit );
	// Get current FPS maximum
	UFUNCTION( BlueprintCallable, Category = "Settings|Screen|FPS" )
	float GetFPSLimit() const;
	// ~ ~ Screen
	// ~ Settings
};
