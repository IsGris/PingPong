// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PingPongGameState.generated.h"

// Describes current game status
UENUM()
enum GameStatus
{
	GameOver       UMETA( DisplayName = "GameOver" ),
	Paused         UMETA( DisplayName = "Paused" ),
	Playing        UMETA( DisplayName = "Playing" ),
	Starting       UMETA( DisplayName = "Starting" )
};

/**
 * GameState for Ping Pong game
 */
UCLASS()
class PINGPONG_API APingPongGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// Current Game Status
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Status" )
	TEnumAsByte<GameStatus> CurrentGameStatus = GameStatus::Starting;
};
