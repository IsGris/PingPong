// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PingPongGameInstance.generated.h"

/**
 * Game instance for ping pong
 */
UCLASS()
class PINGPONG_API UPingPongGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	// ~ Audio
public:
	// Change current effects volume
	UFUNCTION( BlueprintCallable, Category = "Audio|Volume" )
	void SetEffectsVolume( const float& NewVolume );
	// Get current effects volume
	UFUNCTION( BlueprintCallable, Category = "Audio|Volume" )
	float GetEffectsVolume() const;
	// ~ Audio
};
