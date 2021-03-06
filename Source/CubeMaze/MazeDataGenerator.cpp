// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeDataGenerator.h"

#include <algorithm>

UMazeDataGenerator::UMazeDataGenerator()
{
	ResetMaze(12, 12, 1024);
	UE_LOG(LogTemp,Warning,TEXT("MazeData Initialize 0X%p!!!!"), this);
}

void UMazeDataGenerator::FinishDestroy()
{
	UObject::FinishDestroy();
	UE_LOG(LogTemp,Warning,TEXT("MazeData Destroy 0X%p!!!!"), this);
}

void UMazeDataGenerator::ResetMaze(int32 Row, int32 Col, int32 RSeed)
{
	MazeRow = Row;
	MazeCol = Col;
	RowSize = MazeRow * 2 -1;
	ColSize = MazeCol * 2 -1;
	RandomStream.Initialize(RSeed);

	for (int i = 0; i < EMazeDirection::DirectionSize; ++i)
	{
		EdgeEntries[i] = -1;
	}

	// 设置对应的墙和连接点
	MazeData.Empty(RowSize * ColSize);
	for (int y = 0; y < RowSize; ++y)
	{
		for (int x = 0; x < ColSize; ++x)
		{
			if (x % 2 == 1 && y % 2 == 1)  // Junction
			{
				MazeData.Emplace(EMazePlaceType::Junction, x, y, -2);
			} else if (x % 2 == 1 || y % 2 == 1) // Edge
			{
				MazeData.Emplace(EMazePlaceType::Wall, x, y, -2);
			}else
			{
				MazeData.Emplace(EMazePlaceType::Space, x, y, -2);
			}
		}
	}
}

int32 UMazeDataGenerator::GetEdgeEntryByMazeArund(const TObjectPtr<UMazeDataGenerator>& AroundMaze) const
{
	if (AroundMaze == nullptr) return -1;

	for (int32 i = 0; i < EMazeDirection::DirectionSize; ++i)
	{
		// if (AroundMazes[i] == AroundMaze)
		// {
			return EdgeEntries[i];
		// }
	}
		
	return -1;
}

void UMazeDataGenerator::Generate(bool bResetRandomSeed)
{
	if (bResetRandomSeed) RandomStream.Reset();

	TSet<int32> GridSet;
	PrimRecursive(GridSet, MazeCol / 2, MazeRow / 2);
}

int32 UMazeDataGenerator::GenerateEdgeEntry(EMazeDirection::Direction Dir)
{
	if (!EMazeDirection::CheckDirection(Dir)) return -1;
	int32 Index = -1;
	if (Dir == EMazeDirection::Left || Dir == EMazeDirection::Right) Index = (RandomStream.RandHelper(MazeRow) + RandomStream.RandHelper(MazeRow)) / 2;
	else Index = (RandomStream.RandHelper(MazeCol) + RandomStream.RandHelper(MazeCol)) / 2;

	EdgeEntries[Dir] = Index;
	return Index;
}

void UMazeDataGenerator::ResetEdgeEntry()
{
	for (int i = 0; i < EMazeDirection::DirectionSize; ++i)
	{
		EdgeEntries[i] = -1;
	}
}

void UMazeDataGenerator::PrimRecursive(TSet<int32>& GridSet, int32 CurX, int32 CurY, EMazeDirection::Direction FromDir)
{
	if (CurX < 0 || MazeCol <= CurX || CurY < 0 || MazeRow <= CurY) return;
	if (GridSet.Num() == MazeRow * MazeCol || GridSet.Contains(CurX + CurY * MazeCol)) return;

	GridSet.Add(CurX + CurY * MazeCol);

	switch (FromDir)
	{
	case EMazeDirection::Left:
		MazeData[CurX * 2 - 1 + (CurY * 2) * ColSize].MeshIndex = -1;
		break;
	case EMazeDirection::Bottom:
		MazeData[CurX * 2 + (CurY * 2 - 1) * ColSize].MeshIndex = -1;
		break;
	case EMazeDirection::Right:
		MazeData[CurX * 2 + 1 + (CurY * 2) * ColSize].MeshIndex = -1;
		break;
	case EMazeDirection::Top:
		MazeData[CurX * 2 + (CurY * 2 + 1) * ColSize].MeshIndex = -1;
		break;
	default: ;
	}

	EMazeDirection::Direction Dirs[4] = { EMazeDirection::Left, EMazeDirection::Right, EMazeDirection::Top, EMazeDirection::Bottom, };
	for (int i = 0; i < EMazeDirection::DirectionSize; ++i)
	{
		const int32 NewIndex = RandomStream.RandHelper(EMazeDirection::DirectionSize);
		std::swap(Dirs[i], Dirs[NewIndex]);
	}

	for (const auto& Dir : Dirs)
	{
		switch (Dir)
		{
		case EMazeDirection::Left:
			PrimRecursive(GridSet, CurX - 1, CurY, EMazeDirection::Right);
			break;
		case EMazeDirection::Bottom:
			PrimRecursive(GridSet, CurX, CurY - 1, EMazeDirection::Top);
			break;
		case EMazeDirection::Right:
			PrimRecursive(GridSet, CurX + 1, CurY, EMazeDirection::Left);
			break;
		case EMazeDirection::Top:
			PrimRecursive(GridSet, CurX, CurY + 1, EMazeDirection::Bottom);
			break;
		default: ;
		}
	}
}
