// All rights by IsGris


#include "PlayerBarrier.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PingPongGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBarrierController.h"

DEFINE_LOG_CATEGORY( LogPlayerBarrier );

void APlayerBarrier::BeginPlay()
{
    Super::BeginPlay();

    if ( const APlayerController* PlayerController = Cast<APlayerController>( Controller ) ) // Init input context
    {
        if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( PlayerController->GetLocalPlayer() ) )
        {
            if ( PlayerMappingContext )
            {
                Subsystem->ClearAllMappings();
                Subsystem->AddMappingContext( PlayerMappingContext, 0 );
            }
            else
                UE_LOG( LogPlayerBarrier, Warning, TEXT( "PlayerMappingContext is not defined" ) );
        }
    }
}

void APlayerBarrier::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
    Super::SetupPlayerInputComponent( PlayerInputComponent );

    // Init input bindings
    UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>( PlayerInputComponent );
    if ( PlayerEnhancedInputComponent )
    {
        if ( MoveAction )
        {
            PlayerEnhancedInputComponent->BindAction( MoveAction, ETriggerEvent::Triggered, this, &APlayerBarrier::Move );
        }
        else
            UE_LOG( LogPlayerBarrier, Warning, TEXT( "MoveAction is not defined" ) );

        if ( PauseAction )
        {
            PlayerEnhancedInputComponent->BindAction( PauseAction, ETriggerEvent::Triggered, Cast<APingPongGameMode>( UGameplayStatics::GetGameMode( GetWorld() ) ), &APingPongGameMode::SwapPause );
        }
        else
            UE_LOG( LogPlayerBarrier, Warning, TEXT( "PauseAction is not defined" ) );
    }
}

void APlayerBarrier::Move( const FInputActionValue& ActionValue )
{
    if ( CanMove )
    {
        if ( APlayerBarrierController* PlayerController = Cast<APlayerBarrierController>( Controller ) )
            PlayerController->MoveUp( ActionValue.Get<float>() * Speed );
        else
            UE_LOG( LogPlayerBarrier, Warning, TEXT( "Can't cast player controller to MovingBarrierController" ) );
    }
}
