// All rights by IsGris


#include "BallStart.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"

ABallStart::ABallStart() : Super()
{
	GetCapsuleComponent()->InitCapsuleSize( 50.0f, 50.0f );
	GetCapsuleComponent()->SetShouldUpdatePhysicsVolume( false );

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>( TEXT( "Arrow" ) );

	if ( !IsRunningCommandlet() && ArrowComponent )
	{
		ArrowComponent->ArrowColor = FColor( 150, 200, 255 );

		ArrowComponent->ArrowSize = 1.0f;
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SetupAttachment( GetCapsuleComponent() );
		ArrowComponent->bIsScreenSizeScaled = true;
	}

	bIsSpatiallyLoaded = false;
#endif // WITH_EDITORONLY_DATA
}

#if WITH_EDITORONLY_DATA
TObjectPtr<UArrowComponent> ABallStart::GetArrowComponent() const
{
	return ArrowComponent;
}
#endif // WITH_EDITORONLY_DATA
