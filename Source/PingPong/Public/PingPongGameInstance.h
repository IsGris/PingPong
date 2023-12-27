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

protected:
	TObjectPtr<USettingsSaveGame> SettingsSave;

	// ~ Audio
public:
	// Change current effects volume
	UFUNCTION( BlueprintCallable, Category = "Audio|Volume" )
	void SetEffectsVolume( const float& NewVolume );
	// Get current effects volume
	UFUNCTION( BlueprintCallable, Category = "Audio|Volume" )
	float GetEffectsVolume() const;
	// ~ Audio
	// ~ Screen
public:
	// Set current Vsync state
	UFUNCTION( BlueprintCallable, Category = "Screen|V-Sync" )
	void SetVSyncEnabled( const bool& IsEnabled );
	// Get current Vsync state
	UFUNCTION( BlueprintCallable, Category = "Screen|V-Sync" )
	bool GetVSyncEnabled() const;
	// Set current FPS maximum
	UFUNCTION( BlueprintCallable, Category = "Screen|FPS" )
	void SetFPSLimit( const float& NewFPSLimit );
	// Get current FPS maximum
	UFUNCTION( BlueprintCallable, Category = "Screen|FPS" )
	float GetFPSLimit() const;
	// ~ Screen
};
