// All rights by IsGris


#include "MovingBarrier.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AMovingBarrier::AMovingBarrier() : Super()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "CollisionBox" ) );
	CollisionBox->SetupAttachment( RootComponent );
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMesh" ) );
	StaticMesh->SetupAttachment( Cast< USceneComponent >( CollisionBox ) );
	CollisionBox->SetGenerateOverlapEvents( true );
	CollisionBox->OnComponentBeginOverlap.AddDynamic( this, &AMovingBarrier::OnBoxBeginOverlap );
	CollisionBox->OnComponentEndOverlap.AddDynamic( this, &AMovingBarrier::OnBoxEndOverlap );
	StaticMesh->SetGenerateOverlapEvents( false );
#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>( TEXT( "Arrow" ) );

	if ( !IsRunningCommandlet() && ArrowComponent )
	{
		ArrowComponent->ArrowColor = FColor( 150, 200, 255 );

		ArrowComponent->ArrowSize = 1.0f;
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SetupAttachment( GetRootComponent() );
		ArrowComponent->bIsScreenSizeScaled = true;
	}

	bIsSpatiallyLoaded = false;
#endif // WITH_EDITORONLY_DATA
	RootComponent = CollisionBox;
}

void AMovingBarrier::SetAvialibleMoveDirection( const MoveDirection& Value )
{
	AvialibleMoveDirection = Value;
}

MoveDirection AMovingBarrier::GetAvialibleMoveDirection() const
{
	return AvialibleMoveDirection;
}

void AMovingBarrier::AddScore( const int& ScoreToAdd )
{
	this->Score += ScoreToAdd;
}

int AMovingBarrier::GetScore() const
{
	return Score;
}

void AMovingBarrier::OnBoxBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	// Limit movement if barrier hits wall
	if ( !OtherActor->ActorHasTag( FName( TEXT( "Wall" ) ) ) )
		return;

	if ( OverlappedComp->GetComponentLocation().Z > OtherActor->GetActorLocation().Z )
	{
		AvialibleMoveDirection = MoveDirection::Up;
	}
	else
	{
		AvialibleMoveDirection = MoveDirection::Down;
	}
}

void AMovingBarrier::OnBoxEndOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	// Limit movement if barrier hits wall
	if ( !OtherActor->ActorHasTag( FName( TEXT( "Wall" ) ) ) )
		return;

	AvialibleMoveDirection = MoveDirection::UpAndDown;
}

#if WITH_EDITORONLY_DATA
TObjectPtr<UArrowComponent> AMovingBarrier::GetArrowComponent() const
{
	return ArrowComponent;
}
#endif // WITH_EDITORONLY_DATA
