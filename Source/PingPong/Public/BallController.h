// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "BallController.generated.h"

/**
 * Controller for Ball in Ping Pong
 */
UCLASS()
class PINGPONG_API ABallController : public AController
{
	GENERATED_BODY()

	ABallController();

	// ~ Movement
public:
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void MoveUp( const float& Value );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void MoveDown( const float& Value );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void MoveRight( const float& Value );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void MoveLeft( const float& Value );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void Move( const FVector2f& Value );
protected:
	// Component for move ball in 2 dimensions
	TObjectPtr<class UTwoDimensionalMovementComponent> MovementComponent;
	// ~ Movement

};
