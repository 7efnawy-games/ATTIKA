// Fill out your copyright notice in the Description page of Project Settings.


#include "ShakeObjectComponent.h"

UShakeObjectComponent::UShakeObjectComponent()
{

	;
}
void UShakeObjectComponent::BeginPlay()
{
	Super::BeginPlay();
	ChandelierBody = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	UE_LOG(LogTemp,Warning,TEXT("THIS IS CHANDELIER BODY %s"),*ChandelierBody->GetName())
}