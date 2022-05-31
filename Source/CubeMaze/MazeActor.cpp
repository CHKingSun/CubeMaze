// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeActor.h"

#include "MazeDataGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AMazeActor::AMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Default Value
	RandomSeed = 1024;
	MazeRow = 12;
	MazeCol = 12;
	MazeData = NewObject<UMazeDataGenerator>();

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

void AMazeActor::UpdateMaze(bool bResetRandomSeed)
{
	CheckMazeData();
	MazeData->Generate(bResetRandomSeed);

	const FVector2D SpaceSize(200, 200);
	const FVector2D WallSize(100, 200);
	const FVector2D MeshSize(100, 100);

	MazeSpaceSize = FVector2D(MazeRow * SpaceSize.X + (MazeRow - 1) * WallSize.X, MazeCol * SpaceSize.X + (MazeCol - 1) * WallSize.X);

	// Floor
	MazeFloor->ClearInstances();
	MazeFloor->AddInstance(FTransform(
		FRotator::ZeroRotator,
		FVector(-MazeSpaceSize.X / 2.f, -MazeSpaceSize.Y / 2.f, 0),
		FVector(MazeSpaceSize.X / MeshSize.X, MazeSpaceSize.Y / MeshSize.Y, 0.5f)
	));

	// Walls
	MazeWall->ClearInstances();
	FTransform Trans;
	constexpr float ZScale = 1.5f; 
	for (auto& SpaceData : (*MazeData))
	{
		switch (SpaceData.DataType)
		{
		case FMazeDataStruct::Space: break;
		case FMazeDataStruct::Edge:
			if (SpaceData.MeshIndex == -2)
			{
				if (SpaceData.X % 2 == 0)
				{
					const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X);
					const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.X) + SpaceSize.Y; 
					Trans.SetTranslationAndScale3D(FVector(-MazeSpaceSize.X / 2.f + OffsetX, -MazeSpaceSize.Y / 2.f + OffsetY, MeshSize.X * 0.5f) ,FVector(2.f, 1.f, ZScale));	
				}
				else
				{
					const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X) + SpaceSize.X;
					const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.X); 
					Trans.SetTranslationAndScale3D(FVector(-MazeSpaceSize.X / 2.f + OffsetX, -MazeSpaceSize.Y / 2.f + OffsetY, MeshSize.X * 0.5f) ,FVector(1.f, 2.f, ZScale));	
				}

				SpaceData.MeshIndex = MazeWall->AddInstance(Trans);
			}
			break;
		case FMazeDataStruct::Junction:
			if (SpaceData.MeshIndex == -2)
			{
				const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X) + SpaceSize.X;
				const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.X) + SpaceSize.Y; 
				Trans.SetTranslationAndScale3D(FVector(-MazeSpaceSize.X / 2.f + OffsetX, -MazeSpaceSize.Y / 2.f + OffsetY, MeshSize.X * 0.5f) ,FVector(1.f, 1.f, ZScale));	

				SpaceData.MeshIndex = MazeWall->AddInstance(Trans);
			}
			break;
		default: ;
		}
	}
}

void AMazeActor::CheckMazeData()
{
	if (MazeData == nullptr)
	{
		MazeData = NewObject<UMazeDataGenerator>();
		MazeData->ResetMaze(MazeRow, MazeCol, RandomSeed);
	}
}

// Called every frame
void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeActor::UpdateSizeAndRandomSeed(int32 MRow, int32 MCol, int32 RSeed)
{
	RandomSeed = RSeed;
	MazeRow = MRow;
	MazeCol = MCol;
	CheckMazeData();
	MazeData->ResetMaze(MazeRow, MazeCol, RandomSeed);

	UpdateMaze();
}
