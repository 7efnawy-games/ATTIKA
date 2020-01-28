// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

FVector UGrabber::GetPlayerReach() const
{
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(), ThrowAway;
	FRotator PlayerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(ThrowAway, PlayerRotation);
	return PlayerLocation + Reach * PlayerRotation.Vector();
}

FHitResult UGrabber::GetHit() const
{
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit,
		GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(), GetPlayerReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(), false, GetOwner())
	);
	return Hit;
}

AActor* UGrabber::GetHitActor() const
{
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit,
		GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(), GetPlayerReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(), false, GetOwner())
	);
	if (Hit.IsValidBlockingHit())
		return Hit.GetActor();
	return nullptr;
}

void UGrabber::Grab()
{
	Hit = GetHit();
}

void UGrabber::Release()
{
	if (PhysicsHandle)
		PhysicsHandle->ReleaseComponent();
}

void UGrabber::SetPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (!PhysicsHandle)
		UE_LOG(LogTemp, Error, TEXT("%s is missing its physics handle component!"), *GetOwner()->GetName())
}

void UGrabber::GetInput()
{
	Input = GetOwner()->FindComponentByClass<UInputComponent>();

	if (Input) {
		Input->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		Input->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("%s is missing its input component!"), *GetOwner()->GetName())
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	SetPhysicsHandle();

	GetInput();

	Owner = GetOwner();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*if (Owner && Hit.IsValidBlockingHit()) {
		FVector OwnerLocation = Owner->GetActorLocation(), HitActorLocation = Hit.GetActor()->GetActorLocation();
		float CurrentZ = FMath::Clamp<float>(HitActorLocation.Z, OwnerLocation.Z, HitActorLocation.Z);
		Owner->SetActorLocation(FVector(OwnerLocation.X, OwnerLocation.Y, CurrentZ));
	}*/

}