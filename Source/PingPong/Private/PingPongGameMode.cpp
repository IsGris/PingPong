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

	ChangePawnController( DefaultEnemyControllerClass, Cast<APawn>(Enemy) );
	ChangePawnController( DefaultBallControllerClass, Cast<APawn>( Ball) );

	// Init pause widget
	if ( PauseWidget )
	{
		PauseWidgetInstance = CreateWidget<UUserWidget>( GetWorld(), PauseWidget );
		if ( PauseWidgetInstance )
		{
			PauseWidgetInstance->AddToViewport();
			PauseWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
		}
	}
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "PauseWidget is not defined" ) );

	// Init score screen widget
	if ( ScoreScreenWidget )
	{
		ScoreScreenWidgetInstance = CreateWidget<UScoreScreen>( GetWorld(), ScoreScreenWidget );
		if ( ScoreScreenWidgetInstance )
		{
			ScoreScreenWidgetInstance->AddToViewport();
		}
	}
	else
		UE_LOG( LogPingPongGameMode, Warning, TEXT( "ScoreScreenWidget is not defined" ) );

	// Init start with start countdown widget
	CreateCountdownTimer();
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
				GetWorldTimerManager().SetTimer( StartCountdownTimer, this, &APingPongGameMode::StartGame, StartCountdownDuration );
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
	if ( StartCountdownWidgetInstance )
		StartCountdownWidgetInstance->Destruct();
	if ( StartCountdownTimer.IsValid() )
		StartCountdownTimer.Invalidate();
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
	EnableAllMovements();
	DeleteTimer();
}

void APingPongGameMode::SwapPause()
{
	APingPongGameState* CurrentGameState = Cast<APingPongGameState>( GameState ); // Get game state
	if ( !CurrentGameState )
		return;

	TEnumAsByte<GameStatus>& CurrentGameStatus = CurrentGameState->CurrentGameStatus;

	if ( CurrentGameStatus == GameStatus::Playing ) // Check if playing -> set to pause 
	{
		PauseWidgetInstance->SetVisibility( ESlateVisibility::Visible );
		CurrentGameStatus = GameStatus::Paused;
		UGameplayStatics::SetGamePaused( GetWorld(), true );
	}
	else if ( CurrentGameStatus == GameStatus::Paused ) // Check if paused -> set to playing
	{
		PauseWidgetInstance->SetVisibility( ESlateVisibility::Hidden );
		CurrentGameStatus = GameStatus::Playing;
		UGameplayStatics::SetGamePaused( GetWorld(), false );
	}
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
