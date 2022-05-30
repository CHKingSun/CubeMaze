// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeActor.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AMazeActor::AMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Default Value
	RandomStream.Initialize(1024);
	MazeSize = 12;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	MazeFloor = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floor"));
	MazeFloor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeWall = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Wall");
	MazeWall->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AMazeActor::BeginPlay()
{
	Super::BeginPlay();

}

void AMazeActor::GenerateMaze()
{
	const int SideLength = MazeSize * 2 - 1;
	// It will clear the maze.
	MazeWallIndices.Init(-1, SideLength * SideLength);

	for (int x = 0; x < SideLength; ++x)
	{
		for (int y = 0; y < SideLength; ++y)
		{
			MazeWallIndices[x + y * SideLength] = RandomStream.GetUnsignedInt() % 3 == 1 ? -2 : -1;
		}
	}
}

void AMazeActor::UpdateMaze(bool bResetRandomSeed)
{
	if (bResetRandomSeed) RandomStream.Reset();
	GenerateMaze();
	
	const float MeshSize = 100.f;
	const float SideLength = MazeSize * 2 - 1;
	CenterOffset = SideLength * MeshSize / 2.f;

	// Floor
	MazeFloor->ClearInstances();
	MazeFloor->AddInstance(FTransform(
		FRotator::ZeroRotator,
		FVector(-CenterOffset, -CenterOffset, 0),
		FVector(SideLength, SideLength, 0.5f)
	));

	// Walls
	MazeWall->ClearInstances();
	auto MIter = MazeWallIndices.begin();
	FTransform Trans;
	for (int x = 0; x < SideLength; ++x)
	{
		for (int y = 0; y < SideLength; ++y)
		{
			if (*MIter == -2)
			{
				Trans.SetTranslationAndScale3D(
					FVector(-CenterOffset + x * MeshSize, -CenterOffset + y * MeshSize, MeshSize * 0.5f),
					FVector(1.f, 1.f, 1.f));
				*MIter = MazeWall->AddInstance(Trans);
			}
			++MIter;
		}
	} 
}

// Called every frame
void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeActor::UpdateSizeAndRandomStream(int32 MSize, int32 RandomSeed)
{
	RandomStream.Initialize(RandomSeed);
	MazeSize = MSize;

	UpdateMaze();
}
