// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInteractableActor.generated.h"

UCLASS()
class LIQUIDXTEST_API ABaseInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void InteractionEffect();

	// this is just a function we call along the interaction for BP's, some stuff like timelines are way easier in BP's
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BPInteractionEffect();

};
