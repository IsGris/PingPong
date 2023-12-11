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
#include "EnemyStart.h"
#include "BallStart.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY( LogPingPongGameMod );

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
	
	TempTransform.Reset();
	TempTransform = FindBallStartTransform();
	if ( TempTransform.IsSet() )
		SpawnBall( TempTransform.GetValue() );

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
		UE_LOG( LogPingPongGameMod, Warning, TEXT( "PauseWidget is not defined" ) );

	// Init start with start countdown widget
	if ( StartCountdownWidget )
	{
		StartCountdownWidgetInstance = CreateWidget<UUserWidget>( GetWorld(), StartCountdownWidget );
		if ( StartCountdownWidgetInstance )
		{
			StartCountdownWidgetInstance->AddToViewport();
			GetWorldTimerManager().SetTimer( StartCountdownTimer, this, &APingPongGameMode::StartGame, StartCountdownDuration );
			DisableAllMovements();
		} else
			UE_LOG( LogPingPongGameMod, Warning, TEXT( "Error when tries to create StartCountdownWidgetInstance" ) );
	}
	else
		UE_LOG( LogPingPongGameMod, Warning, TEXT( "PauseWidget is not defined" ) );
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
	Player->CanMove = true;
	Enemy->CanMove = true;
}

void APingPongGameMode::DisableAllMovements()
{
	Ball->CanMove = false;
	Player->CanMove = false;
	Enemy->CanMove = false;
}

void APingPongGameMode::StartGame()
{
	// Change game status and unpause
	APingPongGameState* CurrentGameState = Cast<APingPongGameState>( GameState );
	CurrentGameState->CurrentGameStatus = Playing;
	EnableAllMovements();
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

void APingPongGameMode::AddPlayerScore( const int& ScoreToAdd )
{
	Player->AddScore( ScoreToAdd );
}

void APingPongGameMode::AddEnemyScore( const int& ScoreToAdd )
{
	Enemy->AddScore( ScoreToAdd );
}

TOptional<FTransform> APingPongGameMode::FindEnemyStartTransform() const
{
	AEnemyStart* FoundEnemyStart = nullptr; // Init variables for function
	APawn* PawnToFit = DefaultEnemyClass->GetDefaultObject<APawn>();
	TArray<AEnemyStart*> UnOccupiedStartPoints;
	TArray<AEnemyStart*> OccupiedStartPoints;
	UWorld* World = GetWorld();
	for ( TActorIterator<AEnemyStart> It( World ); It; ++It ) // Iterate all enemy starts in world
	{
		AEnemyStart* EnemyStart = *It;

		FVector ActorLocation = EnemyStart->GetActorLocation();
		const FRotator ActorRotation = EnemyStart->GetActorRotation();
		if ( !World->EncroachingBlockingGeometry( PawnToFit, ActorLocation, ActorRotation ) ) // Check does enemy start not blocked by something
		{
			UnOccupiedStartPoints.Add( EnemyStart );
		}
		else if ( World->FindTeleportSpot( PawnToFit, ActorLocation, ActorRotation ) )
		{
			OccupiedStartPoints.Add( EnemyStart );
		}
	}
	if ( FoundEnemyStart == nullptr )
	{
		if ( UnOccupiedStartPoints.Num() > 0 )
		{
			FoundEnemyStart = UnOccupiedStartPoints[FMath::RandRange( 0, UnOccupiedStartPoints.Num() - 1 )];
		}
		else if ( OccupiedStartPoints.Num() > 0 )
		{
			FoundEnemyStart = OccupiedStartPoints[FMath::RandRange( 0, OccupiedStartPoints.Num() - 1 )];
		}
	}

	if ( FoundEnemyStart != nullptr )
		return FTransform( FoundEnemyStart->GetActorRotation(), FoundEnemyStart->GetActorLocation(), FoundEnemyStart->GetActorScale() );
	else
		return TOptional<FTransform>();
}

TOptional<FTransform> APingPongGameMode::FindBallStartTransform() const
{
	ABallStart* FoundBallStart = nullptr; // Init variables for function
	APawn* PawnToFit = DefaultBallClass->GetDefaultObject<APawn>();
	TArray<ABallStart*> UnOccupiedStartPoints;
	TArray<ABallStart*> OccupiedStartPoints;
	UWorld* World = GetWorld();
	for ( TActorIterator<ABallStart> It( World ); It; ++It ) // Iterate all enemy starts in world
	{
		ABallStart* BallStart = *It;

		FVector ActorLocation = BallStart->GetActorLocation();
		const FRotator ActorRotation = BallStart->GetActorRotation();
		if ( !World->EncroachingBlockingGeometry( PawnToFit, ActorLocation, ActorRotation ) ) // Check does enemy start not blocked by something
		{
			UnOccupiedStartPoints.Add( BallStart );
		}
		else if ( World->FindTeleportSpot( PawnToFit, ActorLocation, ActorRotation ) )
		{
			OccupiedStartPoints.Add( BallStart );
		}
	}
	if ( FoundBallStart == nullptr )
	{
		if ( UnOccupiedStartPoints.Num() > 0 )
		{
			FoundBallStart = UnOccupiedStartPoints[FMath::RandRange( 0, UnOccupiedStartPoints.Num() - 1 )];
		}
		else if ( OccupiedStartPoints.Num() > 0 )
		{
			FoundBallStart = OccupiedStartPoints[FMath::RandRange( 0, OccupiedStartPoints.Num() - 1 )];
		}
	}

	if ( FoundBallStart != nullptr )
		return FTransform( FoundBallStart->GetActorRotation(), FoundBallStart->GetActorLocation(), FoundBallStart->GetActorScale() );
	else
		return TOptional<FTransform>();
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
