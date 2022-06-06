// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MazeWidgetBase.h"
#include "ControlWidget.generated.h"

/**
 * 
 */
UCLASS()
class CUBEMAZE_API UControlWidget : public UMazeWidgetBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category=Control, EditAnywhere, BlueprintReadWrite)
	float TurnRate = 120.f;

	UPROPERTY(Category=Control, EditAnywhere, BlueprintReadWrite)
	float RestrictRadius = 180.f;

	TObjectPtr<class UBorder> BorderMove;

	FVector2D MoveDelta;
	FVector2D MoveCenter;

	int32 ViewIndex = -1;
	int32 MoveIndex = -1;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	FEventReply OnBorderMoveTouchStartedCall(FGeometry InGeometry, const FPointerEvent& InGestureEvent );
	UFUNCTION(Category=Control, BlueprintCallable)
	FEventReply OnTouchStartedCall(FGeometry InGeometry, const FPointerEvent& InGestureEvent );
	UFUNCTION(Category=Control, BlueprintCallable)
	FEventReply OnTouchMovedCall(FGeometry InGeometry, const FPointerEvent& InGestureEvent );
	UFUNCTION(Category=Control, BlueprintCallable)
	FEventReply OnTouchEndedCall(FGeometry InGeometry, const FPointerEvent& InGestureEvent );

	UFUNCTION(Category=Control, BlueprintCallable)
	void OnBtnJumpCall()const;

	UFUNCTION(Category=Control, BlueprintCallable)
	void AttachMoveWidget(UBorder* Widget);

	UFUNCTION(Category=Control, BlueprintCallable)
	void ResetControl();
};
