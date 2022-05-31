// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeDataGenerator.h"

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
			if (x % 2 == 1 && y % 2 == 1)
			{
				MazeData.Emplace(FMazeDataStruct::Junction, FMazeDataStruct::NotInitialized, x, y, -1);
			} else if (x % 2 == 1 || y % 2 == 1)
			{
				MazeData.Emplace(FMazeDataStruct::Edge, FMazeDataStruct::NotInitialized, x, y, -1);
			}else
			{
				MazeData.Emplace(FMazeDataStruct::Space, FMazeDataStruct::NotInitialized, x, y, -1);
			}
		}
	}
}

bool UMazeDataGenerator::Generate(bool bResetRandomSeed)
{
	if (bResetRandomSeed) RandomStream.Reset();

	for (auto& SpaceData : MazeData)
	{
		switch (SpaceData.DataType)
		{
		case FMazeDataStruct::Space:
			SpaceData.State = FMazeDataStruct::Initialized;
			break;
		case FMazeDataStruct::Edge:
			SpaceData.State = FMazeDataStruct::NotInitialized;
			break;
		case FMazeDataStruct::Junction:
			SpaceData.State = FMazeDataStruct::NotInitialized;
			break;
		default: ;
		}
	}

	return true;
}
