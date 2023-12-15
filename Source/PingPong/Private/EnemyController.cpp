// All rights by IsGris


#include "EnemyController.h"
#include "Ball.h"
#include "PingPongGameMode.h"
#include "TwoDimensionalMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBarrier.h"
#include "MovingBarrier.h"

AEnemyController::AEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;
	MovementComponent = CreateDefaultSubobject<UTwoDimensionalMovementComponent>( "2DMovementComponent" );
}

void AEnemyController::PerformMove( const float& MaxMoveDistance, const MoveDirection& AvialibleDirection = MoveDirection::UpAndDown )
{
	APingPongGameMode* GameMode = Cast<APingPongGameMode>( UGameplayStatics::GetGameMode( GetWorld() ) );

	if ( GameMode == nullptr )
		return;

	// Calculates where the enemy should move and how many units without going beyond the boundaries of MaxMoveDistance and makes move
	auto MoveValue = FMath::Clamp<float>( GameMode->GetBall()->GetActorLocation().Z - GetPawn()->GetRootComponent()->GetComponentLocation().Z, -MaxMoveDistance, MaxMoveDistance );
	// Check does we can move in this direction
	if ( AvialibleDirection == MoveDirection::UpAndDown || ( MoveValue > 0 && AvialibleDirection == MoveDirection::Up ) || ( MoveValue < 0 && AvialibleDirection == MoveDirection::Down ) )
		MoveUp( MoveValue );
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
		if ( CurrentOwner->GetAvialibleMoveDirection() != MoveDirection::NoMovement )
			PerformMove( CurrentOwner->Speed * DeltaTime, CurrentOwner->GetAvialibleMoveDirection() );
}
