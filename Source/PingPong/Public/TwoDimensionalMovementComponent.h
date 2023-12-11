// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TwoDimensionalMovementComponent.generated.h"

/**
 * Component which implements movement in 2 dimensions for actors
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PINGPONG_API UTwoDimensionalMovementComponent : public UActorComponent
{
	GENERATED_BODY()
	
	// ~ Inits
protected:
	// Calls when game started
	virtual void BeginPlay() override;
	// Calls when all others begin play is called
	void AfterBeginPlay();
	// ~ Inits

	// Owner of the component
	TObjectPtr<USceneComponent> Owner;

	// ~ Movement
protected:
	float CurrentSpeedHorizontal = 0;
	float CurrentSpeedVertical = 0;
	float SpeedHorizontal = 0;
	float SpeedVertical = 0;
	inline bool HaveMovement();
public:
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void SetHorizontalSpeed( const float& Speed );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void SetVerticalSpeed( const float& Speed );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void SetSpeed( const float& Speed );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void StartMovingUp( const float& Speed = 0 );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void StartMovingDown( const float& Speed = 0 );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void StartMovingLeft( const float& Speed = 0 );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void StartMovingRight( const float& Speed = 0 );
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void StopHorizontalMovement();
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void StopVerticalMovement();
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void StopMovement();
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
	void Move( const FVector& Value );
	// Called for constant movement of the owner
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	// ~ Movement
};
