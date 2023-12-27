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

	// Init widgets
	InitWidgetInstance( ScoreScreenWidget, ScoreScreenWidgetInstance );
	InitWidgetInstance( PauseWidget, PauseWidgetInstance );
	InitWidgetInstance( GameOverWidget, GameOverWidgetInstance );
	InitWidgetInstance( OptionsWidget, OptionsWidgetInstance );
	CreateCountdownTimer();
	if ( APlayerController* PlayerController = Cast<APlayerController>( Player->GetController() ) )
		PlayerController->SetInputMode( FInputModeGameAndUI() );

	// Init settings
	SetEffectsVolume( GetEffectsVolume() ); // Get default volume and set it
}

void APingPongGameMode::StartOverCountdownTimer( const bool& NeedToStartGameOnTimerEnd )
{
	DeleteTimer();
	CreateCountdownTimer( NeedToStartGameOnTimerEnd );
}

void APingPongGameMode::CreateCountdownTimer( const bool& NeedToStartGameOnTimerEnd )
{
	if ( StartCountdownWidget )
	{
		StartCountdownWidgetInstance = CreateWidget<UUserWidget>( GetWorld(), StartCountdownWidget );
		if ( StartCountdownWidgetInstance )
		{
			StartCountdownWidgetInstance->AddToViewport();
			if ( NeedToStartGameOnTimerEnd )
			{
				DisableAllMovements();
			}
		}
		else
			UE_LOG( LogPingPongGameMode, Warning, TEXT( "Error when tries to create StartCountdownWidgetInstance" ) );
	}
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "StartCountdown widget is not defined" ) );
}

void APingPongGameMode::DeleteTimer()
{
	if ( StartCountdownWidgetInstance && StartCountdownWidgetInstance->IsConstructed() )
	{
		StartCountdownWidgetInstance->RemoveFromParent();
		StartCountdownWidgetInstance->Destruct();
	}
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
		case GameOver:
			OpenMainMenu();
			break;
	}
}

void APingPongGameMode::OpenMainMenu()
{
	UGameplayStatics::OpenLevel( GetWorld(), FName(*MainMenuLevel.GetAssetName()));
}

void APingPongGameMode::EnableAllMovements()
{
	Ball->CanMove = true;
	Player->SetAvialibleMoveDirection( MoveDirection::UpAndDown );
	Enemy->SetAvialibleMoveDirection( MoveDirection::UpAndDown );
}

void APingPongGameMode::DisableAllMovements()
{
	Ball->CanMove = false;
	Player->SetAvialibleMoveDirection( MoveDirection::NoMovement );
	Enemy->SetAvialibleMoveDirection( MoveDirection::NoMovement );
}

void APingPongGameMode::StartGame()
{
	// Change game status and unpause
	APingPongGameState* CurrentGameState = Cast<APingPongGameState>( GameState );
	CurrentGameState->CurrentGameStatus = Playing;
	IsGameStarted = true;
	EnableAllMovements();
	DeleteTimer();
}

bool APingPongGameMode::ChangeGameStatus( const TEnumAsByte<GameStatus> NewGameStatus )
{
	APingPongGameState* CurrentGameState = Cast<APingPongGameState>( GameState ); // Get game state
	if ( !CurrentGameState )
		return false;

	TEnumAsByte<GameStatus>& CurrentGameStatus = CurrentGameState->CurrentGameStatus;

	if ( NewGameStatus == CurrentGameStatus )
		return true;

	switch ( NewGameStatus ) // Change current game status
	{
	case GameOver:
		if ( CurrentGameStatus != Playing || !IsAvialibleGameOverScreen() )
			return false;
		HideCurrentGameStatusScreen();
		ShowGameOverScreen();
		CurrentGameStatus = GameOver;
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
	return true;
}

void APingPongGameMode::RestartRound()
{
	StartOverCountdownTimer( true );
	
	// Set all actor locations to start
	TOptional<FTransform> TempTransform;
	TempTransform = FindPlayerStartTransform();
	if ( TempTransform.IsSet() && Player )
		Player->SetActorLocationAndRotation( TempTransform.GetValue().GetLocation(), TempTransform.GetValue().GetRotation() );
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "Failed to reset player position in restart round" ) );
	TempTransform = FindEnemyStartTransform();
	if ( TempTransform.IsSet() && Enemy )
		Enemy->SetActorLocationAndRotation( TempTransform.GetValue().GetLocation(), TempTransform.GetValue().GetRotation() );
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "Failed to reset enemy position in restart round" ) );
	TempTransform = FindBallStartTransform();
	if ( TempTransform.IsSet() && Ball )
		Ball->SetActorLocationAndRotation( TempTransform.GetValue().GetLocation(), TempTransform.GetValue().GetRotation() );
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "Failed to reset ball position in restart round" ) );

	if ( Ball )
		Ball->ResetDirection();
	
}

void APingPongGameMode::RestartGame()
{
	UGameplayStatics::OpenLevel( GetWorld(), FName( *GetWorld()->GetMapName() ) ); // Reopen current level and then all starts from beginning
}

void APingPongGameMode::AddPlayerScore( const int& ScoreToAdd )
{
	Player->AddScore( ScoreToAdd );
	if ( ScoreScreenWidgetInstance )
		ScoreScreenWidgetInstance->ChangePlayerScore( Player->GetScore() );
	RestartRound();
}

void APingPongGameMode::AddEnemyScore( const int& ScoreToAdd )
{
	Enemy->AddScore( ScoreToAdd );
	if ( ScoreScreenWidgetInstance )
		ScoreScreenWidgetInstance->ChangeEnemyScore( Enemy->GetScore() );
	RestartRound();
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
}

void APingPongGameMode::SetEffectsVolume( const float& NewVolume )
{
	auto PingPongGameInstance = Cast<UPingPongGameInstance>( UGameplayStatics::GetGameInstance( GetWorld() ) );
	auto WorldContext = GetWorld();
	if ( PingPongGameInstance &&
		 WorldContext &&
		 !EffectsSoundMixClass.IsNull() &&
		 !EffectsSoundClass.IsNull()
		)
	{
		PingPongGameInstance->SetEffectsVolume( NewVolume );
		UGameplayStatics::SetSoundMixClassOverride( WorldContext, EffectsSoundMixClass.Get(), EffectsSoundClass.Get(), NewVolume, 1.0f, 0.0f );
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
	if ( APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		PlayerControllerRef->SetShowMouseCursor( false );
	else
		return false;
	GameOverWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
	UGameplayStatics::SetGamePaused( GetWorld(), false );
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
	if ( APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		PlayerControllerRef->SetShowMouseCursor( false );
	else
		return false;
	PauseWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
	UGameplayStatics::SetGamePaused( GetWorld(), false );
	return true;
}

bool APingPongGameMode::HideOptionsScreen()
{
	if ( !OptionsWidgetInstance )
		return false;
	if ( APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		PlayerControllerRef->SetShowMouseCursor( false );
	else
		return false;
	OptionsWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
	UGameplayStatics::SetGamePaused( GetWorld(), false );
	return true;
}

bool APingPongGameMode::ShowGameOverScreen()
{
	if ( !GameOverWidgetInstance )
		return false;
	if ( APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		PlayerControllerRef->SetShowMouseCursor( true );
	else
		return false;
	GameOverWidgetInstance->SetVisibility( ESlateVisibility::Visible );
	UGameplayStatics::SetGamePaused( GetWorld(), true );
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
	if ( APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		PlayerControllerRef->SetShowMouseCursor( true );
	else
		return false;
	PauseWidgetInstance->SetVisibility( ESlateVisibility::Visible );
	UGameplayStatics::SetGamePaused( GetWorld(), true );
	return true;
}

bool APingPongGameMode::ShowOptionsScreen()
{
	if ( !OptionsWidgetInstance )
		return false;
	if ( APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
		PlayerControllerRef->SetShowMouseCursor( true );
	else
		return false;
	OptionsWidgetInstance->SetVisibility( ESlateVisibility::Visible );
	UGameplayStatics::SetGamePaused( GetWorld(), true );
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

template<typename WidgetType UE_REQUIRES( TIsDerivedFrom<WidgetType, class UUserWidget>::Value )>
bool APingPongGameMode::InitWidgetInstance( const TSubclassOf<WidgetType> Widget, TObjectPtr<WidgetType>& WidgetInstance )
{
	if ( Widget )
	{
		WidgetInstance = CreateWidget<WidgetType>( GetWorld(), Widget );
		if ( WidgetInstance )
		{
			WidgetInstance->AddToViewport();
			WidgetInstance->SetVisibility( ESlateVisibility::Hidden );
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

}

template<typename ControllerType UE_REQUIRES( TIsDerivedFrom<ControllerType, AController>::Value )>
inline void APingPongGameMode::ChangePawnController( APawn* Pawn )
{
	ChangePawnController( ControllerType::StaticClass(), Pawn );
}
