// All rights by IsGris

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "Ball.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogPingPongBall, Log, All );

DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnBallScoredGate );
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnBallHitBarrier );

class FOnBallScoredGate;
class FOnBallHitBarrier;

/**
 * Ball for Ping Pong game
 */
UCLASS()
class PINGPONG_API ABall : public APawn
{
	GENERATED_BODY()

protected:
	// ~ Inits
	// Init class
	ABall();
	// Init Game
	virtual void BeginPlay() override;
	// ~ Inits

	// ~ Movement
public:
	// Reset direction to random for ball
	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void ResetDirection();
	// Does ball can move
	UPROPERTY( BlueprintReadWrite, Category = "Movement" )
	bool CanMove = false;
	// Speed of ball on start
	UPROPERTY( EditDefaultsOnly, Category = "Movement" )
	int StartSpeed = 1;
	// How much does speed of ball increase when hit player or enemy
	UPROPERTY( EditDefaultsOnly, Category = "Movement" )
	int Acceleration = 0.5f;
protected:
	// Current speed of ball
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite , Category = "Movement" )
	int CurrentSpeed;
protected:
	// Direction in which ball moving
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector2f Direction;
	// ~ Movement

	// ~ Misc
public:
	// Broadcasts when the ball was scored into the player's gate
	UPROPERTY( BlueprintAssignable )
	FOnBallScoredGate OnPlayerGateScored;
	// Broadcasts when the ball was scored into the enemy's gate
	UPROPERTY( BlueprintAssignable )
	FOnBallScoredGate OnEnemyGateScored;
	UPROPERTY( BlueprintAssignable )
	FOnBallHitBarrier OnBarrierHit;
protected:
	// Called evey frame
	virtual void Tick( float DeltaTime ) override;
	UFUNCTION()
	void OnBoxBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	// Sphere of ball
	UPROPERTY( EditDefaultsOnly, Category = "Sphere" )
	class UStaticMeshComponent* Sphere;
	// Sphere of ball that implements collision
	UPROPERTY( EditDefaultsOnly, Category = "Sphere" )
	class USphereComponent* CollisionSphere;
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
