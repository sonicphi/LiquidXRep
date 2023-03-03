// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractableActor.h"

// Sets default values
ABaseInteractableActor::ABaseInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//just the base method for interaction
void ABaseInteractableActor::InteractionEffect()
{
	BPInteractionEffect();

}