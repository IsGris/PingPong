// All rights by IsGris


#include "Ball.h"
#include "BallController.h"
#include "PingPongGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"

DEFINE_LOG_CATEGORY( LogPingPongBall );

ABall::ABall() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	CollisionSphere = CreateDefaultSubobject<USphereComponent>( TEXT( "CollisionSphere" ) );
	RootComponent = Cast<USceneComponent>(CollisionSphere);
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Sphere" ) );
	Sphere->AttachToComponent( GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform );
	Sphere->OnComponentBeginOverlap.AddDynamic( this, &ABall::OnBoxBeginOverlap );
#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>( TEXT( "Arrow" ) );

	if ( !IsRunningCommandlet() && ArrowComponent )
	{
		ArrowComponent->ArrowColor = FColor( 150, 200, 255 );

		ArrowComponent->ArrowSize = 1.0f;
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SetupAttachment( GetRootComponent() );
		ArrowComponent->bIsScreenSizeScaled = true;
	}

	bIsSpatiallyLoaded = false;
#endif // WITH_EDITORONLY_DATA
	UE_LOG( LogPingPongBall, Log, TEXT( "Components inited" ) );
}

void ABall::BeginPlay()
{
	Super::BeginPlay();

	ResetDirection();

}

void ABall::ResetDirection()
{
	Direction = { float( FMath::RandBool() ? 1 : -1 ), FMath::RandRange( 1.0f, -1.0f ) };
	CurrentSpeed = StartSpeed;
	UE_LOG( LogPingPongBall, Verbose, TEXT( "Direction reseted" ) );
}

void ABall::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if ( CanMove )
	{
		if ( ABallController* BallController = Cast<ABallController>( Controller ) )
			BallController->Move( Direction * DeltaTime * CurrentSpeed );
		else
			UE_LOG( LogPingPongBall, Warning, TEXT( "Can't cast ball controller to ABallController" ) );
	}
}

void ABall::OnBoxBeginOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if ( OtherActor->ActorHasTag( FName( TEXT( "Wall" ) ) ) ) // Hitted down or up wall
		Direction.Y *= -1;
	else if ( OtherActor->ActorHasTag( FName( TEXT( "MovingBarrier" ) ) ) ) // Hitted player or enemy
	{
		if ( ( OtherActor->ActorHasTag( FName( TEXT( "Player" ) ) ) && Direction.X >= 0 ) || ( OtherActor->ActorHasTag( FName( TEXT( "Enemy" ) ) ) && Direction.X < 0 ) )
			return;

		FVector CollidedOrigin;
		FVector CollidedBounds;
		OtherActor->GetActorBounds( true, CollidedOrigin, CollidedBounds );
		
		float TopPoint = CollidedOrigin.Z + CollidedBounds.Z;
		float BottomPoint = CollidedOrigin.Z - CollidedBounds.Z;
		float BallPosition = OverlappedComp->GetComponentLocation().Z;
		float DistanceToTop = FMath::Abs( BallPosition - TopPoint );
		float DistanceToBottom = FMath::Abs( BallPosition - BottomPoint );
		float TotalDistance = FMath::Abs( TopPoint - BottomPoint );
		
		Direction = FVector2f( -Direction.X, FMath::Clamp(1 - 2 * ( DistanceToTop / TotalDistance ), -0.85f, 0.85f) );
		CurrentSpeed += Acceleration;
		
		OnBarrierHit.Broadcast();
	}
	else if ( OtherActor->ActorHasTag( FName( TEXT( "PlayerGate" ) ) ) ) // Hitted with gate
		OnPlayerGateScored.Broadcast();
	else if ( OtherActor->ActorHasTag( FName( TEXT( "EnemyGate" ) ) ) )
		OnEnemyGateScored.Broadcast();
}

#if WITH_EDITORONLY_DATA
TObjectPtr<UArrowComponent> ABall::GetArrowComponent() const
{
	return ArrowComponent;
}
#endif // WITH_EDITORONLY_DATA
