// All rights by IsGris


#include "PingPongLibrary.h"
#include "PingPongGameMode.h"
#include "PingPongGameInstance.h"
#include "Kismet/GameplayStatics.h"

APingPongGameMode* UPingPongLibrary::GetPingPongGameMode( const UObject* WorldContext )
{
	return Cast<APingPongGameMode>( UGameplayStatics::GetGameMode( WorldContext ) );
}

UPingPongGameInstance* UPingPongLibrary::GetPingPongGameInstance( const UObject* WorldContext )
{
	return Cast<UPingPongGameInstance>( UGameplayStatics::GetGameInstance( WorldContext ) );
}
