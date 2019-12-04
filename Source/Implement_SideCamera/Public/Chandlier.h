// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chandlier.generated.h"
class UStaticMeshComponent;
class USpotLightComponent;
class UPhysicsConstraintComponent;
UCLASS()
class IMPLEMENT_SIDECAMERA_API AChandlier : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChandlier();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Plane=nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* Chandlier = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpotLightComponent* ChandlierLight = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UPhysicsConstraintComponent* Plane_Chandlier_Constraint = nullptr;
	
	UFUNCTION(BlueprintCallable)
		void DropChandlier();
	UFUNCTION(BlueprintCallable)
		void HezElChandlier();//Lamo2a5za
	UFUNCTION(BlueprintCallable)
		void TryToDropChandlier();

};
