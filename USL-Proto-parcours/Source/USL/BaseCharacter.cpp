// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create a camera component, attach it to the root component, set his location near the head.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	
	//Camera will follow the rotation give by the controller of this Pawn
	Camera->bUsePawnControlRotation = false;

	// Create a Collision box who will fire overlaps events about wall jump;
	WallJump_box = CreateDefaultSubobject<UBoxComponent>(TEXT("collision_parcours"));
	WallJump_box->SetupAttachment(RootComponent);
	WallJump_box->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	WallJump_box->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	skin_fps = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skin FPS"));
	skin_fps->SetupAttachment(Camera);

	gun_fps = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun FPS"));
	gun_fps->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	//Init int Var
	jump_count = 0;
	nb_dash = 0;
	tmp = 0;
	JumpMaxCount = 1;
	JumpMaxHoldTime = 0.2;
	sweet_roll_left = 0;
	sweet_roll_right = 0;

	//Init Bool Var
	bCanWallRun = false;
	bIsclimbing = false;
	bIsLastTickClimbing = false;
	bIsJumping = false;
	bCanWallJump = false;
	bCanDash = false;
	bIsleft = true;
	bIsRoll_Left = false;
	bIsRoll_Right = false;
	bIsSliding = false;
	bIsCameraRoll = false;
	bResetClimb = false;
	bResetWallRun = true;
	bCanSlide = true;
	//Init Vector Var
	origin = FVector(0, 0, 0);
	translation = FVector(0, 0, 1);
	tot = FVector(0, 0, 0);
	Start = FVector(0, 0, 100);
	End = FVector(0, 0, -55);
	dir_wall_running = FVector(0, 0, 0);
	dir_slide = FVector(0, 0, 0);
	
	// LineTrace will look only for static mesh and will ignore this actor
	TraceChannel = ECC_WorldStatic;
	Params.AddIgnoredActor(this);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Start = this->GetTransform().GetLocation();
	End = Start;
		End += GetActorForwardVector() * 35;
	End = Start;
	End += Camera->GetForwardVector() * 35;
	
	//Prepare the start vector and the end vector for a linetrace
	Start = this->GetTransform().GetLocation();
	Start.Z = Start.Z - this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 1;
	End = Start;
	End.Z = Start.Z - 2;
	DrawDebugLine(GetWorld(), Start, End, FColor(0, 0, 255), false, -1, 0, 2.0);
	DrawDebugBox(GetWorld(), End, FVector(30, 30, 2), FColor(0, 0, 255), false,-1,0,2);
	
	//look if the character touch the ground
	if (GetWorld()->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CollisionShape, Params, ResponseParam))
	{
		bIsJumping = false;
		bCanDash = true;
		bResetClimb = false;
		bResetWallRun = false;
	}
	else
		bIsJumping = true;
	
	if (bJump_function == true)
		jump_function();

	if ((bJump_function == false && bIsRoll_Right == true) || (bIsRoll_Right == true && bIsWallRunning == false))
		reset_roll_right_cam();
	else if((bJump_function == false && bIsRoll_Left == true) || (bIsRoll_Left == true && bIsWallRunning == false))
		reset_roll_left_cam();

	if (bIsSliding == true)
		launch_slide();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//Bind movement event
	PlayerInputComponent->BindAxis("forward", this, &ABaseCharacter::forward);
	PlayerInputComponent->BindAxis("lateral", this, &ABaseCharacter::lateral);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABaseCharacter::Try_crouch);
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ABaseCharacter::slide);
	PlayerInputComponent->BindAction("Slide", IE_Released, this, &ABaseCharacter::unslide);
	//Bind dash event
	PlayerInputComponent->BindAction("Back_dash", IE_Pressed, this, &ABaseCharacter::back_dash);
	PlayerInputComponent->BindAction("Front_dash", IE_Pressed, this, &ABaseCharacter::front_dash);
	PlayerInputComponent->BindAction("Left_dash", IE_Pressed, this, &ABaseCharacter::left_dash);
	PlayerInputComponent->BindAction("Right_dash",IE_Pressed, this, &ABaseCharacter::dashlateral);
	
	//Bind mouse event
	PlayerInputComponent->BindAxis("turn", this, &ABaseCharacter::turn_character);
	PlayerInputComponent->BindAxis("lookup", this, &ABaseCharacter::look_up);
	
	//Bind jump event
	PlayerInputComponent->BindAction("jump", IE_Pressed, this, &ABaseCharacter::walljump);
	PlayerInputComponent->BindAction("jump", IE_Pressed, this, &ABaseCharacter::Jumphold);
	PlayerInputComponent->BindAction("jump", IE_Released, this, &ABaseCharacter::JumpRelease);
	PlayerInputComponent->BindAction("jump", IE_Pressed, this, &ABaseCharacter::Jump);
	PlayerInputComponent->BindAction("jump", IE_Released, this, &ABaseCharacter::StopJumping);
}

void	ABaseCharacter::launch_slide()
{
	if (dir_slide == FVector(0, 0, 0))
	{
		bIsBlockMove = true;
		dir_slide = GetActorForwardVector();
			view = this->GetActorRotation();
			view.Pitch = 80;
			this->SetActorRotation(view);
			view = Camera->GetComponentRotation();
			view.Pitch = 0;
			view.Yaw = GetActorRotation().Yaw;
			view.Roll = GetActorRotation().Roll;
			Camera->SetWorldRotation(view);
			bIsBlockMove = false;
			origin = GetActorLocation();
			slowdown = 0;
	}
	if (Stop_slide == false)
	{
		
		
		Start = this->GetTransform().GetLocation();
		Start.Z = Start.Z - 10;
		End = Start + dir_slide * 88;
		DrawDebugLine(GetWorld(), Start, End, FColor(0, 0, 255), false, -1, 0, 2.0);
		//look if the character touch the ground
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params, ResponseParam))
		{
			Stop_slide = true;
			unslide();
			return;
		}

		if (FVector::Distance(origin, GetActorLocation()) < slide_max_distance)
			LaunchCharacter(dir_slide * slide_speed, true, false);
		else
		{
			slowdown -= 5;
			if (slide_speed + slowdown >= 0)
				LaunchCharacter(dir_slide * (slide_speed + slowdown), true, false);
			else
			{
				Stop_slide = true;
				unslide();
			}
		}
	}
	}
 
void ABaseCharacter::forward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
	if (Value > 0)
		bIsForwardKeyPress = true;
	else
		{
		bIsForwardKeyPress = false;
		}
}

void ABaseCharacter::lateral(float Value)
{
	if(bIsSliding == false)
		AddMovementInput(GetActorRightVector(), Value);
}

void ABaseCharacter::look_up(float Value)
{
	//AddControllerPitchInput(Value);
	if (!bIsBlockMove)
	{
		view = Camera->GetComponentRotation();
		if (bIsSliding == false)
		{
			if ((view.Pitch >= 80 && Value * 2 > 0) || (view.Pitch <= -80 && Value * 2 < 0))
				view.Pitch += -1 * (Value * 2);
			else if (view.Pitch < 80 && view.Pitch > -80)
				view.Pitch += -1 * (Value * 2);
		}
		else
		{
			if ((view.Pitch >= 80 && Value * 2 > 0) || (view.Pitch <= 20 && Value * 2 < 0))
				view.Pitch += -1 * (Value * 2);
			else if (view.Pitch < 80 && view.Pitch > 20)
				view.Pitch += -1 * (Value * 2);
		}
		Camera->SetWorldRotation(view);
	}
}

void ABaseCharacter::turn_character(float Value)
{
//	if (bIsclimbing == false && bIsCameraRoll == false)
//	if(!bIsSliding)
		AddControllerYawInput(Value);
	
}

void ABaseCharacter::Jumphold()
{
	bJump_function = true;
	GetWorld()->GetTimerManager().SetTimer(time, this, &ABaseCharacter::CanWallRun, 0.1f, false, 0.3f);
}

void ABaseCharacter::JumpRelease()
{
	bJump_function = false;
	bIsclimbing = false;
	bCanWallRun = false;
}

void	ABaseCharacter::CanWallRun()
{
	if(bJump_function == true)
	bCanWallRun = true;

}

void ABaseCharacter::Try_crouch()
{
	if (CanCrouch() == true)
	{
		Crouch();
		bCanSlide = false;
	}
	else
	{
		UnCrouch();
		bCanSlide = true;
	}
}
void	ABaseCharacter::wall_climbing()
{

	Start = this->GetTransform().GetLocation();
	End = Start + (GetActorForwardVector() * 35);
	DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), false, -1, 0, 2.0);
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params, ResponseParam) && bResetClimb == false)
	{
		bIsclimbing = true;
		bIsLastTickClimbing = true;
		if (origin == FVector(0, 0, 0))
		{
			origin = GetActorLocation();
		}
		if ((distance =  FVector::Distance(origin, GetActorLocation())) >= climbing_max_distance)
		{
			bIsLastTickClimbing = false;
				bIsclimbing = false;
				bResetClimb = true;
				origin = FVector(0, 0, 0);
		}
		else
			LaunchCharacter(FVector(0, 0, climbing_Speed), true, true);
	}
	else
	{
		bIsclimbing = false;
		if (bIsLastTickClimbing == true)
		{
			LaunchCharacter(FVector(0, 0, climbing_projection), true, true);
			bIsLastTickClimbing = false;

		}
		else
		{
			bIsJumping = true;
		}
	}
}

void	ABaseCharacter::wall_run_right()
{
	if (bIsSliding == true)
		return;
	if (dir_linetrace_right == FVector(0, 0, 0))
	{
		dir_linetrace_right = GetActorRightVector();
		origin = GetActorLocation();
	}
	Start = this->GetTransform().GetLocation();
	End = Start + (dir_linetrace_right * 37);
	DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), false, -1, 0, 2.0);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params, ResponseParam)  && bIsJumping == true && ((this->GetVelocity().X != 0) || (this->GetVelocity().Y != 0)) && (distance = FVector::Distance(origin, GetActorLocation())) <= wallrun_max_distance && bResetWallRun == false )
	{
		Desired = Camera->GetComponentRotation();
		if (sweet_roll_right >= -15)
		{
		//	Camera->bUsePawnControlRotation = false;
			bIsCameraRoll = true;
			Desired.Roll += -1;
			sweet_roll_right += -1;
			Camera->SetWorldRotation(Desired);
		}
		//Camera->SetRelativeRotation(Desired);
		bIsCameraRoll = false;
		bIsWallRunning = true;
		//Camera->bUsePawnControlRotation = true;
		if (dir_wall_running == FVector(0, 0, 0))
			dir_wall_running = (GetActorForwardVector() + GetActorRightVector()) * wallrun_speed;
		LaunchCharacter(dir_wall_running, true, true);
		bIsRoll_Right = true;
	}
	else
	{
		bIsWallRunning = false;
		dir_wall_running = FVector(0, 0, 0);
		dir_linetrace_right = FVector(0, 0, 0);
		bIsleft = (true);
		if ((distance = FVector::Distance(origin, GetActorLocation()))> wallrun_max_distance)
			bResetWallRun = true;
		return;
	}
	bIsleft = (false);
}

void	ABaseCharacter::reset_roll_right_cam()
{
	Desired = Camera->GetComponentRotation();
	if (sweet_roll_right != 0)
	{
		bIsCameraRoll = true;
		Desired.Roll += 1;
		sweet_roll_right += 1;
		Camera->SetWorldRotation(Desired);
	}
	else
	{
		bIsleft = (true);
		bIsRoll_Right = false;
		bIsCameraRoll = false;
		return;
	}

}

void	ABaseCharacter::wall_run_left()
{
	if (bIsSliding == true)
		return;
	if (dir_linetrace_left == FVector(0, 0, 0))
	{
		dir_linetrace_left = GetActorRightVector() * -1;
		origin = GetActorLocation();
	}
	Start = this->GetTransform().GetLocation();
	End = Start + (dir_linetrace_left * 37);
	DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), false, -1, 0, 2.0);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params, ResponseParam) && bIsJumping == true && ((this->GetVelocity().X != 0) || (this->GetVelocity().Y != 0)) && (distance = FVector::Distance(origin, GetActorLocation())) <= wallrun_max_distance && bResetWallRun == false)
	{
		Desired = Camera->GetComponentRotation();
		if (sweet_roll_left <= 15)
		{
			bIsCameraRoll = true;
			Desired.Roll += 1;
			sweet_roll_left += 1;
		}
		Camera->SetWorldRotation(Desired);
		bIsWallRunning = true;
		if (dir_wall_running == FVector(0, 0, 0))
			dir_wall_running = (GetActorForwardVector() + (GetActorRightVector()) * -1) * wallrun_speed;
		LaunchCharacter(dir_wall_running, true, true);
		bIsRoll_Left = true;
	}
	else
	{
		bIsWallRunning = false;
		dir_wall_running = FVector(0, 0, 0);
		dir_linetrace_left = FVector(0, 0, 0);
		bIsleft = false;
		if ((distance = FVector::Distance(origin, GetActorLocation()))> wallrun_max_distance)
			bResetWallRun = true;
		return;
	}
	bIsleft = (true);
}

void	ABaseCharacter::reset_roll_left_cam()
{
	Desired = Camera->GetComponentRotation();
	if (sweet_roll_left != 0)
	{
		Desired.Roll += -1;
		sweet_roll_left += -1;
		Camera->SetWorldRotation(Desired);
	}
	else
	{
	//	Camera->bUsePawnControlRotation = true;
	//	Camera->SetWorldRotation(GetControlRotation());
		bIsCameraRoll = false;
		bIsleft = false;
		bIsRoll_Left = false;
		return;
	}
}

void	ABaseCharacter::wall_running()
{

	if (bCanWallRun == true)
	{
		if (bIsleft == true)
			wall_run_left();
		else
			wall_run_right();
	}
	}


void ABaseCharacter::jump_function()
{	

	wall_climbing();
	wall_running();
}



void ABaseCharacter::dashlateral()
{
	if (bIsJumping && bCanDash)
	{
		nb_dash++;
		GetWorld()->GetTimerManager().SetTimer(time, this, &ABaseCharacter::test, 0.1f, false, 0.2f);
		if (nb_dash == 2)
		{
			
			LaunchCharacter((GetActorRightVector() + FVector(0,0,dash_Z)) * dash_speed, true, true);
			bCanDash = false;
		}
	}


}


void ABaseCharacter::back_dash()
{
	if (bIsJumping && bCanDash)
	{
		nb_dash++;
		GetWorld()->GetTimerManager().SetTimer(time, this, &ABaseCharacter::test, 0.1f, false, 0.2f);
		if (nb_dash == 2)
		{
			LaunchCharacter((GetActorForwardVector() + FVector(0, 0, -1 * dash_Z)) * (-1 * dash_speed), true, true);
			bCanDash = false;
		}
	}


}

void ABaseCharacter::left_dash()
{
	if (bIsJumping && bCanDash)
	{
		nb_dash++;
		GetWorld()->GetTimerManager().SetTimer(time, this, &ABaseCharacter::test, 0.1f, false, 0.2f);
		if (nb_dash == 2)
		{
			LaunchCharacter((GetActorRightVector() + FVector(0, 0,-1 * dash_Z)) * (-1 * dash_speed), true, true);
			bCanDash = false;
		}
	}


}

void ABaseCharacter::front_dash()
{
	if (bIsJumping && bCanDash)
	{
		nb_dash++;
		GetWorld()->GetTimerManager().SetTimer(time, this, &ABaseCharacter::test, 0.1f, false, 0.2f);
		if (nb_dash == 2)
		{
			LaunchCharacter((GetActorForwardVector() + FVector(0, 0, dash_Z)) * dash_speed, true, true);
			bCanDash = false;
		}
	}


}

void ABaseCharacter::walljump()
{
	rots = Camera->GetComponentRotation();
	tot = rots.Vector();
	tot.Z = walljump_z;
	if (bCanWallJump == true)
		this->LaunchCharacter(tot * walljump_force, true, true);
	bCanWallJump = false;
}

void	ABaseCharacter::slide()
{
	if (bIsForwardKeyPress == true && bCanSlide == true)
	{
		
		bIsSliding = true;
	}
}

void	ABaseCharacter::unslide()
{
	if (bCanSlide == false || bIsSliding == false)
		return;
	bIsSliding = false;
	bIsBlockMove = true;
	dir_slide = FVector(0,0,0);
	view = this->GetActorRotation();
	view.Pitch = 0;
	this->SetActorRotation(view);
	view = Camera->GetComponentRotation();
	view.Pitch = 0;
	view.Yaw = GetActorRotation().Yaw;
	view.Roll = GetActorRotation().Roll;
	Camera->SetWorldRotation(view);
	bIsBlockMove = false;
	Stop_slide = false;

	Start = this->GetTransform().GetLocation();
	Start.Z = Start.Z + this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 1;
	End = Start;
	End.Z = Start.Z + 2;
	DrawDebugLine(GetWorld(), Start, End, FColor(0, 0, 255), false, -1, 0, 2.0);
	DrawDebugBox(GetWorld(), End, FVector(30, 30, 2), FColor(0, 0, 255), false, -1, 0, 2);
	if (GetWorld()->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CollisionShape, Params, ResponseParam))
	{
		Try_crouch();
		return ;
	}
	if (slide_speed + slowdown <= 0)
	{
		Try_crouch();
		slowdown = 0;
	}
	}


void ABaseCharacter::NotifyActorBeginOverlap(AActor * OtherActor)
{
	bCanWallJump = true;
}

// Function in progress
void ABaseCharacter::test()
{
	nb_dash = 0;
}

void ABaseCharacter::NotifyActorEndOverlap(AActor * OtherActor)
{
	bCanWallJump = false;
}