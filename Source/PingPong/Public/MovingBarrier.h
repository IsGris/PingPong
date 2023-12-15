// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MovingBarrier.generated.h"

UENUM()
enum class MoveDirection
{
	NoMovement      UMETA( DisplayName = "NoMovement" ),
	UpAndDown       UMETA( DisplayName = "UpAndDown" ),
	Up              UMETA( DisplayName = "Up" ),
	Down            UMETA( DisplayName = "Down" )
};

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
	// Speed of the barrier
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = "Movement" )
	int Speed = 1;
protected:
	// Where can move this barrier
	MoveDirection AvialibleMoveDirection = MoveDirection::NoMovement;
public:
	// Set where can move this barrier
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void SetAvialibleMoveDirection( const MoveDirection& Value );
	// Get where can move this barrier
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	MoveDirection GetAvialibleMoveDirection() const;
	// ~ Movement

	// ~ Statistics
public:
	// Adds score to barrier
	UFUNCTION( BlueprintCallable, Category = "Statistics" )
	void AddScore( const int& ScoreToAdd = 1 );
	// Get score of this barrier
	UFUNCTION( BlueprintCallable, Category = "Statistics" )
	int GetScore() const;
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
	// Limit movement if hits wall
	UFUNCTION()
	void OnBoxBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
	void OnBoxEndOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );
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
