// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PingPongLibrary.generated.h"

/**
 * Library for blueprints in ping pong game
 */
UCLASS()
class PINGPONG_API UPingPongLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "PingPong", meta=(WorldContext = "WorldContext") )
	static class APingPongGameMode* GetPingPongGameMode( const UObject* WorldContext );
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "PingPong", meta = ( WorldContext = "WorldContext" ) )
	static class UPingPongGameInstance* GetPingPongGameInstance( const UObject* WorldContext );
};
