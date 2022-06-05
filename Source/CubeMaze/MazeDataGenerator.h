// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MazeDataGenerator.generated.h"

UENUM(BlueprintType)
namespace EMazePlaceType
{
	enum Type
	{
		Space UMETA(DisplayName = "Space"),  // 空地
		Wall UMETA(DisplayName = "Wall"),  // 墙
		Junction UMETA(DisplayName = "Junction"), // 连接点
	};
}


UENUM(BlueprintType)
namespace EMazeDirection
{
	enum Direction
	{
		Left = 0 UMETA(DisplayName = "Left"),
		Bottom = 1 UMETA(DisplayName = "Bottom"),
		Right = 2 UMETA(DisplayName = "Right"),
		Top = 3 UMETA(DisplayName = "Top"),
		DirectionSize UMETA(DisplayName = "DirectionSize"),
	};

}

namespace EMazeDirection
{
	static constexpr Direction Directions[DirectionSize] = { Left, Bottom, Right, Top, };
	
	inline bool CheckDirection(Direction Dir) { return Dir < DirectionSize; }
}


USTRUCT(BlueprintType)
struct FMazeDataStruct
{
	GENERATED_BODY()

	UPROPERTY(Category=Maze, BlueprintReadWrite, EditAnywhere, DisplayName="DataType")
	TEnumAsByte<EMazePlaceType::Type> DataType;
	UPROPERTY(Category=Maze, BlueprintReadWrite, EditAnywhere, DisplayName="X")
	int32 X;
	UPROPERTY(Category=Maze, BlueprintReadWrite, EditAnywhere, DisplayName="Y")
	int32 Y;
	UPROPERTY(Category=Maze, BlueprintReadWrite, EditAnywhere, DisplayName="MeshIndex")
	int32 MeshIndex;  // -1表示不使用，-2表示需要使用，但是没有初始化，0+表示对应实例的index

	FMazeDataStruct() : DataType(EMazePlaceType::Space), X(0), Y(0), MeshIndex(-1) {}
	FMazeDataStruct(EMazePlaceType::Type DataType, int32 X, int32 Y, int32 MeshIndex) : DataType(DataType), X(X), Y(Y), MeshIndex(MeshIndex) {}
};


/**
 * 
 */
UCLASS(BlueprintType)
class CUBEMAZE_API UMazeDataGenerator : public UObject
{
	GENERATED_BODY()

protected:
	FRandomStream RandomStream;
	
	int32 MazeRow;
	int32 MazeCol;

	int32 RowSize;
	int32 ColSize;
	
	TArray<FMazeDataStruct> MazeData;

	// l, b, r, t
	int32 EdgeEntries[EMazeDirection::DirectionSize];

public:
	UMazeDataGenerator();
	virtual void FinishDestroy() override;

	TArray<FMazeDataStruct>::RangedForIteratorType begin() { return MazeData.begin(); }
	TArray<FMazeDataStruct>::RangedForConstIteratorType begin()const { return MazeData.begin(); }
	TArray<FMazeDataStruct>::RangedForIteratorType end() { return MazeData.end(); }
	TArray<FMazeDataStruct>::RangedForConstIteratorType end()const { return MazeData.end(); }

	int32 GetEdgeEntry(EMazeDirection::Direction Dir)const { return CheckDirection(Dir) ? EdgeEntries[Dir] : -1; }

	void SetEdgeEntry(EMazeDirection::Direction Dir, int32 EntryIndex) { if (CheckDirection(Dir)) { EdgeEntries[Dir] = EntryIndex; } }
	
	void ResetMaze(int32 Row, int32 Col, int32 RSeed);

	int32 GetEdgeEntryByMazeArund(const TObjectPtr<UMazeDataGenerator>& AroundMaze)const;
	
	void Generate(bool bResetRandomSeed = true);

	int32 GenerateEdgeEntry(EMazeDirection::Direction Dir);

	void ResetEdgeEntry();
	
protected:

	// FromDir: 从哪个方向过来。如果是DirectionSize，则表示不从任何方向过来，初始化时使用。
	void PrimRecursive(TSet<int32>& GridSet, int32 CurX, int32 CurY, EMazeDirection::Direction FromDir=EMazeDirection::DirectionSize);
};
