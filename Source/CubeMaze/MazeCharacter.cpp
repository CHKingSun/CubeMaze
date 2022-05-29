// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeCharacter.h"

// Sets default values
AMazeCharacter::AMazeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->TurnRate = 45.f;
	this->LookUpRate = 45.f;
}

// Called when the game starts or when spawned
void AMazeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMazeCharacter::MoveForward(float Value)
{
	if (Controller && !FMath::IsNearlyZero(Value))
	{
		FVector Forward = Controller->GetControlRotation().Vector();  // 找到前向
		Forward.Z = 0.f;  // 去掉Z轴的数据
		if (Forward.Normalize())  // 单位化，一般不会失败
		{
			AddMovementInput(Forward, Value);
		}
	}
}

void AMazeCharacter::MoveRight(float Value)
{
	if (Controller && !FMath::IsNearlyZero(Value))
	{
		FVector Forward = Controller->GetControlRotation().Vector();  // 找到前向
		Forward.Z = 0.f;  // 去掉Z轴的数据
		if (Forward.Normalize())  // 单位化，一般不会失败
		{
			AddMovementInput(Forward.RotateAngleAxis(90.f, FVector::ZAxisVector), Value);
		}
	}
}

void AMazeCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * this->TurnRate * GetWorld()->GetDeltaSeconds());
}

void AMazeCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * this->LookUpRate * GetWorld()->GetDeltaSeconds());
}

// Called every frame
void AMazeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMazeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward/Back", this, &AMazeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight/Left", this, &AMazeCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMazeCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnAtRate", this, &AMazeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Lookup", this, &AMazeCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpAtRate", this, &AMazeCharacter::LookUpAtRate);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMazeCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMazeCharacter::StopJumping);
}

