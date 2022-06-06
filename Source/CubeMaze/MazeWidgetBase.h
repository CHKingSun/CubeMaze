// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MazeWidgetBase.generated.h"


UENUM(BlueprintType)
enum class EWidgetState : uint8
{
	Pop UMETA(DisplayName = Pop),
	Push UMETA(DisplayName = Push),
};


/**
 * 
 */
UCLASS()
class CUBEMAZE_API UMazeWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(Category=MazeWidget, EditAnywhere, BlueprintReadWrite)
	bool bNeedPause;

	UPROPERTY(Category=MazeWidget, EditAnywhere, BlueprintReadWrite)
	bool bNeedMouse;

	UPROPERTY(Category=MazeWidget, BlueprintReadOnly, VisibleAnywhere)
	EWidgetState WidgetState;

public:

	UFUNCTION(Category=MazeWidget, BlueprintGetter)
	bool IsWigetNeedPause()const { return bNeedPause; }
	
	UFUNCTION(Category=MazeWidget, BlueprintGetter)
	bool IsWigetNeedMouse()const { return bNeedMouse; }

	UFUNCTION(Category=MazeWidget, BlueprintGetter)
	EWidgetState GetWidgetState()const { return WidgetState; }

	// 或许还需要应该Push/Pop的状态数据，但目前这样是OK的
	UFUNCTION(Category=MazeWidget, BlueprintCallable)
	virtual void OnEnterWidget() { WidgetState = EWidgetState::Push; SetVisibility(ESlateVisibility::Visible); }
	
	UFUNCTION(Category=MazeWidget, BlueprintCallable)
	virtual void OnExitWidget() { SetVisibility(ESlateVisibility::Hidden); WidgetState = EWidgetState::Pop; }
};
