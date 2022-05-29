// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MazeCharacter.generated.h"

UCLASS()
class CUBEMAZE_API AMazeCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category=Input, EditAnywhere)
	float TurnRate;

	UPROPERTY(Category=Input, EditAnywhere)
	float LookUpRate;

public:
	// Sets default values for this character's properties
	AMazeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Value);

	void LookUpAtRate(float Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
