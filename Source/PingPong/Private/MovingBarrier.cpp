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
}

void AMovingBarrier::AddScore( const int& ScoreToAdd )
{
	this->Score += ScoreToAdd;
}

int AMovingBarrier::GetScore() const
{
	return Score;
}

#if WITH_EDITORONLY_DATA
TObjectPtr<UArrowComponent> AMovingBarrier::GetArrowComponent() const
{
	return ArrowComponent;
}
#endif // WITH_EDITORONLY_DATA
