// All rights by IsGris

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "PingPongGameState.h"

#include "PingPongGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnGameStatusChanged, const TEnumAsByte<GameStatus>&, NewGameStatus, const TEnumAsByte<GameStatus>&, OldGameStatus );

class FOnGameStatusChanged;

// Describes all sounds in game
UENUM()
enum class PingPongSound
{
	BallBounce       UMETA( DisplayName = "BallBounce" ),
	EnemyScored        UMETA( DisplayName = "EnemyScored" ),
	PlayerScored       UMETA( DisplayName = "PlayerScored" ),
	Lose         UMETA( DisplayName = "Lose" ),
	Win        UMETA( DisplayName = "Win" )
};

DECLARE_LOG_CATEGORY_EXTERN( LogPingPongGameMode, Log, All );

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

	// ~ Countdown Timer
public:
	// Start Countdown timer from start
	UFUNCTION( BlueprintCallable, Category = "Countdown Timer" )
	void StartOverCountdownTimer(const bool& NeedToStartGameOnTimerEnd = true);
protected:
	// Create timer in viewport
	UFUNCTION( BlueprintCallable, Category = "Countdown Timer" )
	void CreateCountdownTimer( const bool& NeedToStartGameOnTimerEnd = true );
	// Delete timer from viewport if not deleted yet
	UFUNCTION( BlueprintCallable, Category = "Countdown Timer" )
	void DeleteTimer();
	// ~ Countdown Timer

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
	// ~ Variables

	// ~ Status
protected:
	UPROPERTY( BlueprintReadwrite, Category = "Status" )
	bool IsGameStarted = false;
public:
	// Update game save file
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SaveGame();
	// Enable all movements for player, ball and enemy
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void EnableAllMovements();
	// Disable all movements for player, ball and enemy
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void DisableAllMovements();
	// Start ping pong game
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void StartGame();
	// Handle if player press go back(for example, returning to the game from a pause)
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void HandleGoBackButton();
	// Open main menu
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void OpenMainMenu();
	// Sets new Game status and handle it
	UFUNCTION( BlueprintCallable, Category = "Status" )
	bool ChangeGameStatus( const TEnumAsByte<enum GameStatus> NewGameStatus );
	// Restarts current round
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void RestartRound();
	// Restarts current game
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void RestartGame();
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
	// Finds start Transform in level that has type StartType and 
	// Takes in argument pawn that need to be fit in start position
	template<typename StartType>
	TOptional<FTransform> FindStartTransform( TSubclassOf<AActor> ActorClassToFit ) const;
	// Finds player start Transform
	TOptional<FTransform> FindPlayerStartTransform() const;
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

	// ~ Audio
	// ~ ~ Functions
public:
	// Init all audio systems
	void InitAudio();
	// Change current effects volume
	UFUNCTION( BlueprintCallable, Category = "Audio|Volume" )
	void SetEffectsVolume( const float& NewVolume, bool SaveSettings = false );
	// Get current effects volume
	UFUNCTION( BlueprintCallable, Category = "Audio|Volume" )
	float GetEffectsVolume() const;
	// Play certain effect sound
	UFUNCTION( BlueprintCallable, Category = "Audio|Effects" )
	void PlayEffectSound( const TEnumAsByte<PingPongSound>& SoundToPlay );
	// ~ ~ Functions
	// ~ ~ Variables
protected:
	UPROPERTY( EditDefaultsOnly, Category = "Audio|Sound" )
	TSoftObjectPtr<class USoundClass> EffectsSoundClass;
	UPROPERTY( EditDefaultsOnly, Category = "Audio|Sound" )
	TSoftObjectPtr<class USoundMix> EffectsSoundMixClass;
	UPROPERTY( EditDefaultsOnly, Category = "Audio|Sound" )
	class USoundBase* BallBounceSound;
	UPROPERTY( EditDefaultsOnly, Category = "Audio|Sound" )
	class USoundBase* EnemyScoredSound;
	UPROPERTY( EditDefaultsOnly, Category = "Audio|Sound" )
	class USoundBase* PlayerScoredSound;
	UPROPERTY( EditDefaultsOnly, Category = "Audio|Sound" )
	class USoundBase* LoseSound;
	UPROPERTY( EditDefaultsOnly, Category = "Audio|Sound" )
	class USoundBase* WinSound;
	// ~ ~ Variables
	// ~ Audio

	// ~ Widgets
	// ~ ~ Functions
	// ~ ~ ~ Hide
protected:
	// Get current game status and hide all widget from this game status
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Hide" )
	void HideCurrentGameStatusScreen();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Hide" )
	bool HideGameOverScreen();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Hide" )
	bool HideStartingCountdown();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Hide" )
	bool HidePauseScreen();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Hide" )
	bool HideOptionsScreen();
	// ~ ~ ~ Hide
	// ~ ~ ~ Show
protected:
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Show" )
	bool ShowGameOverScreen();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Show" )
	bool ShowStartingCountdown();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Show" )
	bool ShowPauseScreen();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|Show" )
	bool ShowOptionsScreen();
	// ~ ~ ~ Show
	// ~ ~ ~ IsAvialible
protected:
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|IsAvialible" )
	bool IsAvialibleGameOverScreen();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|IsAvialible" )
	bool IsAvialibleStartingCountdown();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|IsAvialible" )
	bool IsAvialiblePauseScreen();
	UFUNCTION( BlueprintCallable, Category = "Widgets|Status|IsAvialible" )
	bool IsAvialibleOptionsScreen();
	// ~ ~ ~ IsAvialible
	// ~ ~ ~ Misc
protected:
	// Init widget on screen and make it hidden
	template<typename WidgetType UE_REQUIRES( TIsDerivedFrom<WidgetType, class UUserWidget>::Value )>
	bool InitWidgetInstance( const TSubclassOf<WidgetType> Widget, TObjectPtr<WidgetType>& WidgetInstance );
	// ~ ~ ~ Misc
	// ~ ~ Functions
	// ~ ~ Variables
protected:
	// Widget that implements score on screen
	UPROPERTY( EditDefaultsOnly, Category = "Widgets" )
	TSubclassOf<class UScoreScreen> ScoreScreenWidget;
	// Instance of score screen widget
	TObjectPtr<class UScoreScreen> ScoreScreenWidgetInstance;
	// Widget for pause menu
	UPROPERTY( EditDefaultsOnly, Category = "Widgets" )
	TSubclassOf<UUserWidget> PauseWidget;
	// Instance of pause menu widget
	TObjectPtr<class UUserWidget> PauseWidgetInstance;
	// Widget for start countdown
	UPROPERTY( EditDefaultsOnly, Category = "Widgets" )
	TSubclassOf<class UUserWidget> StartCountdownWidget;
	// Instance of countdown widget
	TObjectPtr<class UUserWidget> StartCountdownWidgetInstance;
	// Widget for game over screen
	UPROPERTY( EditDefaultsOnly, Category = "Widgets" )
	TSubclassOf<class UUserWidget> GameOverWidget;
	// Instance of game over screen widget
	TObjectPtr<class UUserWidget> GameOverWidgetInstance;
	// Widget for options screen
	UPROPERTY( EditDefaultsOnly, Category = "Widgets" )
	TSubclassOf<class UUserWidget> OptionsWidget;
	// Instance of options widget
	TObjectPtr<class UUserWidget> OptionsWidgetInstance;
	// ~ ~ Variables
	// ~ Widgets

	// ~ Delegates
	// ~ ~ Functions
protected:
	UFUNCTION()
	void OnBallHitBarrier();
	UFUNCTION()
	void OnPlayerWinRound();
	UFUNCTION()
	void OnEnemyWinRound();
	UFUNCTION()
	void TrySaveGameOnChangedStatus( const TEnumAsByte<GameStatus>& NewGameStatus, const TEnumAsByte<GameStatus>& OldGameStatus );
	// ~ ~ Functions
	// ~ ~ Events
	UPROPERTY( BlueprintAssignable, Category = "Events" )
	FOnGameStatusChanged OnGameStatusChanged;
	// ~ ~ Events
	// ~ Delegates
	

	// ~ Miscellaneous
protected:
	// Pointer to main menu level
	UPROPERTY( EditDefaultsOnly, Category = "Levels" )
	TSoftObjectPtr<class UWorld> MainMenuLevel;
public:
	// Changes controller for pawn
	UFUNCTION( BlueprintCallable, Category = "Miscellaneous" )
	void ChangePawnController( UClass* ControllerType, APawn* Pawn );
	// Changes controller for pawn
	template<typename ControllerType UE_REQUIRES( TIsDerivedFrom<ControllerType, AController>::Value )>
	void ChangePawnController( APawn* Pawn );
	// ~ Miscellaneous
};

