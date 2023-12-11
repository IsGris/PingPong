// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerBarrierController.generated.h"

/**
 * Controller for Player in Ping Pong game
 */
UCLASS()
class PINGPONG_API APlayerBarrierController : public APlayerController
{
	GENERATED_BODY()

	// Init class
	APlayerBarrierController();

	// ~ Movement
public:
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void MoveUp( const float& Value );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void MoveDown( const float& Value );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void MoveLeft( const float& Value );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void MoveRight( const float& Value );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void Move( const FVector2f& Value );
	// ~ ~ Components
protected:
	TObjectPtr<class UTwoDimensionalMovementComponent> MovementComponent;
	// ~ ~ Components
	// ~ Movement
};
