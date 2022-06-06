// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MazeHUD.generated.h"


class UMazeWidgetBase;


USTRUCT(BlueprintType)
struct FMazeWidgetStruct
{
	GENERATED_BODY()

	UPROPERTY(Category=MazeWidget, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMazeWidgetBase> WidgetClass;
	
	UPROPERTY(Category=MazeWidget, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMazeWidgetBase> Widget;
};


/**
 * 
 */
UCLASS()
class CUBEMAZE_API AMazeHUD : public AHUD
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category=MazeWidget, BlueprintReadOnly, VisibleAnywhere)
	TMap<FString, FMazeWidgetStruct> AllWidgets;

	UPROPERTY(Category=MazeWidget, BlueprintReadOnly, VisibleAnywhere)
	int32 PauseWidgetCount;

	// TODO Create Stack to push/pop widget

protected:

	UFUNCTION(Category=MazeWidget, BlueprintCallable)
	void CheckGamePause();

public:

	UFUNCTION(Category=MazeWidget, BlueprintCallable)
	void BindWidgetClass(const FString& WidgetName, TSubclassOf<UMazeWidgetBase> WidgetClass);

	UFUNCTION(Category=MazeWidget, BlueprintCallable)
	UMazeWidgetBase* GetWidget(const FString& WidgetName, bool bPushWidget=false);

	UFUNCTION(Category=MazeWidget, BlueprintCallable)
	void RemoveWidget(const FString& WidgetName);

	UFUNCTION(Category=MazeWidget, BlueprintCallable)
	void PushWidget(UMazeWidgetBase* Widget);
	
	UFUNCTION(Category=MazeWidget, BlueprintCallable)
	void PopWidget(UMazeWidgetBase* Widget);
};
