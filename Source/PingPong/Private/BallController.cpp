// All rights by IsGris


#include "BallController.h"
#include "TwoDimensionalMovementComponent.h"

ABallController::ABallController() : Super()
{
	// Init component
	MovementComponent = CreateDefaultSubobject<UTwoDimensionalMovementComponent>( TEXT( "2DMovementComponent" ) );
}

void ABallController::MoveUp( const float& Value )
{
	MovementComponent->MoveUp( Value );
}

void ABallController::MoveDown( const float& Value )
{
	MovementComponent->MoveDown( Value );
}

void ABallController::MoveRight( const float& Value )
{
	MovementComponent->MoveRight( Value );
}

void ABallController::MoveLeft( const float& Value )
{
	MovementComponent->MoveLeft( Value );
}

void ABallController::Move( const FVector2f& Value )
{
	MovementComponent->Move( Value );
}
