// Fill out your copyright notice in the Description page of Project Settings.


#include "IteractableButton.h"
#include "Kismet/GameplayStatics.h"
#include "LiquidXTest/LiquidXTestCharacter.h"

// Sets default values
AIteractableButton::AIteractableButton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;




	// Create a follow camera
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	
	RootComponent = MeshComp;
	
	ButtonRaiseSpeed = 1;

}

// Called when the game starts or when spawned
void AIteractableButton::BeginPlay()
{
	Super::BeginPlay();

	//save the initial location so that we can go back to it when the button is pressed
	initialLocation = GetActorLocation();
}

// Called every frame
void AIteractableButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//when we press the button it goes down and back up slowly, if the location of the button in the Z axis is smaller than the initial location we keep raising it
	if (GetActorLocation().Z < initialLocation.Z)
	{
		//we exposed a var to BP here so that we can change the speed which the button raises in the editor for convenience
		SetActorLocation(GetActorLocation() + FVector(0, 0, 10 * DeltaTime* ButtonRaiseSpeed));
	}

}

void AIteractableButton::InteractionEffect()
{
	Super::InteractionEffect();

	//push the button down from out initial position
	SetActorLocation(initialLocation - FVector(0, 0, 15));

	//this is just a small effect that teleports the player when he presses the button, we get the PC here
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	ACharacter* character = PlayerController->GetCharacter();


	ALiquidXTestCharacter* liquidXCharacter = Cast<ALiquidXTestCharacter>(character);
	
	//if the cast is sucessfull and the character exists we teleport
	if (liquidXCharacter)
	{
		liquidXCharacter->SetActorLocation(FVector(750,1000,200));
	}

}