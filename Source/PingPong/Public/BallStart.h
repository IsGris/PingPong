// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "Engine/NavigationObjectBase.h"
#include "BallStart.generated.h"

/**
 * Init position of the ball in level
 */
UCLASS()
class PINGPONG_API ABallStart : public ANavigationObjectBase
{
	GENERATED_BODY()
public:

	ABallStart();

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY()
	TObjectPtr<class UArrowComponent> ArrowComponent;
public:
	/** Returns ArrowComponent subobject **/
	TObjectPtr<class UArrowComponent> GetArrowComponent() const;
#endif
};
