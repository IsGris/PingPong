// All rights by IsGris


#include "ScoreScreen.h"
#include "Components/TextBlock.h"

void UScoreScreen::ChangePlayerScore( const int& Value )
{
	if ( PlayerScore )
		PlayerScore->SetText( FText::AsNumber( Value ) );
}

void UScoreScreen::ChangeEnemyScore( const int& Value )
{
	if ( EnemyScore )
		EnemyScore->SetText( FText::AsNumber( Value ) );
}

void UScoreScreen::ChangeScore( const int& NewPlayerScore, const int& NewEnemyScore )
{
	ChangePlayerScore( NewPlayerScore );
	ChangeEnemyScore( NewEnemyScore );
}
