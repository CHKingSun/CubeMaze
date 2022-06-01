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
		Wall,  // 墙
		Junction, // 连接点
	};

	PlaceType DataType;
	int32 X;
	int32 Y;
	int32 MeshIndex;  // -1表示不使用，-2表示需要使用，但是没有初始化，0+表示对应实例的index

	FMazeDataStruct() : DataType(Space), X(0), Y(0), MeshIndex(-1) {}
	FMazeDataStruct(PlaceType DataType, int32 X, int32 Y, int32 MeshIndex) : DataType(DataType), X(X), Y(Y), MeshIndex(MeshIndex) {}
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

	TArray<FMazeDataStruct>::RangedForIteratorType begin() { return MazeData.begin(); }
	TArray<FMazeDataStruct>::RangedForConstIteratorType begin()const { return MazeData.begin(); }
	TArray<FMazeDataStruct>::RangedForIteratorType end() { return MazeData.end(); }
	TArray<FMazeDataStruct>::RangedForConstIteratorType end()const { return MazeData.end(); }
	
	TObjectPtr<UMazeDataGenerator> GetMazeAround(Direction Dir)const { return CheckDirection(Dir) ? AroundMazes[Dir] : nullptr; }

	int32 GetEdgeEntry(Direction Dir)const { return CheckDirection(Dir) ? EdgeEntries[Dir] : -1; }

	void SetEdgeEntry(Direction Dir, int32 EntryIndex) { if (CheckDirection(Dir)) { EdgeEntries[Dir] = EntryIndex; } }
	
	void SetMazeAround(Direction Dir, const TObjectPtr<UMazeDataGenerator>& AroundMaze) { if (CheckDirection(Dir)) { AroundMazes[Dir] = AroundMaze; } }
	
	void ResetMaze(int32 Row, int32 Col, int32 RSeed);

	int32 GetEdgeEntryByMazeArund(const TObjectPtr<UMazeDataGenerator>& AroundMaze)const;
	
	void Generate(bool bResetRandomSeed = true);

	void GenerateEdgeEntry();

	void ResetEdgeEntry();
	
protected:

	// FromDir: 从哪个方向过来。如果是DirectionSize，则表示不从任何方向过来，初始化时使用。
	void PrimRecursive(TSet<int32>& GridSet, int32 CurX, int32 CurY, Direction FromDir=DirectionSize);
};
