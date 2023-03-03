// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractableActor.h"
#include "IteractableButton.generated.h"

/**
 * 
 */
UCLASS()
class LIQUIDXTEST_API AIteractableButton : public ABaseInteractableActor
{
	GENERATED_BODY()


public:
	// Sets AIteractableButton values for this actor's properties
	AIteractableButton();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* MeshComp;

	FVector initialLocation;

	//speed at which the button raises, exposed to BP
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ButtonRaiseSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InteractionEffect();
	
};
