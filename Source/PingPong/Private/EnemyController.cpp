// All rights by IsGris


#include "EnemyController.h"
#include "Ball.h"
#include "PingPongGameMode.h"
#include "TwoDimensionalMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBarrier.h"

AEnemyController::AEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;
	MovementComponent = CreateDefaultSubobject<UTwoDimensionalMovementComponent>( "2DMovementComponent" );
}

void AEnemyController::PerformMove( const float& MaxMoveDistance )
{
	if ( APingPongGameMode* GameMode = Cast<APingPongGameMode>( UGameplayStatics::GetGameMode( GetWorld() ) ) )
		// calculates where the enemy should move and how many units without going beyond the boundaries of MaxMoveDistance and makes move
		MoveUp( FMath::Clamp<float>( GameMode->GetBall()->GetActorLocation().Z - GetPawn()->GetRootComponent()->GetComponentLocation().Z, -MaxMoveDistance, MaxMoveDistance ) );
}

void AEnemyController::MoveUp( const float& Value )
{
	MovementComponent->MoveUp( Value );
}

void AEnemyController::MoveDown( const float& Value )
{
	MovementComponent->MoveDown( Value );
}

void AEnemyController::MoveLeft( const float& Value )
{
	MovementComponent->MoveLeft( Value );
}

void AEnemyController::MoveRight( const float& Value )
{
	MovementComponent->MoveRight( Value );
}

void AEnemyController::Move( const FVector2f& Value )
{
	MovementComponent->Move( Value );
}

void AEnemyController::Tick( float DeltaTime )
{
	if ( auto CurrentOwner = Cast<AEnemyBarrier>( GetPawn() ) )
		if ( CurrentOwner->CanMove )
			PerformMove( CurrentOwner->Speed * DeltaTime );
}
