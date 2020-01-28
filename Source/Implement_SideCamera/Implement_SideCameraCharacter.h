// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TriggerVolume.h"
#include "Implement_SideCameraCharacter.generated.h"

class UPhysicsHandleComponent;
UCLASS(config=Game)
class AImplement_SideCameraCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AImplement_SideCameraCharacter();
	void PrintGrab();
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);
	
	UFUNCTION(BlueprintCallable)
	void ClimbUp(float Value);


	/** Called for side to side input */
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable)
	void ClimbRight(float Value);
	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	///Update
	
	
	//UFUNCTION()
	//Functions
	//void OverlapImplement(AActor* OverlappedActor, AActor* OtherActor);
	void BeginPlay();
	void Tick(float DeltaSeconds);
	
	UFUNCTION(BlueprintCallable)
	void ParentThrowItem();
	
	void ThrowItem(float ZForce, float ForceMagnitude, UPrimitiveComponent* GrabbedItem);
	
	UFUNCTION(BlueprintCallable)
	void GrabItem();
	bool MultipleLineTraces(FHitResult & OutHit);
	bool SphereTrace(FHitResult  &OutHit,float Radius, bool LowTrace);

	void ToggleClimbing();	
	void Start_Climbing();
	void Stop_Climbing();
	
	void Set_GrabPoint_ClimbPoint();


	FVector ComplementaryOfForwardVector(FVector V);
	//Attributes 
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
		bool bGrabItem=false;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UPrimitiveComponent* GrabbedItem=nullptr;

	UPhysicsHandleComponent* PhysicsHandler=nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Capsule")
	ATriggerVolume* CameraTrigger=nullptr;
	
	UInputComponent* InputComponent;
	USceneComponent* GrabPoint;
	USceneComponent* ClimbPoint;
	USceneComponent* CharacterRightPoint;
	USceneComponent* CharacterLeftPoint;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbing)
	bool ClimbTurn=true;


};

