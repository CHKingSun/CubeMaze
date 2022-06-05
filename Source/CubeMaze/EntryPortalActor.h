// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalActor.h"
#include "MazeDataGenerator.h"
#include "EntryPortalActor.generated.h"

/**
 * 
 */
UCLASS()
class CUBEMAZE_API AEntryPortalActor : public APortalActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category=Portal, BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte<EMazeDirection::Direction> EntryDir = EMazeDirection::DirectionSize;

	UPROPERTY(Category=Maze, BlueprintReadOnly, EditDefaultsOnly)
	int32 EntryPos = -1;

public:

	void SetEntryDirection(EMazeDirection::Direction Dir) { EntryDir = Dir; }

	void SetEntryPosition(int32 Pos) { EntryPos = Pos; }

	EMazeDirection::Direction GetEntryDirection()const { return EntryDir; }

	int32 GetBindPortalEntry()const;
};
