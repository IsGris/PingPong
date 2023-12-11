// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MovingBarrier.generated.h"

/*
* Moving barrier for ping pong game
*/
UCLASS(Abstract)
class PINGPONG_API AMovingBarrier : public APawn
{
	GENERATED_BODY()

protected:
	// Init class
	AMovingBarrier();

	// ~ Movement
public:
	// Does barrier can move
	UPROPERTY( BlueprintReadWrite, Category = "Movement" )
	bool CanMove = false;
	// Speed of the barrier
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = "Movement" )
	int Speed = 1;
	// ~ Movement

	// ~ Statistics
public:
	// Adds score to barrier
	UFUNCTION( BlueprintCallable, Category = "Statistics" )
	void AddScore( const int& ScoreToAdd = 1 );
protected:
	// Score of the barrier
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Statistics" )
	int Score = 0;
	// ~ Statistics

	// ~ Misc
protected:
	// Static mesh of the barrier
	UPROPERTY( EditDefaultsOnly, Category = "Appearance" )
	class UStaticMeshComponent* StaticMesh;
	// Box of the barrier that implements collision
	UPROPERTY( EditDefaultsOnly, Category = "Appearance" )
	class UBoxComponent* CollisionBox;
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY()
	TObjectPtr<class UArrowComponent> ArrowComponent;
public:
	/** Returns ArrowComponent subobject **/
	TObjectPtr<class UArrowComponent> GetArrowComponent() const;
#endif
	// ~ Misc
};
