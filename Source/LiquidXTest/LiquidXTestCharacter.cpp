// Copyright Epic Games, Inc. All Rights Reserved.

#include "LiquidXTestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "BaseInteractableActor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"


//////////////////////////////////////////////////////////////////////////
// ALiquidXTestCharacter

ALiquidXTestCharacter::ALiquidXTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm




	//MY STUFFFF
	//latest version i was working with was .26 

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(RootComponent);



	RadialForceComponent->Radius = 5000;
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->ImpulseStrength = 100000;
	


	//this is new in ue5 i assume, very nice
	JumpMaxCount = 2;
	JumpMaxHoldTime = 0;

	myJumpCount = 0;

	isJetPackActive = false;
	isHoldingObject = false;
	
	//-----------------
	
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ALiquidXTestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALiquidXTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALiquidXTestCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALiquidXTestCharacter::Look);

		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ALiquidXTestCharacter::Interact);

		//Interacting
		EnhancedInputComponent->BindAction(SpellAction, ETriggerEvent::Started, this, &ALiquidXTestCharacter::Spell);

	}

}

void ALiquidXTestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALiquidXTestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ALiquidXTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//when the jetpack is active we add a force on tick
	if (isJetPackActive)
	{
 		GetCharacterMovement()->AddForce(FVector(0, 0, 100000));
	}


	//We raycast from the camera to see what we are looking at, this is nescessary for the interactions
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	//params.TraceTag = TraceTag;

	GetWorld()->LineTraceSingleByChannel(LookingAtResults, GetFollowCamera()->GetComponentLocation(), (GetFollowCamera()->GetForwardVector() * 100000) + GetFollowCamera()->GetComponentLocation(), ECollisionChannel::ECC_WorldDynamic, params);

	//UPDATE CUBE PHYSICS
	//every frame we pull the cube towards our right hand
	if (actorBeingHeld)
	{
		GetMesh()->GetBoneLocation(FName("hand_r"));

		PhysicsHandle->SetTargetLocation(GetMesh()->GetBoneLocation(FName("hand_r")));
	}

}

void ALiquidXTestCharacter::Jump()
{
	
	//we count how many times we jumped and we either do a regular jump by calling the Super function or we use the jetpack
	//i put both in the same key because it would feel better from a game design perspective 
	
	if (myJumpCount >= JumpMaxCount)
	{
		//the jetpack movement only works with "flying mode"
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);	
		isJetPackActive = true;
	}
	else
	{
		Super::Jump();
	}

	myJumpCount++;
	
}

void ALiquidXTestCharacter::StopJumping()
{
	//when we stop jumping we set the mode to falling so that unreal can run its internal stuff properly
	//we also say that the jetpack is turned off
	Super::StopJumping();

	isJetPackActive = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);

}


void ALiquidXTestCharacter::Landed(const FHitResult& Hit)
{
	//when we land we reset our jump count so we can jump again
	Super::Landed(Hit);
	myJumpCount = 0;
}

void ALiquidXTestCharacter::Interact()
{
	//we use the actor we fetch every frame here
	ABaseInteractableActor* lookingAtActor = Cast<ABaseInteractableActor>(LookingAtResults.GetActor());


	//if the cast was sucessfull to one of our "interactable" classes we use E to interact if not we use E to grab
	if (lookingAtActor)
	{
		lookingAtActor->InteractionEffect();
	}
	else
	{
		//if the cast failed then its a pickup cube

		//this could've been done with an attach to socket but it looks cooler this way
		if(isHoldingObject == false)
		{
			//if we are not holding an object we pick it up
			heldActorColisionComp = LookingAtResults.GetComponent();
			actorBeingHeld = LookingAtResults.GetActor();
			PhysicsHandle->GrabComponentAtLocation(LookingAtResults.GetComponent(), FName(""), LookingAtResults.ImpactPoint);
			isHoldingObject = true;
			// we use PhysicsHandle to grab the object smoothly
		}
		else
		{
			//if we are holding an object we clear vars and throw it away
			actorBeingHeld = nullptr;
			//heldActorColisionComp->SetEnableGravity(false);
			if (heldActorColisionComp)
			{
				heldActorColisionComp->AddImpulse(GetActorForwardVector()*100000);
			}
		
			PhysicsHandle->ReleaseComponent();
			isHoldingObject = false;			
		}
	}
}

//this is a radial "fus roh dah!!!!!!!"
void ALiquidXTestCharacter::Spell()
{
	//shake in BP because its way easier and its just 1 node
	BPCameraShake();
	RadialForceComponent->FireImpulse();
}
