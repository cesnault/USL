// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/GameFramework/NavMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Classes/Kismet/KismetMathLibrary.h"
#include "Public/Math/Vector.h"
#include "Camera/CameraComponent.h"
#include "Classes/Components/CapsuleComponent.h"
#include "Classes/Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "WorldCollision.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class USL_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Var list:
	UPROPERTY(EditAnywhere)
	UCameraComponent*	Camera;
	UPROPERTY(EditAnywhere)
	UKismetMathLibrary*	math;
	FRotator			Desired;
	UBoxComponent*		WallJump_box;
	USpringArmComponent*	arms;
	UPROPERTY(EditAnywhere, Category = Mesh)
		USkeletalMeshComponent* skin_fps;
	UPROPERTY(EditAnywhere, Category = Mesh)
		USkeletalMeshComponent* gun_fps;


	UPROPERTY(EditAnywhere, Category = "Base Character")
		float				distance;
	int32					tmp;
	int32					nb_dash;
	UPROPERTY(EditAnywhere,Category = "Base Character")
	int32					jump_count;
	int32					sweet_roll_left;
	int32					sweet_roll_right;
	UPROPERTY(EditAnywhere, Category = "Climb")
		int32					climbing_Speed;
	UPROPERTY(EditAnywhere, Category = "Climb")
		int32					climbing_projection;
	UPROPERTY(EditAnywhere, Category = "Climb")
		int32					climbing_max_distance;
	UPROPERTY(EditAnywhere, Category = "Wall Run")
		int32							wallrun_speed;
	UPROPERTY(EditAnywhere, Category = "Wall Jump")
		int32							walljump_force;
	UPROPERTY(EditAnywhere, Category = "Slide")
		int32							slide_speed;
	UPROPERTY(EditAnywhere, Category = "Dash")
		int32							dash_speed;
	UPROPERTY(EditAnywhere, Category = "Dash")
		float							dash_Z;
	UPROPERTY(EditAnywhere, Category = "Wall Jump")
		float							walljump_z;
	UPROPERTY(EditAnywhere, Category = "Wall Run")
		int32					wallrun_max_distance;
	UPROPERTY(EditAnywhere, Category = "Slide")
		int32				slide_max_distance;
	int32					slowdown;

	FVector				Location;
	FVector				tot;
	FVector				dir_wall_running;
	FVector				dir_linetrace_right;
	FVector				dir_linetrace_left;
	FVector				dir_slide;
	FVector				origin;
	FRotator			view;
	FRotator			rots;
	FTimerHandle		time;
	
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool				bIsJumping;
	bool				bIsSliding;
	bool				bIsleft;
	bool				bCanWallJump;
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool				bCanWallRun;
	bool				bCanSlide;
	bool				bCanDash;
	bool				Stop_slide;
	bool				bIsBlockMove;
	bool				bIsclimbing;
	bool				bIsLastTickClimbing;
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool				bJump_function;
	bool				bIsWallRunning;
	bool				bIsRoll_Left;
	bool				bIsRoll_Right;
	bool				bIsCameraRoll;
	bool				bIsForwardKeyPress;
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool				bResetClimb;
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool				bResetWallRun;
	//Var for linetrace
	FHitResult			OutHit;
	FVector				Start;
	FVector				End;
	ECollisionChannel	TraceChannel;
	FCollisionQueryParams Params;
	FCollisionResponseParams ResponseParam;
	FCollisionQueryParams CollisionParams;
	FCollisionShape			CollisionShape;
	
	FQuat					Rot;
	FTransform				transform;
	FVector					translation;
	
	//Function list :
	// Function called when a collision box start to overlaps another one
	UFUNCTION()
	void				NotifyActorBeginOverlap(AActor * OtherActor) override;
	
	// Function called when a collision box doesn't overlaps anymore with another one
	void				NotifyActorEndOverlap(AActor * OtherActor) override;
	
	// Move the character forward(+) and backward(-)
	void				forward(float Value);
	
	// Move the character on the right(+) and on the left(-)
	void				lateral(float Value);
	
	//Rotate the character on the right(+) and on the left(-)
	void				turn_character(float Value);
	
	void				look_up(float Value);
	//Climb  wall if the player is right in front of a wall
	void				wall_climbing();
	
	//Function called when the player try to run and check if he can
	void				wall_running();
	
	//When the player try to slide
	void				launch_slide();
	
	//Reverse the state of slide
	void				unslide();
	
	//Function that check if the wall is on the left if not go to wall_run_righ()
	void				wall_run_left();
	
	//Function that check if the wall is on the right if not go to wall_run_left();
	void				wall_run_right();
	
	//Function called when the player start a wall run
	void				wall_run();
	
	//(Wrong name)timer that stop the player to use infinite dash
	void				dashlateral();
	
	//Fuction called when the player want to dash in front of him
	void				front_dash();
	
	//Fuction called when the player want to dash behind him
	void				back_dash();
	//Check if the player can crouch
	void				Try_crouch();
	
	//Fuction called when the player want to dash on the left
	void				left_dash();
	
	//Function called when the player start a slide
	void				slide();
	
	// Launch the character where he is looking if he can perform a wall jump
	void				walljump();
	
	//Function called when spacebar is pressed
	void				jump_function();
	
	void				dash();
	
	//In progress
	void				test();
	
	//Function called to know if the key jump is hold
	void				Jumphold();
	
	//Function called when the key jump is released
	void				JumpRelease();
	
	//Check if the player can actually wall run
	void				CanWallRun();
	
	//Reset the angle of the camera after the end of the wall run
	void				reset_roll_right_cam();
	void				reset_roll_left_cam();
};
