// All rights by IsGris


#include "PlayerBarrierController.h"
#include "TwoDimensionalMovementComponent.h"

APlayerBarrierController::APlayerBarrierController()
{
	MovementComponent = CreateDefaultSubobject<UTwoDimensionalMovementComponent>( "2DMovementComponent" );
}

void APlayerBarrierController::MoveUp( const float& Value )
{
	MovementComponent->MoveUp( Value );
}

void APlayerBarrierController::MoveDown( const float& Value )
{
	MovementComponent->MoveDown( Value );
}

void APlayerBarrierController::MoveLeft( const float& Value )
{
	MovementComponent->MoveLeft( Value );
}

void APlayerBarrierController::MoveRight( const float& Value )
{
	MovementComponent->MoveRight( Value );
}

void APlayerBarrierController::Move( const FVector2f& Value )
{
	MovementComponent->Move( Value );
}
