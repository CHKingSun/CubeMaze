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

	UFUNCTION(Category=MazeWidget, BlueprintNativeEvent)
	void OnEnterWidget();
	virtual void OnEnterWidget_Implementation() { WidgetState = EWidgetState::Push; SetVisibility(ESlateVisibility::Visible); }
	
	UFUNCTION(Category=MazeWidget, BlueprintNativeEvent)
	void OnExitWidget();
	virtual void OnExitWidget_Implementation()  { SetVisibility(ESlateVisibility::Hidden); WidgetState = EWidgetState::Pop; }
};
