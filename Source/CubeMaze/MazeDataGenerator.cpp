// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeDataGenerator.h"

#include <algorithm>

UMazeDataGenerator::UMazeDataGenerator()
{
	ResetMaze(12, 12, 1024);	
}

void UMazeDataGenerator::ResetMaze(int32 Row, int32 Col, int32 RSeed)
{
	MazeRow = Row;
	MazeCol = Col;
	RowSize = MazeRow * 2 -1;
	ColSize = MazeCol * 2 -1;
	RandomStream.Initialize(RSeed);

	for (int i = 0; i < Direction::DirectionSize; ++i)
	{
		EdgeEntries[i] = -1;
		AroundMazes[i] = nullptr;
	}

	// 设置对应的墙和连接点
	MazeData.Empty(RowSize * ColSize);
	for (int y = 0; y < ColSize; ++y)
	{
		for (int x = 0; x < RowSize; ++x)
		{
			if (x % 2 == 1 && y % 2 == 1)  // Junction
			{
				MazeData.Emplace(FMazeDataStruct::Junction, x, y, -2);
			} else if (x % 2 == 1 || y % 2 == 1) // Edge
			{
				MazeData.Emplace(FMazeDataStruct::Edge, x, y, -2);
			}else
			{
				MazeData.Emplace(FMazeDataStruct::Space, x, y, -2);
			}
		}
	}
}

void UMazeDataGenerator::Generate(bool bResetRandomSeed)
{
	if (bResetRandomSeed) RandomStream.Reset();

	TSet<int32> GridSet;
	PrimRecursive(GridSet, MazeRow / 2, MazeCol / 2);
}

void UMazeDataGenerator::PrimRecursive(TSet<int32>& GridSet, int32 CurX, int32 CurY, Direction FromDir)
{
	if (CurX < 0 || MazeRow <= CurX || CurY < 0 || MazeCol <= CurY) return;
	if (GridSet.Num() == MazeRow * MazeCol || GridSet.Contains(CurX + CurY * MazeRow)) return;

	GridSet.Add(CurX + CurY * MazeRow);

	switch (FromDir)
	{
	case Left:
		MazeData[CurX * 2 - 1 + (CurY * 2) * RowSize].MeshIndex = -1;
		break;
	case Bottom:
		MazeData[CurX * 2 + (CurY * 2 - 1) * RowSize].MeshIndex = -1;
		break;
	case Right:
		MazeData[CurX * 2 + 1 + (CurY * 2) * RowSize].MeshIndex = -1;
		break;
	case Top:
		MazeData[CurX * 2 + (CurY * 2 + 1) * RowSize].MeshIndex = -1;
		break;
	default: ;
	}

	Direction Dirs[4] = { Left, Right, Top, Bottom, };
	for (int i = 0; i < DirectionSize; ++i)
	{
		const int32 NewIndex = RandomStream.RandHelper(DirectionSize);
		std::swap(Dirs[i], Dirs[NewIndex]);
	}

	for (const auto& Dir : Dirs)
	{
		switch (Dir)
		{
		case Left:
			PrimRecursive(GridSet, CurX - 1, CurY, Right);
			break;
		case Bottom:
			PrimRecursive(GridSet, CurX, CurY - 1, Top);
			break;
		case Right:
			PrimRecursive(GridSet, CurX + 1, CurY, Left);
			break;
		case Top:
			PrimRecursive(GridSet, CurX, CurY + 1, Bottom);
			break;
		default: ;
		}
	}
}
