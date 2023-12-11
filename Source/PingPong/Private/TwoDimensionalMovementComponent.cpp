// All rights by IsGris


#include "TwoDimensionalMovementComponent.h"

void UTwoDimensionalMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// Init AfterBeginPlay function
	GetWorld()->OnWorldBeginPlay.AddUObject( this, &UTwoDimensionalMovementComponent::AfterBeginPlay );
}

void UTwoDimensionalMovementComponent::AfterBeginPlay()
{
	if ( AController* OwnerController = Cast<AController>( GetOwner() ) ) // check does owner is controller
	{
		// get owner of the controller
		Owner = OwnerController->GetPawn()->GetRootComponent();
	}
	else
	{
		// get owner
		Owner = GetOwner()->GetRootComponent();
	}
}

inline bool UTwoDimensionalMovementComponent::HaveMovement()
{
	return CurrentSpeedHorizontal != 0 && CurrentSpeedVertical != 0;
}

void UTwoDimensionalMovementComponent::SetHorizontalSpeed( const float& Speed )
{
	if ( Speed == 0 && CurrentSpeedVertical == 0 ) // No speed set for movement
		SetComponentTickEnabled( false );
	CurrentSpeedHorizontal = Speed;
	SpeedHorizontal = Speed;
}

void UTwoDimensionalMovementComponent::SetVerticalSpeed( const float& Speed )
{
	if ( Speed == 0 && CurrentSpeedHorizontal == 0 ) // No speed set for movement
		SetComponentTickEnabled( false );
	CurrentSpeedVertical = Speed;
	SpeedVertical = Speed;
}

void UTwoDimensionalMovementComponent::SetSpeed( const float& Speed )
{
	if ( Speed == 0 ) // No speed set for movement
		SetComponentTickEnabled( false );
	CurrentSpeedHorizontal = Speed;
	SpeedHorizontal = Speed;
	CurrentSpeedVertical = Speed;
	SpeedVertical = Speed;
}

void UTwoDimensionalMovementComponent::StartMovingUp( const float& Speed )
{
	SetComponentTickEnabled( true );
	if ( Speed == 0 && SpeedVertical != 0 ) // Set speed from class property
		CurrentSpeedVertical = SpeedVertical;
	else if ( Speed == 0 && !HaveMovement() ) // No speed set for movement
		SetComponentTickEnabled( false );
	else // Set speed from argument
		CurrentSpeedVertical = Speed;
}

void UTwoDimensionalMovementComponent::StartMovingDown( const float& Speed )
{
	SetComponentTickEnabled( true );
	if ( Speed == 0 && SpeedVertical != 0 ) // Set speed from class property
		CurrentSpeedVertical = -SpeedVertical;
	else if ( Speed == 0 && !HaveMovement() ) // No speed set for movement
		SetComponentTickEnabled( false );
	else // Set speed from argument
		CurrentSpeedVertical = -Speed;
}

void UTwoDimensionalMovementComponent::StartMovingLeft( const float& Speed )
{
	SetComponentTickEnabled( true );
	if ( Speed == 0 && SpeedHorizontal != 0 ) // Set speed from class property
		CurrentSpeedHorizontal = -SpeedHorizontal;
	else if ( Speed == 0 && !HaveMovement() ) // No speed set for movement
		SetComponentTickEnabled( false );
	else // Set speed from argument
		CurrentSpeedHorizontal = -Speed;
}

void UTwoDimensionalMovementComponent::StartMovingRight( const float& Speed )
{
	SetComponentTickEnabled( true );
	if ( Speed == 0 && SpeedHorizontal != 0 ) // Set speed from class property
		CurrentSpeedHorizontal = SpeedHorizontal;
	else if ( Speed == 0 && !HaveMovement() ) // No speed set for movement
		SetComponentTickEnabled( false );
	else // Set speed from argument
		CurrentSpeedHorizontal = Speed;
}

void UTwoDimensionalMovementComponent::StopHorizontalMovement()
{
	CurrentSpeedHorizontal = 0;
	if ( !HaveMovement() )
		SetComponentTickEnabled( false );

}

void UTwoDimensionalMovementComponent::StopVerticalMovement()
{
	CurrentSpeedVertical = 0;
	if ( !HaveMovement() )
		SetComponentTickEnabled( false );
}

void UTwoDimensionalMovementComponent::StopMovement()
{
	CurrentSpeedVertical = 0;
	CurrentSpeedHorizontal = 0;
	SetComponentTickEnabled( false );
}

void UTwoDimensionalMovementComponent::MoveUp( const float& Value )
{
	Move( FVector2f( 0, Value ) );
}

void UTwoDimensionalMovementComponent::MoveDown( const float& Value )
{
	Move( FVector2f( 0, -Value ) );
}

void UTwoDimensionalMovementComponent::MoveRight( const float& Value )
{
	Move( FVector2f( Value, 0 ) );
}

void UTwoDimensionalMovementComponent::MoveLeft( const float& Value )
{
	Move( FVector2f( -Value, 0 ) );
}

void UTwoDimensionalMovementComponent::Move( const FVector2f& Value )
{
	Move( FVector( 0, Value.X, Value.Y ) );
}

void UTwoDimensionalMovementComponent::Move( const FVector& Value )
{
	Owner->AddRelativeLocation( Value );
}

void UTwoDimensionalMovementComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Move( FVector2f( CurrentSpeedHorizontal, CurrentSpeedVertical ) * DeltaTime );
}
