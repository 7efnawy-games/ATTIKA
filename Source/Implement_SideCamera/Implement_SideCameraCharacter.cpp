// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Implement_SideCameraCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AImplement_SideCameraCharacter

AImplement_SideCameraCharacter::AImplement_SideCameraCharacter()
{
	//Tickable	
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	///Update
	
}

//////////////////////////////////////////////////////////////////////////
// Input
void AImplement_SideCameraCharacter::PrintGrab()
{
	UE_LOG(LogTemp,Warning,TEXT("GRABBING"))
}
void AImplement_SideCameraCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	InputComponent = PlayerInputComponent;
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AImplement_SideCameraCharacter::GrabItem);
	PlayerInputComponent->BindAction("Climb", IE_Pressed, this, &AImplement_SideCameraCharacter::ToggleClimbing);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AImplement_SideCameraCharacter::ParentThrowItem);
	
	///Update
	PlayerInputComponent->BindAxis("MoveForward", this, &AImplement_SideCameraCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AImplement_SideCameraCharacter::MoveRight);
	
	

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	
	
	///Update
	//Remove No need For Mouse
	/*PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AImplement_SideCameraCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AImplement_SideCameraCharacter::LookUpAtRate);*/

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AImplement_SideCameraCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AImplement_SideCameraCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AImplement_SideCameraCharacter::OnResetVR);
}


void AImplement_SideCameraCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AImplement_SideCameraCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AImplement_SideCameraCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AImplement_SideCameraCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AImplement_SideCameraCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AImplement_SideCameraCharacter::MoveForward(float Value)
{
	
	if ((Controller != NULL) && (Value != 0.0f))
	{

		//UE_LOG(LogTemp, Warning, TEXT("Moving with value:%f"),Value)
		
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);


		AddMovementInput(Direction, Value);
	}
}

void AImplement_SideCameraCharacter::MoveRight(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Right"))
	
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void AImplement_SideCameraCharacter::ClimbUp(float Value)
{
	
	if ((Controller != NULL) && (Value != 0.0f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving Up with the value of %f"), Value)

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		//const FRotator RollRotation(0, 0, Rotation.Roll); //3awz ab2a afhmha 3shan eli b3mlo da esmo tahbeed

		// get forward vector

		Cast<UCharacterMovementComponent>(GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Flying);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
		
		AddMovementInput(Direction, Value);
	}
}
void AImplement_SideCameraCharacter::ClimbRight(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Moving Right with the value of %f"),Value)
	if ((Controller != NULL) && (Value != 0.0f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving Up with the value of %f"), Value)

			const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		//const FRotator RollRotation(0, 0, Rotation.Roll); //3awz ab2a afhmha 3shan eli b3mlo da esmo tahbeed

		// get forward vector

		//Cast<UCharacterMovementComponent>(GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Flying);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}
///My Update
void AImplement_SideCameraCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("TESTING Begin play"))
	Set_GrabPoint_ClimbPoint();
	
}
void AImplement_SideCameraCharacter::ParentThrowItem()
{
	ThrowItem(25000, 15000, GrabbedItem);
	
}



///Character Actions
void AImplement_SideCameraCharacter::ToggleClimbing()
{
	if (ClimbTurn)
	{
		Start_Climbing();
	}
	else
	{
		Stop_Climbing();
	}
}

void AImplement_SideCameraCharacter::Start_Climbing()
{	
	FHitResult OutHit;
	FVector StartPoint = GetMesh()->GetComponentLocation() + FVector(0, 0, 130);
	FVector EndPoint = ClimbPoint->GetComponentLocation();


	DrawDebugLine
	(
		GetWorld(),
		StartPoint,
		EndPoint,
		FColor(230, 230, 1),
		true,
		3.f,
		'\000',
		2.f
	);


	FCollisionQueryParams Other_TraceParamaters = FCollisionQueryParams(FName(""), false, this);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsTypes;
	ObjectsTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	bool LineTraceResult = GetWorld()->LineTraceSingleByObjectType(OutHit, StartPoint, EndPoint,
		ObjectsTypes, Other_TraceParamaters);
	if (LineTraceResult)
	{
		///Lock MovementConstraints on Y-Axis
		if (InputComponent != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("INPUT COMPONENT CHANGED"))
				InputComponent->AxisBindings.Empty();//OR Reset

			InputComponent->BindAxis("ClimbRight", this, &AImplement_SideCameraCharacter::ClimbRight);
			InputComponent->BindAxis("ClimbUp", this, &AImplement_SideCameraCharacter::ClimbUp);
		
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("INPUT COMPONENT Didn't Change CHANGED"))

		}
	
		ClimbTurn = false;
	}
}
void AImplement_SideCameraCharacter::Stop_Climbing()
{

	ClimbTurn = true;
	InputComponent->AxisBindings.Empty();//OR Reset
	InputComponent->BindAxis("MoveForward", this, &AImplement_SideCameraCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AImplement_SideCameraCharacter::MoveRight);

	Cast<UCharacterMovementComponent>(GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Walking);

}

void AImplement_SideCameraCharacter::GrabItem()//Bind To akey
{
	USkeletalMeshComponent* SkeltalMesh = GetMesh();
	
	FHitResult OutHit;
	//MultipleLineTraces(OutHit);
	bool UpperTraceResult=SphereTrace(OutHit,22.5f,false);
	bool LowerTraceResult = SphereTrace(OutHit, 22.5f, true);


	if (UpperTraceResult)
	{
		UE_LOG(LogTemp,Warning,TEXT("We Are hitting sth"))
		PhysicsHandler = this->FindComponentByClass<UPhysicsHandleComponent>();
		GrabbedItem = OutHit.GetComponent();
		bGrabItem = true;
		
		PhysicsHandler->GrabComponent(GrabbedItem, NAME_None, GrabPoint->GetComponentLocation(), true);
	}
	else if (LowerTraceResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("We Are hitting sth"))
		PhysicsHandler = this->FindComponentByClass<UPhysicsHandleComponent>();
		GrabbedItem = OutHit.GetComponent();
		bGrabItem = true;
		PhysicsHandler->GrabComponent(GrabbedItem, NAME_None, GrabPoint->GetComponentLocation(), true);
	}
	else
	{

		//UE_LOG(LogTemp, Warning, TEXT("We Are not hitting sth"))
	}
	//bool LineTraceResult = GetWorld()->LineTraceSingleByObjectType(OutHit, StartPoint, EndPoint,
		//ObjectsTypes, Other_TraceParamaters);


	
	//FVector GrabPoint = this->FindComponentByClass<USceneComponent>()->GetComponentLocation();
	
	//UE_LOG(LogTemp,Warning,TEXT("Grab Point %s"),*GrabPoint->GetComponentLocation().ToString())
	
		
	

}
void AImplement_SideCameraCharacter::ThrowItem(float  ZForce, float ForceMagnitude, UPrimitiveComponent* GrabbedItem)//Bind to akey
{
	if(GrabbedItem)
	GrabbedItem->SetEnableGravity(true);
	if (PhysicsHandler&&GrabbedItem)
	{
		PhysicsHandler->ReleaseComponent();
		FVector SideImpulse = GetActorForwardVector()*ForceMagnitude;
		FVector TotalImpulse = SideImpulse + FVector(0, 0, ZForce);
		GrabbedItem->AddImpulse(TotalImpulse, NAME_None , false);

	}
}

void AImplement_SideCameraCharacter::Set_GrabPoint_ClimbPoint()
{
	TArray<USceneComponent*> SceneComponentsinCharacter;
	this->GetComponents<USceneComponent>(SceneComponentsinCharacter);
	
	for (int32 i = 0; i < SceneComponentsinCharacter.Num(); i++)
	{

		if (SceneComponentsinCharacter[i]->GetName() == "CharacterWidthL")
		{
			CharacterLeftPoint = SceneComponentsinCharacter[i];
		}
		 if (SceneComponentsinCharacter[i]->GetName() == "CharacterWidthR")
		{
			CharacterRightPoint = SceneComponentsinCharacter[i];
		}
		 if (SceneComponentsinCharacter[i]->GetName() == "GrabPoint")
		{
			GrabPoint = SceneComponentsinCharacter[i];

		}
		 if (SceneComponentsinCharacter[i]->GetName() == "ClimbPoint")
		{
			ClimbPoint = SceneComponentsinCharacter[i];
		}
		
	}
}


FVector AImplement_SideCameraCharacter::ComplementaryOfForwardVector(FVector V)
{
	V.X = 1 - V.X;
	V.Y = 1 - V.Y;
	V.Z = 1 - V.Z;
	return V;

}
void  AImplement_SideCameraCharacter::Tick(float Deltaseconds)
{
	if (GrabbedItem)
		PhysicsHandler->SetTargetLocation(GrabPoint->GetComponentLocation());
	 Super::Tick(Deltaseconds);
	// UE_LOG(LogTemp,Warning,TEXT("Ticking"))
	

}

bool AImplement_SideCameraCharacter::MultipleLineTraces(FHitResult& OutHit)
{
	FVector CharacterWidthVector = CharacterLeftPoint->GetComponentLocation() - CharacterRightPoint->GetComponentLocation();

	int characterwidth = (int)FMath::Max(abs(CharacterWidthVector.X), abs(CharacterWidthVector.Y));
	if (CharacterLeftPoint && CharacterRightPoint)
		for (int i = 0; i < characterwidth; i++)
		{

			FVector StartPoint = CharacterLeftPoint->GetComponentLocation() + FVector(i * GetActorRightVector().X, i * GetActorRightVector().Y, -30);
			FVector EndPoint = ComplementaryOfForwardVector(GetActorRightVector().GetAbs()) * GrabPoint->GetComponentLocation() + GetActorRightVector().GetAbs() * StartPoint;

			//FVector StartPoint =FVector(0, 0, 0);
			DrawDebugLine
			(
				GetWorld(),
				StartPoint,
				EndPoint,
				FColor(230, 230, 1),
				true,
				3.f,
				'\000',
				2.f
			);


			FCollisionQueryParams Other_TraceParamaters = FCollisionQueryParams(FName(""), false, this);
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsTypes;
			ObjectsTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
			bool LineTraceResult = GetWorld()->LineTraceSingleByObjectType(OutHit, StartPoint, EndPoint,
				ObjectsTypes, Other_TraceParamaters);



		}
	return false;

}
bool AImplement_SideCameraCharacter::SphereTrace(FHitResult& OutHit, float Radius,bool LowTrace)
{	
	FVector StartPoint;
	FVector EndPoint;

	if (LowTrace)
	{
		StartPoint = GetTargetLocation() + FVector(0, 0, -70);
		EndPoint = GrabPoint->GetComponentLocation() + FVector(0, 0, -70);
	}
	else
	{
		StartPoint = GetTargetLocation();
		EndPoint = GrabPoint->GetComponentLocation();
	}
	FCollisionQueryParams Other_TraceParamaters = FCollisionQueryParams(FName(""), false, this);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsTypes;
	ObjectsTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	
	
	bool TraceResult = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), StartPoint, EndPoint, Radius, ObjectsTypes
		, false, { this }, EDrawDebugTrace::Persistent, OutHit, true, FLinearColor::Red);
	return TraceResult;
}

