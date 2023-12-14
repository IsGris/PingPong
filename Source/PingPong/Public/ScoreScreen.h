// All rights by IsGris

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreScreen.generated.h"

/**
 * Display player and enemy score on screen
 */
UCLASS()
class PINGPONG_API UScoreScreen : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Set")
	void ChangePlayerScore( const int& Value );
	UFUNCTION( BlueprintCallable, Category = "Set" )
	void ChangeEnemyScore( const int& Value );
	UFUNCTION( BlueprintCallable, Category = "Set" )
	void ChangeScore( const int& PlayerScore, const int& EnemyScore );

protected:
	UPROPERTY( BlueprintReadOnly, meta = ( BindWidget ) )
	class UTextBlock* PlayerScore;

	UPROPERTY( BlueprintReadOnly, meta = ( BindWidget ) )
	class UTextBlock* EnemyScore;
};
