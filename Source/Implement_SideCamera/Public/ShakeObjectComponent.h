// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ShakeObjectComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class IMPLEMENT_SIDECAMERA_API UShakeObjectComponent : public UMovementComponent
{
	GENERATED_BODY()

private:
	

public:
	UStaticMeshComponent* ChandelierBody;
	UShakeObjectComponent();//Default Constructor
	void BeginPlay();
};
