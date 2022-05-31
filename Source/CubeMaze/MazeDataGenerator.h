// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MazeDataGenerator.generated.h"

USTRUCT(BlueprintType)
struct FMazeDataStruct
{
	GENERATED_BODY()

	enum PlaceType
	{
		Space,  // 空地
		Edge,  // 墙
		Junction, // 连接点
	};
	
	enum PlaceState
	{
		NotUsed,  // 不使用，即留空
		NotInitialized,  // 使用，但是还没有初始化模型
		Initialized,  // 使用，并且已经初始化模型
	};

	PlaceType DataType;
	PlaceState State;
	int32 X;
	int32 Y;
	int32 MeshIndex;

	FMazeDataStruct() : DataType(Space), State(NotUsed), X(0), Y(0), MeshIndex(-1) {}
	FMazeDataStruct(PlaceType DataType, PlaceState State, int32 X, int32 Y, int32 MeshIndex) : DataType(DataType), State(State), X(X), Y(Y), MeshIndex(MeshIndex) {}
};


/**
 * 
 */
UCLASS(BlueprintType)
class CUBEMAZE_API UMazeDataGenerator : public UObject
{
	GENERATED_BODY()

public:
	enum Direction
	{
		Left = 0,
		Bottom = 1,
		Right = 2,
		Top = 3,
		DirectionSize,
	};
	
	static bool CheckDirection(Direction Dir) { return Dir < Direction::DirectionSize; }

protected:
	FRandomStream RandomStream;
	
	int32 MazeRow;
	int32 MazeCol;

	int32 RowSize;
	int32 ColSize;
	
	TArray<FMazeDataStruct> MazeData;

	// l, b, r, t
	int32 EdgeEntries[Direction::DirectionSize];
	TObjectPtr<UMazeDataGenerator> AroundMazes[Direction::DirectionSize];

public:
	UMazeDataGenerator();

	// const FMazeDataStruct& operator[](int32 index)const { return MazeData[index]; }  // check交给TArray做
	// FMazeDataStruct& operator[](int32 index) { return MazeData[index]; }  // check交给TArray做
	//
	// const FMazeDataStruct& operator()(int32 x, int32 y)const { return MazeData[x + y * RowSize]; }  // check交给TArray做
	// FMazeDataStruct& operator()(int32 x, int32 y) { return MazeData[x + y * RowSize]; }  // check交给TArray做

	TArray<FMazeDataStruct>::RangedForIteratorType begin() { return MazeData.begin(); }
	TArray<FMazeDataStruct>::RangedForConstIteratorType begin()const { return MazeData.begin(); }
	TArray<FMazeDataStruct>::RangedForIteratorType end() { return MazeData.end(); }
	TArray<FMazeDataStruct>::RangedForConstIteratorType end()const { return MazeData.end(); }
	
	TObjectPtr<UMazeDataGenerator> GetMazeAround(Direction Dir)const { return CheckDirection(Dir) ? AroundMazes[Dir] : nullptr; }

	int32 GetEdgeEntry(Direction Dir)const { return CheckDirection(Dir) ? EdgeEntries[Dir] : -1; }

	void SetEdgeEntry(Direction Dir, int32 EntryIndex) { if (CheckDirection(Dir)) { EdgeEntries[Dir] = EntryIndex; } }
	
	void SetMazeAround(Direction Dir, const TObjectPtr<UMazeDataGenerator>& AroundMaze) { if (CheckDirection(Dir)) { AroundMazes[Dir] = AroundMaze; } }
	
	void ResetMaze(int32 Row, int32 Col, int32 RSeed);

	bool Generate(bool bResetRandomSeed = true);
};
