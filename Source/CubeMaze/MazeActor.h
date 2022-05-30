// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeActor.generated.h"

UCLASS()
class CUBEMAZE_API AMazeActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category=Maze, EditAnywhere, BlueprintReadOnly)
	FRandomStream RandomStream;
	
	UPROPERTY(Category=Maze, EditAnywhere, BlueprintReadOnly)
	int32 MazeSize = 12;

	UPROPERTY(Category=Maze, VisibleDefaultsOnly, BlueprintReadOnly)
	float CenterOffset;

	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInstancedStaticMeshComponent> MazeFloor;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInstancedStaticMeshComponent> MazeWall;

	// 保存每块墙的实例的index
	// -2: 需要创建墙，但是还没创建
	// -1: 不需要墙，留空
	// 0+: 已经创建完成，值为对应墙的实例的index
	UPROPERTY(Category=Maze, BlueprintReadOnly)
	TArray<int32> MazeWallIndices;
	
public:	
	// Sets default values for this actor's properties
	AMazeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void GenerateMaze();

	UFUNCTION(Category=Maze, BlueprintCallable)
	void UpdateMaze(bool bResetRandomSeed = true);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category=Maze, BlueprintCallable)
	void UpdateSizeAndRandomStream(int32 MSize, int32 RamdomSeed);
};
