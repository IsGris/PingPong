// All rights by IsGris


#include "PingPongGameMode.h"
#include "EnemyBarrier.h"
#include "Ball.h"
#include "BallController.h"
#include "EnemyController.h"
#include "Blueprint/UserWidget.h"
#include "PlayerBarrier.h"
#include "PingPongGameState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "EnemyStart.h"
#include "BallStart.h"
#include "EngineUtils.h"
#include "ScoreScreen.h"
#include "PingPongGameState.h"
#include "PingPongGameInstance.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

DEFINE_LOG_CATEGORY( LogPingPongGameMode );

APingPongGameMode::APingPongGameMode() : Super()
{
	// Init classes
	DefaultEnemyClass = AEnemyBarrier::StaticClass();
	DefaultBallClass = ABall::StaticClass();
	DefaultBallControllerClass = ABallController::StaticClass();
	DefaultEnemyControllerClass = AEnemyController::StaticClass();
}

void APingPongGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Spawn Enemy and Ball if start position set
	TOptional<FTransform> TempTransform;
	TempTransform = FindEnemyStartTransform();
	if ( TempTransform.IsSet() )
		SpawnEnemy( TempTransform.GetValue() );
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "EnemyStart transform not finded cannot reset round" ) );
	
	TempTransform.Reset();
	TempTransform = FindBallStartTransform();
	if ( TempTransform.IsSet() )
		SpawnBall( TempTransform.GetValue() );
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "BallStart transform not finded cannot reset round" ) );

	UE_LOG( LogPingPongGameMode, Log, TEXT( "All entities spawned" ) );

	// Init variables
	Player = FindPlayer();
	Enemy = FindEnemy();
	Ball = FindBall();

	ChangePawnController( DefaultEnemyControllerClass, Cast<APawn>( Enemy ) );
	ChangePawnController( DefaultBallControllerClass, Cast<APawn>( Ball ) );

	if ( Ball )
	{
		Ball->OnBarrierHit.AddDynamic( this, &APingPongGameMode::OnBallHitBarrier );
		Ball->OnPlayerGateScored.AddDynamic( this, &APingPongGameMode::OnEnemyWinRound );
		Ball->OnEnemyGateScored.AddDynamic( this, &APingPongGameMode::OnPlayerWinRound );
	}

	DisableAllMovements();

	UE_LOG( LogPingPongGameMode, Log, TEXT( "All entities configured" ) );

	// Init widgets
	InitWidgetInstance( ScoreScreenWidget, ScoreScreenWidgetInstance, true );
	InitWidgetInstance( PauseWidget, PauseWidgetInstance );
	InitWidgetInstance( GameOverWidget, GameOverWidgetInstance );
	InitWidgetInstance( OptionsWidget, OptionsWidgetInstance );
	InitWidgetInstance( WinWidget, WinWidgetInstance );
	CreateCountdownTimer();
	if ( APlayerController* PlayerController = Cast<APlayerController>( Player->GetController() ) )
		PlayerController->SetInputMode( FInputModeGameOnly() );

	UE_LOG( LogPingPongGameMode, Log, TEXT( "All widgets configured" ) );

	// Init settings
	SetEffectsVolume( GetEffectsVolume() ); // Get default volume and set it

	OnGameStatusChanged.AddDynamic( this, &APingPongGameMode::TrySaveGameOnChangedStatus );

	UE_LOG( LogPingPongGameMode, Log, TEXT( "All settings configured" ) );
}

void APingPongGameMode::StartOverCountdownTimer()
{
	CreateCountdownTimer();
}

void APingPongGameMode::CreateCountdownTimer()
{
	if ( StartCountdownWidget )
	{
		StartCountdownWidgetInstance = CreateWidget<UUserWidget>( GetWorld(), StartCountdownWidget );
		if ( StartCountdownWidgetInstance )
		{
			StartCountdownWidgetInstance->AddToViewport();
		}
		else
			StartCountdownWidgetInstance->Construct();
	}
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "StartCountdown widget is not defined" ) );
}

APlayerBarrier* APingPongGameMode::GetPlayer()
{
	return Player;
}

AEnemyBarrier* APingPongGameMode::GetEnemy()
{
	return Enemy;
}

ABall* APingPongGameMode::GetBall()
{
	return Ball;
}

void APingPongGameMode::HandleGoBackButton()
{
	APingPongGameState* CurrentGameState = Cast<APingPongGameState>( GameState ); // Get game state
	if ( !CurrentGameState )
		return;

	switch ( CurrentGameState->CurrentGameStatus )
	{
		case Starting:
		case Playing:
			ChangeGameStatus( GameStatus::Paused );
			break;
		case Paused:
			ChangeGameStatus( ( IsGameStarted ? GameStatus::Playing : GameStatus::Starting ) );
			break;
		case Options:
			ChangeGameStatus( GameStatus::Paused );
			break;
	}

	UE_LOG( LogPingPongGameMode, Verbose, TEXT( "Go back button successfully handled" ) );
}

void APingPongGameMode::SaveGame()
{
	if ( auto PingPongGameInstance = Cast<UPingPongGameInstance>( UGameplayStatics::GetGameInstance( GetWorld() ) ) )
	{
		PingPongGameInstance->SaveGame();
		UE_LOG( LogPingPongGameMode, Log, TEXT( "Game successfully saved" ) );
	}
}

void APingPongGameMode::EnableAllMovements()
{
	Ball->CanMove = true;
	Player->SetAvialibleMoveDirection( MoveDirection::UpAndDown );
	Enemy->SetAvialibleMoveDirection( MoveDirection::UpAndDown );
	UE_LOG( LogPingPongGameMode, VeryVerbose, TEXT( "Movement enabled" ) );
}

void APingPongGameMode::DisableAllMovements()
{
	Ball->CanMove = false;
	Player->SetAvialibleMoveDirection( MoveDirection::NoMovement );
	Enemy->SetAvialibleMoveDirection( MoveDirection::NoMovement );
	UE_LOG( LogPingPongGameMode, VeryVerbose, TEXT( "Movement disabled" ) );
}

void APingPongGameMode::StartGame()
{
	// Change game status and unpause
	APingPongGameState* CurrentGameState = Cast<APingPongGameState>( GameState );
	CurrentGameState->CurrentGameStatus = Playing;
	IsGameStarted = true;
	EnableAllMovements();
	HideStartingCountdown();
	UE_LOG( LogPingPongGameMode, Log, TEXT( "Game started" ) );
}

bool APingPongGameMode::ChangeGameStatus( const TEnumAsByte<GameStatus> NewGameStatus )
{
	APingPongGameState* CurrentGameState = Cast<APingPongGameState>( GameState ); // Get game state
	if ( !CurrentGameState )
		return false;

	TEnumAsByte<GameStatus>& CurrentGameStatus = CurrentGameState->CurrentGameStatus;

	if ( NewGameStatus == CurrentGameStatus )
		return true;

	if ( IsGameStatusUI( NewGameStatus ) )
	{
		if ( APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		{
			PlayerControllerRef->SetShowMouseCursor( true );
			PlayerControllerRef->SetInputMode( FInputModeUIOnly() );
			UGameplayStatics::SetGamePaused( GetWorld(), true );
		}
		else
			return false;
	}
	else
	{
		if ( APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		{
			PlayerControllerRef->SetShowMouseCursor( false );
			PlayerControllerRef->SetInputMode( FInputModeGameOnly() );
			UGameplayStatics::SetGamePaused( GetWorld(), false );
		}
		else
			return false;
	}

	switch ( NewGameStatus ) // Change current game status
	{
	case GameOver:
		if ( CurrentGameStatus != Playing || !IsAvialibleGameOverScreen() )
			return false;
		HideCurrentGameStatusScreen();
		ShowGameOverScreen();
		PlayEffectSound( PingPongSound::Lose );
		CurrentGameStatus = GameOver;
		break;
	case Win:
		if ( CurrentGameStatus != Playing )
			return false;
		HideCurrentGameStatusScreen();
		ShowWinScreen();
		PlayEffectSound( PingPongSound::Win );
		CurrentGameStatus = Win;
		break;
	case Playing:
		if ( !( CurrentGameStatus == Starting || CurrentGameStatus == Paused || CurrentGameStatus == Options ) )
			return false;
		HideCurrentGameStatusScreen();
		CurrentGameStatus = Playing;
		break;
	case Starting:
		if ( !( CurrentGameStatus == Paused || CurrentGameStatus == Options ) || !IsAvialibleStartingCountdown() )
			return false;
		HideCurrentGameStatusScreen();
		ShowStartingCountdown();
		CurrentGameStatus = Starting;
		break;
	case Paused:
		if ( !( CurrentGameStatus == Playing || CurrentGameStatus == Starting || CurrentGameStatus == Options ) || !IsAvialiblePauseScreen() )
			return false;
		HideCurrentGameStatusScreen();
		ShowPauseScreen();
		CurrentGameStatus = Paused;
		break;
	case Options:
		if ( CurrentGameStatus != Paused || !IsAvialibleOptionsScreen() )
			return false;
		HideCurrentGameStatusScreen();
		ShowOptionsScreen();
		CurrentGameStatus = Options;
		break;
	default: 
		{
			auto NewGameStatusAsString = UEnum::GetValueAsString<GameStatus>( NewGameStatus );
			UE_LOG( LogPingPongGameMode, Warning, 
					TEXT( "Cannot change game status to %s in ChangeGameStatus because the logic of the function does not take this game status into account" ),
					*NewGameStatusAsString );
			return false;
		}
		break;
	}

	OnGameStatusChanged.Broadcast( NewGameStatus, CurrentGameStatus );

	UE_LOG( LogPingPongGameMode, Log, TEXT( "Game status changed" ) );
	
	return true;
}

void APingPongGameMode::RestartRound()
{
	IsGameStarted = false;

	Cast<APingPongGameState>( GameState )->CurrentGameStatus = Starting;

	StartOverCountdownTimer();
	
	// Set all actor locations to start
	TOptional<FTransform> TempTransform;
	TempTransform = FindPlayerStartTransform();
	if ( TempTransform.IsSet() && Player )
	{
		Player->SetActorLocationAndRotation( TempTransform.GetValue().GetLocation(), TempTransform.GetValue().GetRotation() );
		Player->SetAvialibleMoveDirection( MoveDirection::NoMovement );
	}
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "Failed to reset player position in restart round" ) );
	TempTransform = FindEnemyStartTransform();
	if ( TempTransform.IsSet() && Enemy )
	{
		Enemy->SetActorLocationAndRotation( TempTransform.GetValue().GetLocation(), TempTransform.GetValue().GetRotation() );
		Enemy->SetAvialibleMoveDirection( MoveDirection::NoMovement );
	}
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "Failed to reset enemy position in restart round" ) );
	TempTransform = FindBallStartTransform();
	if ( TempTransform.IsSet() && Ball )
		Ball->SetActorLocationAndRotation( TempTransform.GetValue().GetLocation(), TempTransform.GetValue().GetRotation() );
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "Failed to reset ball position in restart round" ) );

	if ( Ball )
		Ball->ResetDirection();

	DisableAllMovements();

	UE_LOG( LogPingPongGameMode, Log, TEXT( "Round restarted" ) );

}

void APingPongGameMode::RestartGame()
{
	FString CurrentLevelName = GetWorld()->GetMapName();
	CurrentLevelName.RemoveFromStart( GetWorld()->StreamingLevelsPrefix );
	UGameplayStatics::OpenLevel( GetWorld(), FName( CurrentLevelName ) ); // Reopen current level and then all starts from beginning
}

void APingPongGameMode::AddPlayerScore( const int& ScoreToAdd )
{
	Player->AddScore( ScoreToAdd );
	if ( ScoreScreenWidgetInstance )
		ScoreScreenWidgetInstance->ChangePlayerScore( Player->GetScore() );
	if ( Player->GetScore() >= ScoreToWin )
		ChangeGameStatus( Win );
	else
		RestartRound();
	UE_LOG( LogPingPongGameMode, VeryVerbose, TEXT( "Added player score" ) );
}

void APingPongGameMode::AddEnemyScore( const int& ScoreToAdd )
{
	Enemy->AddScore( ScoreToAdd );
	if ( ScoreScreenWidgetInstance )
		ScoreScreenWidgetInstance->ChangeEnemyScore( Enemy->GetScore() );
	if ( Enemy->GetScore() >= ScoreToWin )
		ChangeGameStatus( GameOver );
	else
		RestartRound();
	UE_LOG( LogPingPongGameMode, VeryVerbose, TEXT( "Added enemy score" ) );
}

template<typename StartType>
TOptional<FTransform> APingPongGameMode::FindStartTransform( TSubclassOf<AActor> ActorClassToFit ) const
{
	StartType* FoundStart = nullptr; // Init variables for function
	AActor* ActorToFit = ActorClassToFit->GetDefaultObject<AActor>();
	TArray<StartType*> UnOccupiedStartPoints;
	TArray<StartType*> OccupiedStartPoints;
	UWorld* World = GetWorld();
	for ( TActorIterator<StartType> It( World ); It; ++It ) // Iterate all enemy starts in world
	{
		StartType* CurrentStart = *It;

		FVector ActorLocation = CurrentStart->GetActorLocation();
		const FRotator ActorRotation = CurrentStart->GetActorRotation();
		if ( !World->EncroachingBlockingGeometry( ActorToFit, ActorLocation, ActorRotation ) ) // Check does start not blocked by something
		{
			UnOccupiedStartPoints.Add( CurrentStart );
		}
		else if ( World->FindTeleportSpot( ActorToFit, ActorLocation, ActorRotation ) )
		{
			OccupiedStartPoints.Add( CurrentStart );
		}
	}
	if ( FoundStart == nullptr )
	{
		if ( UnOccupiedStartPoints.Num() > 0 )
		{
			FoundStart = UnOccupiedStartPoints[FMath::RandRange( 0, UnOccupiedStartPoints.Num() - 1 )];
		}
		else if ( OccupiedStartPoints.Num() > 0 )
		{
			FoundStart = OccupiedStartPoints[FMath::RandRange( 0, OccupiedStartPoints.Num() - 1 )];
		}
	}

	if ( FoundStart != nullptr )
		return FTransform( FoundStart->GetActorRotation(), FoundStart->GetActorLocation(), FoundStart->GetActorScale() );
	else
		return TOptional<FTransform>();
}

TOptional<FTransform> APingPongGameMode::FindPlayerStartTransform() const
{
	return FindStartTransform<APlayerStart>( DefaultPawnClass );
}

TOptional<FTransform> APingPongGameMode::FindEnemyStartTransform() const
{
	return FindStartTransform<AEnemyStart>( DefaultEnemyClass );
}

TOptional<FTransform> APingPongGameMode::FindBallStartTransform() const
{
	return FindStartTransform<ABallStart>( DefaultBallClass );
}

void APingPongGameMode::SpawnEnemy( const FTransform& ActorTransform )
{
	GetWorld()->SpawnActor( DefaultEnemyClass, &ActorTransform );
}

void APingPongGameMode::SpawnBall( const FTransform& ActorTransform )
{
	GetWorld()->SpawnActor( DefaultBallClass, &ActorTransform );
}

APlayerBarrier* APingPongGameMode::FindPlayer() const
{
	return FindActorWithType<APlayerBarrier>();
}

AEnemyBarrier* APingPongGameMode::FindEnemy() const
{
	return FindActorWithType<AEnemyBarrier>();
}

ABall* APingPongGameMode::FindBall() const
{
	return FindActorWithType<ABall>();
}

template<typename ActorType UE_REQUIRES( TIsDerivedFrom<ActorType, AActor>::Value )>
inline ActorType* APingPongGameMode::FindActorWithType() const
{
	return Cast<ActorType>( UGameplayStatics::GetActorOfClass( GetWorld(), ActorType::StaticClass() ) );
}

AActor* APingPongGameMode::FindActorWithType( UClass* ActorType ) const
{
	return UGameplayStatics::GetActorOfClass( GetWorld(), ActorType );
}

void APingPongGameMode::InitAudio()
{
	UGameplayStatics::PushSoundMixModifier( GetWorld(), EffectsSoundMixClass.Get() );
	UE_LOG( LogPingPongGameMode, Log, TEXT( "Audio inited" ) );
}

void APingPongGameMode::SetEffectsVolume( const float& NewVolume, bool SaveSettings )
{
	auto PingPongGameInstance = Cast<UPingPongGameInstance>( UGameplayStatics::GetGameInstance( GetWorld() ) );
	auto WorldContext = GetWorld();
	if ( PingPongGameInstance &&
		 WorldContext &&
		 !EffectsSoundMixClass.IsNull() &&
		 !EffectsSoundClass.IsNull()
		)
	{
		PingPongGameInstance->SetEffectsVolume( NewVolume, SaveSettings );
		EffectsSoundClass->Properties.Volume = NewVolume;
		UE_LOG( LogPingPongGameMode, VeryVerbose, TEXT( "Effects volume changed" ) );
	}
}

float APingPongGameMode::GetEffectsVolume() const
{
	if ( auto PingPongGameInstance = Cast<UPingPongGameInstance>( UGameplayStatics::GetGameInstance( GetWorld() ) ) )
		return PingPongGameInstance->GetEffectsVolume();
	else
		return 1.0f;
}

void APingPongGameMode::PlayEffectSound( const TEnumAsByte<PingPongSound>& SoundToPlay )
{
	switch ( SoundToPlay.GetValue() )
	{
		case PingPongSound::BallBounce:
			UGameplayStatics::PlaySound2D( this, BallBounceSound );
			break;
		case PingPongSound::EnemyScored:
			UGameplayStatics::PlaySound2D( this, EnemyScoredSound );
			break;
		case PingPongSound::PlayerScored:
			UGameplayStatics::PlaySound2D( this, PlayerScoredSound );
			break;
		case PingPongSound::Lose:
			UGameplayStatics::PlaySound2D( this, LoseSound );
			break;
		case PingPongSound::Win:
			UGameplayStatics::PlaySound2D( this, WinSound );
			break;
	}
}

void APingPongGameMode::HideCurrentGameStatusScreen()
{
	APingPongGameState* CurrentGameState = Cast<APingPongGameState>( GameState ); // Get game state
	if ( !CurrentGameState )
		return;

	switch ( CurrentGameState->CurrentGameStatus )
	{
		case GameOver:
			HideGameOverScreen();
			break;
		case Win:
			HideWinScreen();
			break;
		case Starting:
			HideStartingCountdown();
			break;
		case Paused:
			HidePauseScreen();
			break;
		case Options:
			HideOptionsScreen();
			break;
	}
}

bool APingPongGameMode::HideGameOverScreen()
{
	if ( !GameOverWidgetInstance )
		return false;
	GameOverWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
	return true;
}

bool APingPongGameMode::HideStartingCountdown()
{
	if ( !StartCountdownWidgetInstance )
		return false;
	StartCountdownWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
	return true;
}

bool APingPongGameMode::HidePauseScreen()
{
	if ( !PauseWidgetInstance )
		return false;
	PauseWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
	return true;
}

bool APingPongGameMode::HideOptionsScreen()
{
	if ( !OptionsWidgetInstance )
		return false;
	OptionsWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
	return true;
}

bool APingPongGameMode::HideWinScreen()
{
	if ( !WinWidgetInstance )
		return false;
	WinWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
	return true;
}

bool APingPongGameMode::ShowGameOverScreen()
{
	if ( !GameOverWidgetInstance )
		return false;
	GameOverWidgetInstance->SetVisibility( ESlateVisibility::Visible );
	return true;
}

bool APingPongGameMode::ShowStartingCountdown()
{
	if ( !StartCountdownWidgetInstance )
		return false;
	StartCountdownWidgetInstance->SetVisibility( ESlateVisibility::Visible );
	return true;
}

bool APingPongGameMode::ShowPauseScreen()
{
	if ( !PauseWidgetInstance )
		return false;
	PauseWidgetInstance->SetVisibility( ESlateVisibility::Visible );
	return true;
}

bool APingPongGameMode::ShowOptionsScreen()
{
	if ( !OptionsWidgetInstance )
		return false;
	OptionsWidgetInstance->SetVisibility( ESlateVisibility::Visible );
	return true;
}

bool APingPongGameMode::ShowWinScreen()
{
	if ( !WinWidgetInstance )
		return false;
	WinWidgetInstance->SetVisibility( ESlateVisibility::Visible );
	return true;
}

bool APingPongGameMode::IsAvialibleGameOverScreen()
{
	if ( !GameOverWidgetInstance || !UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		return false;
	else
		return true;
}

bool APingPongGameMode::IsAvialibleStartingCountdown()
{
	if ( !StartCountdownWidgetInstance )
		return false;
	else
		return true;
}

bool APingPongGameMode::IsAvialiblePauseScreen()
{
	if ( !PauseWidgetInstance || !UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		return false;
	else
		return true;
}

bool APingPongGameMode::IsAvialibleOptionsScreen()
{
	if ( !OptionsWidgetInstance || !UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		return false;
	else
		return true;
}

bool APingPongGameMode::IsAvialibleWinScreen()
{
	if ( !WinWidgetInstance || !UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		return false;
	else
		return true;
}

template<typename WidgetType UE_REQUIRES( TIsDerivedFrom<WidgetType, class UUserWidget>::Value )>
bool APingPongGameMode::InitWidgetInstance( const TSubclassOf<WidgetType> Widget, TObjectPtr<WidgetType>& WidgetInstance, const bool& IsVisible )
{
	if ( Widget )
	{
		WidgetInstance = CreateWidget<WidgetType>( GetWorld(), Widget );
		if ( WidgetInstance )
		{
			WidgetInstance->AddToViewport();
			if ( !IsVisible )
				WidgetInstance->SetVisibility( ESlateVisibility::Hidden );
			UE_LOG( LogPingPongGameMode, Verbose, TEXT( "Widget instance %s successfully inited" ), Widget.Get()->GetClass()->GetFName() );
			return true;
		}
		else
		{
			UE_LOG( LogPingPongGameMode, Warning, TEXT( "Cannot create widget instance of widget %s given in function InitWidgetInstance" ), 
					*( Widget->GetClass()->GetFName().ToString() ) );
			return false;
		}
	}
	else
	{
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "Widget given in function InitWidgetInstance is not defined" ) );
		return false;
	}
}

void APingPongGameMode::OnBallHitBarrier()
{
	PlayEffectSound( PingPongSound::BallBounce );
}

void APingPongGameMode::OnPlayerWinRound()
{
	AddPlayerScore();
	PlayEffectSound( PingPongSound::PlayerScored );
}

void APingPongGameMode::OnEnemyWinRound()
{
	AddEnemyScore();
	PlayEffectSound( PingPongSound::EnemyScored );
}

void APingPongGameMode::TrySaveGameOnChangedStatus( const TEnumAsByte<GameStatus>& NewGameStatus, const TEnumAsByte<GameStatus>& OldGameStatus )
{
	if ( NewGameStatus == Paused && OldGameStatus == Options )
		SaveGame();
}

bool APingPongGameMode::IsGameStatusUI( const TEnumAsByte<GameStatus>& GameStatusValue )
{
	return ( GameStatusValue == GameOver || GameStatusValue == Paused || GameStatusValue == Options || GameStatusValue == Win );
}

void APingPongGameMode::ChangePawnController( UClass* ControllerType, APawn* Pawn )
{
	if ( Pawn == nullptr || ControllerType == nullptr )
		return;

	if ( AController* CurrentController = Pawn->GetController() ) // Delete current controller from pawn
	{
		CurrentController->UnPossess();
		CurrentController->Destroy();
	}

	auto NewController = Cast<AController>( GetWorld()->SpawnActor( ControllerType, &Pawn->GetTransform() ) ); // Spawn new controller

	NewController->Possess( Pawn );
	Pawn->Controller = NewController;

	UE_LOG( LogPingPongGameMode, Verbose, TEXT( "Successfully changed controller to %s for pawn %s" ), ControllerType->GetFName(), Pawn->GetFName() );
}

template<typename ControllerType UE_REQUIRES( TIsDerivedFrom<ControllerType, AController>::Value )>
inline void APingPongGameMode::ChangePawnController( APawn* Pawn )
{
	ChangePawnController( ControllerType::StaticClass(), Pawn );
}
