// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class PINGPONG_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
	// Init class
	AEnemyController();

	// ~ Movement
public:
	// Make move to get closer to the ball
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void PerformMove( const float& MaxMoveDistance, const enum MoveDirection& AvialibleDirection );
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
protected:
	// Called for move enemy
	virtual void Tick( float DeltaTime ) override;
	// ~ Movement
};
