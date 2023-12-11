// All rights by IsGris

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "Ball.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogPingPongBall, Log, All );

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
	// Does ball can move
	UPROPERTY( BlueprintReadWrite, Category = "Movement" )
	bool CanMove = false;
	// Current speed of ball
	UPROPERTY( EditAnywhere, Category = "Movement" )
	int Speed = 1;
	// How much does speed of ball increase when hit player or enemy
	UPROPERTY( EditDefaultsOnly, Category = "Movement" )
	int Acceleration = 0.5f;
protected:
	// Direction in which ball moving
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector2f Direction;
	// ~ Movement

	// ~ Misc
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
	// ~ Misc
};
