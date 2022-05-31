// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeMazeActor.generated.h"

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
	
public:	
	// Sets default values for this actor's properties
	ACubeMazeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Category=Maze, BlueprintCallable)
	void UpdateCubeMaze(bool bResetRandomSeed = true);

	FVector2D InitializeChildMaze(const TObjectPtr<UChildActorComponent> Child, int32 MRow, int32 MCol) const;

	bool CheckChildActor()const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
