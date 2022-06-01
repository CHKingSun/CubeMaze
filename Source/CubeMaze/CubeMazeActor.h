// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeMazeActor.generated.h"

class AMazeActor;

UCLASS()
class CUBEMAZE_API ACubeMazeActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category=Maze, EditAnywhere, BlueprintReadOnly)
	FRandomStream RandomStream;

	UPROPERTY(Category=Maze, EditAnywhere, BlueprintReadOnly)
	FIntVector MazeSize;

	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> MazeBottom;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> MazeTop;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> MazeFront;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> MazeBack;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> MazeLeft;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> MazeRight;

	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInstancedStaticMeshComponent> MazeEdge;

	TObjectPtr<AMazeActor> ActorBottom;
	TObjectPtr<AMazeActor> ActorTop;
	TObjectPtr<AMazeActor> ActorFront;
	TObjectPtr<AMazeActor> ActorBack;
	TObjectPtr<AMazeActor> ActorLeft;
	TObjectPtr<AMazeActor> ActorRight;
	
public:	
	// Sets default values for this actor's properties
	ACubeMazeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Category=Maze, BlueprintCallable)
	void GenerateCubeMaze(bool bResetRandomSeed = true);

	bool CheckChildActor();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
