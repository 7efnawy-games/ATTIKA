// Fill out your copyright notice in the Description page of Project Settings.

#include "Chandlier.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include"Components/SpotLightComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
AChandlier::AChandlier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* EmptyScene= CreateDefaultSubobject<USceneComponent>(FName("Scene"));
	SetRootComponent(EmptyScene);

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(FName("Plane"));
	Plane->SetupAttachment(EmptyScene);
	Plane->SetMobility(EComponentMobility::Movable);

	Chandlier = CreateDefaultSubobject<UStaticMeshComponent>(FName("Chandlier"));
	Chandlier->SetupAttachment(EmptyScene);
	Chandlier->SetMobility(EComponentMobility::Movable);

	ChandlierLight = CreateDefaultSubobject<USpotLightComponent>(FName("SpotLight"));
	ChandlierLight->SetupAttachment(Chandlier);
	
	
	Plane_Chandlier_Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Plane Chandlier Constraint"));
	Plane_Chandlier_Constraint->SetupAttachment(Plane);

	Plane_Chandlier_Constraint->SetRelativeLocation(FVector(0, 0, -50));
	if(Chandlier->Mobility!= EComponentMobility::Movable && Plane->Mobility!=EComponentMobility::Movable)
	{

		UE_LOG(LogTemp, Warning, TEXT("Plane :Moveable, Chandlier:Moveable"))
		Plane_Chandlier_Constraint->SetConstrainedComponents(Chandlier, NAME_None, Plane, NAME_None);
	}
	UE_LOG(LogTemp, Warning, TEXT("Plane : Not Moveable, Chandlier:Not Moveable"))


		//UnComment Later
		
	

	Chandlier->SetWorldLocation(Plane->GetComponentLocation());
	//UE_LOG(LogTemp, Warning, TEXT("Chandlier WorldLocation %s"),*Chandlier->GetComponentLocation().ToString())

	Chandlier->SetRelativeLocation(FVector(0, 0,-200));
	//UE_LOG(LogTemp, Warning, TEXT("Chandlier WorldLocation NOW %s"),*Chandlier->GetComponentLocation().ToString())

	ChandlierLight ->SetRelativeRotation(FRotator(-90.f,0.f,0.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Plane_Mesh(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (Plane_Mesh.Object)
		Plane->SetStaticMesh(Plane_Mesh.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder'"));

	if (Cylinder.Object)
		Chandlier->SetStaticMesh(Cylinder.Object);


	UE_LOG(LogTemp,Warning,TEXT("CONSTRUCTED"))


}

// Called when the game starts or when spawned
void AChandlier::BeginPlay()
{
	Super::BeginPlay();
	
	
}
void AChandlier::HezElChandlier()
{
	FVector Torque = FVector(100.f, 100.f, 100.f);//Direction is the axis of rotation
	Chandlier->AddTorqueInDegrees(Torque, NAME_None, true);
}
void AChandlier::DropChandlier()
{
	Plane_Chandlier_Constraint->BreakConstraint();
	Chandlier->AddForce(FVector(0, 0, -5000), NAME_None, true);
}
// Called every frame
void AChandlier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	TryToDropChandlier();
}
void AChandlier::TryToDropChandlier()
{	
	FHitResult OutHit;
	FVector StartLocation = Chandlier->GetComponentLocation();
	FVector EndLocation = StartLocation + FVector(0, 0, -5000);
	FCollisionQueryParams Paramaters = FCollisionQueryParams(FName(""), false, this);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsTypes;
	ObjectsTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	ObjectsTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));


	bool Vertical_LineTraceResult = GetWorld()->LineTraceSingleByObjectType(OutHit, StartLocation, EndLocation,
		FCollisionObjectQueryParams(ObjectsTypes), Paramaters);
	if (Vertical_LineTraceResult)
	{
		UE_LOG(LogTemp,Warning,TEXT("I HIT A STH"))
		DropChandlier();
	}
	

}

