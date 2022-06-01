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
	bNeedEdge = true;
	MazeData = NewObject<UMazeDataGenerator>();

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	MazeFloor = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floor"));
	MazeFloor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeWall = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Wall");
	MazeWall->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeEdge = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Edge");
	MazeEdge->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AMazeActor::BeginPlay()
{
	Super::BeginPlay();

}

void AMazeActor::GenerateMaze(bool bResetRandomSeed)
{
	CheckMazeData();
	MazeData->Generate(bResetRandomSeed);

	// 这里先不加上边界的尺寸
	const FVector2D MazeSpaceSize = GetMazeRawSpaceSize();
	const FVector2D CenterOffset = MazeSpaceSize / 2.f;

	// Floor
	MazeFloor->ClearInstances();
	MazeFloor->AddInstance(FTransform(
		FRotator::ZeroRotator,
		FVector(-CenterOffset.X, -CenterOffset.Y, 0),
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
					Trans.SetTranslationAndScale3D(FVector(-CenterOffset.X + OffsetX, -CenterOffset.Y + OffsetY, MeshSize * 0.5f), FVector(SpaceScale.X, WallScale.Y, WallScale.Z));	
				}
				else
				{
					const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X) + SpaceSize.X;
					const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.Y); 
					Trans.SetTranslationAndScale3D(FVector(-CenterOffset.X + OffsetX, -CenterOffset.Y + OffsetY, MeshSize * 0.5f), FVector(WallScale.X, SpaceScale.Y, WallScale.Z));	
				}

				SpaceData.MeshIndex = MazeWall->AddInstance(Trans);
			}
			break;
		case FMazeDataStruct::Junction:
			if (SpaceData.MeshIndex == -2)
			{
				const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X) + SpaceSize.X;
				const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.Y) + SpaceSize.Y; 
				Trans.SetTranslationAndScale3D(FVector(-CenterOffset.X + OffsetX, -CenterOffset.Y + OffsetY, MeshSize * 0.5f), WallScale);	

				SpaceData.MeshIndex = MazeWall->AddInstance(Trans);
			}
			break;
		default: ;
		}
	}

	// Edge
	UpdateMazeEdge(bNeedEdge);
}

void AMazeActor::UpdateMazeEdge(bool bNEdge)
{
	bNeedEdge = bNEdge;
	MazeEdge->ClearInstances();
	if (!bNeedEdge) return;

	FTransform Trans;
	FVector2D MazeSpaceSize = GetMazeRawSpaceSize();
	const FVector2D CenterOffset = MazeSpaceSize / 2.f;

	auto UpdateMazeEdge = [&](int32 InEntry, const FVector& InStartLoc, int32 InDir)
	{
		const int32 SideLength = InDir == 1 ? MazeRow : MazeCol;  // 也可以把MazeRow和MazeCol做成FIntVector2D
		FVector CurScale(EdgeScale);
		if (InEntry == -1)
		{
			CurScale[InDir] = MazeSpaceSize[InDir] / MeshSize;
			Trans.SetTranslationAndScale3D(InStartLoc, CurScale);
			MazeEdge->AddInstance(Trans);
		} else
		{
			FVector Movement(0.f);
			CurScale[InDir] = (WallScale[InDir] + SpaceScale[InDir]) * InEntry;
			Trans.SetTranslationAndScale3D(InStartLoc, CurScale);
			MazeEdge->AddInstance(Trans);
			Movement[InDir] = CurScale[InDir] * MeshSize;
			Trans.AddToTranslation(Movement);
			CurScale[InDir] = SpaceScale[InDir];
			CurScale.Z = FloorZScale;
			Trans.SetScale3D(CurScale);
			MazeEdge->AddInstance(Trans);
			Movement[InDir] = SpaceScale[InDir] * MeshSize;
			Trans.AddToTranslation(Movement);
			CurScale.Z = EdgeScale.Z;
			CurScale[InDir] = (WallScale[InDir] + SpaceScale[InDir]) * (SideLength - InEntry - 1);
			Trans.SetScale3D(CurScale);
			MazeEdge->AddInstance(Trans);
		}
	};
	
	int32 Entry = MazeData->GetEdgeEntry(UMazeDataGenerator::Left);
	UpdateMazeEdge(Entry, FVector(-CenterOffset.X - EdgeSize.X, -CenterOffset.Y, 0.f), 1);
	Entry = MazeData->GetEdgeEntry(UMazeDataGenerator::Right);
	UpdateMazeEdge(Entry, FVector(CenterOffset.X, -CenterOffset.Y, 0.f), 1);
	Entry = MazeData->GetEdgeEntry(UMazeDataGenerator::Bottom);
	UpdateMazeEdge(Entry, FVector(-CenterOffset.X, -CenterOffset.Y - EdgeSize.Y, 0.f), 0);
	Entry = MazeData->GetEdgeEntry(UMazeDataGenerator::Top);
	UpdateMazeEdge(Entry, FVector(-CenterOffset.X, CenterOffset.Y, 0.f), 0);

	// Corner
	Trans.SetScale3D(EdgeScale);
	Trans.SetLocation(FVector(-CenterOffset.X - EdgeSize.X, -CenterOffset.Y - EdgeSize.Y, 0.f));
	MazeEdge->AddInstance(Trans);
	Trans.SetLocation(FVector(CenterOffset.X, -CenterOffset.Y - EdgeSize.Y, 0.f));
	MazeEdge->AddInstance(Trans);
	Trans.SetLocation(FVector(CenterOffset.X, CenterOffset.Y, 0.f));
	MazeEdge->AddInstance(Trans);
	Trans.SetLocation(FVector(-CenterOffset.X - EdgeSize.X, CenterOffset.Y , 0.f));
	MazeEdge->AddInstance(Trans);
}

FVector2D AMazeActor::GetMazeRawSpaceSize() const
{
	return FVector2D(MazeCol * SpaceSize.X + (MazeCol - 1) * WallSize.X, MazeRow * SpaceSize.Y + (MazeRow - 1) * WallSize.Y);
}

FVector2D AMazeActor::GetMazeSpaceSize() const
{
	FVector2D Ret = GetMazeRawSpaceSize();
	if (bNeedEdge) Ret += FVector2D(EdgeSize.X * 2, EdgeSize.Y * 2);

	return Ret;
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

void AMazeActor::SetSizeAndRandomSeed(int32 MCol, int32 MRow, int32 RSeed)
{
	RandomSeed = RSeed;
	MazeRow = MRow;
	MazeCol = MCol;
	CheckMazeData();
	MazeData->ResetMaze(MazeRow, MazeCol, RandomSeed);
}
