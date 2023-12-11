// All rights by IsGris


#include "MovingBarrier.h"
#include "Components/BoxComponent.h"

// Sets default values
AMovingBarrier::AMovingBarrier() : Super()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "CollisionBox" ) );
	CollisionBox->SetupAttachment( RootComponent );
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMesh" ) );
	StaticMesh->SetupAttachment( Cast< USceneComponent >( CollisionBox ) );
	CollisionBox->SetGenerateOverlapEvents( true );
	StaticMesh->SetGenerateOverlapEvents( false );
}

void AMovingBarrier::AddScore( const int& ScoreToAdd )
{
	this->Score += Score;
}