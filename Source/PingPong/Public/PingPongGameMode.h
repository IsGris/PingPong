// All rights by IsGris

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"

#include "PingPongGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogPingPongGameMod, Log, All );

/**
 * GameMode for Ping Pong game
 */
UCLASS()
class PINGPONG_API APingPongGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	// ~ Inits
private:
	// Default constructor
	APingPongGameMode();
	// Init Game
	virtual void BeginPlay() override;
	// ~ Inits

	// ~ Variables
public:
	/** Default enemy class */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Classes )
	TSubclassOf<class AEnemyBarrier> DefaultEnemyClass;
	/** Default Enemy controller class */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Classes )
	TSubclassOf<class AEnemyController> DefaultEnemyControllerClass;
	/** Default ball class */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Classes )
	TSubclassOf<class ABall> DefaultBallClass;
	/** Default ball controller class */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Classes )
	TSubclassOf<class ABallController> DefaultBallControllerClass;
	// ~ ~ Getters
	UFUNCTION( BlueprintCallable, Category = "Get" )
	class APlayerBarrier* GetPlayer();
	UFUNCTION( BlueprintCallable, Category = "Get" )
	class AEnemyBarrier* GetEnemy();
	UFUNCTION( BlueprintCallable, Category = "Get" )
	class ABall* GetBall();
	// ~ ~ Getters
	// Actor pointers
protected:
	TObjectPtr<class APlayerBarrier> Player;
	TObjectPtr<class AEnemyBarrier> Enemy;
	TObjectPtr<class ABall> Ball;
	// How many seconds wait before game starts
	UPROPERTY( EditDefaultsOnly, Category = "Start" )
	float StartCountdownDuration = 3;
	// Timer used for countdown and start game
	struct FTimerHandle StartCountdownTimer;
	// ~ Variables

	// ~ Status
public:
	// Enable all movements for player, ball and enemy
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void EnableAllMovements();
	// Disable all movements for player, ball and enemy
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void DisableAllMovements();
	// Start ping pong game
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void StartGame();
	// Swaps current pause state if playing or paused
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SwapPause();
	// Add score to player
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void AddPlayerScore( const int& ScoreToAdd = 1 );
	// Add score to enemy
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void AddEnemyScore( const int& ScoreToAdd = 1 );
	// ~ Status
	
	// ~ Spawn
protected:
	// Spawns Enemy
	UFUNCTION( BlueprintCallable, Category = "Spawn" )
	void SpawnEnemy( const FTransform& ActorTransform );
	// Spawns Ball
	UFUNCTION( BlueprintCallable, Category = "Spawn" )
	void SpawnBall( const FTransform& ActorTransform );
	// ~ Spawn

	// ~ Find functions
public:
	// Finds enemy start Transform
	TOptional<FTransform> FindEnemyStartTransform() const;
	// Finds ball start Transform
	TOptional<FTransform> FindBallStartTransform() const;
	// Find first Player in world
	UFUNCTION( BlueprintCallable, Category = "Find" )
	class APlayerBarrier* FindPlayer() const;
	// Find first Enemy in world
	UFUNCTION( BlueprintCallable, Category = "Find" )
	class AEnemyBarrier* FindEnemy() const;
	// Find first Ball in world
	UFUNCTION( BlueprintCallable, Category = "Find" )
	class ABall* FindBall() const;
	// Find first actor with type ActorType in world
	template<typename ActorType UE_REQUIRES( TIsDerivedFrom<ActorType, AActor>::Value )>
	ActorType* FindActorWithType() const;
	// Find first actor with type ActorType in world
	UFUNCTION( BlueprintCallable, Category = "Find" )
	class AActor* FindActorWithType( UClass* ActorType ) const;
	// ~ Find functions

	// ~ Widgets
protected:
	// Widget for pause menu
	UPROPERTY( EditDefaultsOnly, Category = "Widgets" )
	TSubclassOf<UUserWidget> PauseWidget;
	// Instance of pause menu widget
	class UUserWidget* PauseWidgetInstance;
	// Widget for start countdown
	UPROPERTY( EditDefaultsOnly, Category = "Widgets" )
	TSubclassOf<class UUserWidget> StartCountdownWidget;
	// Instance of countdown widget
	TObjectPtr<class UUserWidget> StartCountdownWidgetInstance;
	// ~ Widgets
	
	// ~ Miscellaneous
	// Changes controller for pawn
	UFUNCTION( BlueprintCallable, Category = "Miscellaneous" )
	void ChangePawnController( UClass* ControllerType, APawn* Pawn );
	// Changes controller for pawn
	template<typename ControllerType UE_REQUIRES( TIsDerivedFrom<ControllerType, AController>::Value )>
	void ChangePawnController( APawn* Pawn );
	// ~ Miscellaneous
};

