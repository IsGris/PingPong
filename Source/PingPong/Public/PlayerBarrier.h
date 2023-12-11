// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "MovingBarrier.h"
#include "PlayerBarrier.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogPlayerBarrier, Log, All );

/**
 * Player in Ping Pong game
 */
UCLASS()
class PINGPONG_API APlayerBarrier : public AMovingBarrier
{
	GENERATED_BODY()
	
	// Init object when started playing
	virtual void BeginPlay() override;
	
	// ~ Input
protected:
	virtual void SetupPlayerInputComponent( UInputComponent* PlayerInputComponent ) override;
	// ~ ~ Actions
protected:
	UPROPERTY( Category = "Input", EditDefaultsOnly )
	TObjectPtr<class UInputAction> MoveAction;
	UPROPERTY( Category = "Input", EditDefaultsOnly )
	TObjectPtr<class UInputAction> PauseAction;
	UPROPERTY( Category = "Input", EditDefaultsOnly )
	TObjectPtr<class UInputMappingContext> PlayerMappingContext;
	// ~ ~ Actions
protected:
	void Move( const struct FInputActionValue& ActionValue );
	// ~ Input
	
};
