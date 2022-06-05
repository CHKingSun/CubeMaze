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
	int32 RandomSeed;
	
	UPROPERTY(Category=Maze, EditAnywhere, BlueprintReadOnly)
	int32 MazeCol = 12;
	UPROPERTY(Category=Maze, EditAnywhere, BlueprintReadOnly)
	int32 MazeRow = 12;

	UPROPERTY(Category=Maze, EditAnywhere, BlueprintReadWrite)
	bool bNeedEdge;

	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInstancedStaticMeshComponent> MazeFloor;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInstancedStaticMeshComponent> MazeWall;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInstancedStaticMeshComponent> MazeEdge;

	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> EntryLeft;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> EntryRight;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> EntryTop;
	UPROPERTY(Category=Maze, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> EntryBottom;

	// 保存每块墙的实例的index
	// -2: 需要创建墙，但是还没创建
	// -1: 不需要墙，留空
	// 0+: 已经创建完成，值为对应墙的实例的index
	UPROPERTY(Category=Maze, BlueprintReadOnly)
	TObjectPtr<class UMazeDataGenerator> MazeData;

	TObjectPtr<class APortalActor> EntryActors[4];

	// 这些是默认尺寸
	const float MeshSize = 100.f;
	const float FloorZScale = 0.5f;
	const FVector SpaceScale = FVector(2, 2, 1.5f);
	const FVector WallScale = FVector(0.5, 0.5f, 1.5f);
	const FVector SpaceSize = SpaceScale * MeshSize; 
	const FVector WallSize = WallScale * MeshSize;
	const FVector EdgeScale = FVector(1.f, 1.f, WallScale.Z + FloorZScale);
	const FVector EdgeSize = EdgeScale * MeshSize;
	
public:	
	// Sets default values for this actor's properties
	AMazeActor();

	virtual void FinishDestroy() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Category=Maze, BlueprintCallable)
	void CheckMazeData();

	bool CheckChildActor();

	UFUNCTION()
	void OnEntryBeginOvelap(APortalActor* OverlappedActor, AActor* OtherActor);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category=Maze, BlueprintCallable)
	void SetSizeAndRandomSeed(int32 MCol, int32 MRow, int32 RSeed);

	UFUNCTION(Category=Maze, BlueprintCallable)
	void GenerateMaze(bool bResetRandomSeed = true);

	void UpdateMazeEdge(bool bNEdge = true);

	const TObjectPtr<UMazeDataGenerator>& GetMazeData()const { return MazeData; }

	FVector2D GetMazeRawSpaceSize()const;
	FVector2D GetMazeSpaceSize()const;
};
