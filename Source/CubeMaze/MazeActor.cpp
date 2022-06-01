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
	
	constexpr float MeshSize = 100.f;
	const FVector SpaceScale(2, 2, 1.5f);
	const FVector WallScale(0.5, 0.5f, 1.5f);
	const FVector SpaceSize = SpaceScale * MeshSize; 
	const FVector WallSize = WallScale * MeshSize; 

	MazeSpaceSize = FVector2D(MazeCol * SpaceSize.X + (MazeCol - 1) * WallSize.X, MazeRow * SpaceSize.Y + (MazeRow - 1) * WallSize.Y);

	// Floor
	constexpr float FloorZScale = 0.5f; 
	MazeFloor->ClearInstances();
	MazeFloor->AddInstance(FTransform(
		FRotator::ZeroRotator,
		FVector(-MazeSpaceSize.X / 2.f, -MazeSpaceSize.Y / 2.f, 0),
		FVector(MazeSpaceSize.X / MeshSize, MazeSpaceSize.Y / MeshSize, FloorZScale)
	));

	// Walls
	MazeWall->ClearInstances();
	FTransform Trans;
	for (auto& SpaceData : (*MazeData))
	{
		switch (SpaceData.DataType)
		{
		case FMazeDataStruct::Space: break;
		case FMazeDataStruct::Wall:
			if (SpaceData.MeshIndex == -2)
			{
				if (SpaceData.X % 2 == 0)
				{
					const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X);
					const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.Y) + SpaceSize.Y; 
					Trans.SetTranslationAndScale3D(FVector(-MazeSpaceSize.X / 2.f + OffsetX, -MazeSpaceSize.Y / 2.f + OffsetY, MeshSize * 0.5f), FVector(SpaceScale.X, WallScale.Y, WallScale.Z));	
				}
				else
				{
					const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X) + SpaceSize.X;
					const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.Y); 
					Trans.SetTranslationAndScale3D(FVector(-MazeSpaceSize.X / 2.f + OffsetX, -MazeSpaceSize.Y / 2.f + OffsetY, MeshSize * 0.5f), FVector(WallScale.X, SpaceScale.Y, WallScale.Z));	
				}

				SpaceData.MeshIndex = MazeWall->AddInstance(Trans);
			}
			break;
		case FMazeDataStruct::Junction:
			if (SpaceData.MeshIndex == -2)
			{
				const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X) + SpaceSize.X;
				const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.Y) + SpaceSize.Y; 
				Trans.SetTranslationAndScale3D(FVector(-MazeSpaceSize.X / 2.f + OffsetX, -MazeSpaceSize.Y / 2.f + OffsetY, MeshSize * 0.5f), WallScale);	

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

void AMazeActor::UpdateSizeAndRandomSeed(int32 MCol, int32 MRow, int32 RSeed)
{
	RandomSeed = RSeed;
	MazeRow = MRow;
	MazeCol = MCol;
	CheckMazeData();
	MazeData->ResetMaze(MazeRow, MazeCol, RandomSeed);

	UpdateMaze();
}
