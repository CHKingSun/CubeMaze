// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlWidget.h"
#include "Components/Border.h"
#include "MazeCharacter.h"
#include "Kismet/GameplayStatics.h"


void UControlWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()) || MoveIndex == -1)
		return;
	if (const auto Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
		if (const auto Character = Cast<AMazeCharacter>(Player)) {
			if (BorderMove) BorderMove->SetRenderTranslation(MoveDelta);
			Character->MoveForward(-MoveDelta.Y);
			Character->MoveRight(MoveDelta.X);
		}
	}
	
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FEventReply UControlWidget::OnBorderMoveTouchStartedCall(FGeometry InGeometry, const FPointerEvent& InGestureEvent)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()) || MoveIndex != -1) return FEventReply(false);

	MoveIndex = InGestureEvent.GetPointerIndex();
	MoveCenter = InGestureEvent.GetScreenSpacePosition();
	return FEventReply(true);
}

FEventReply UControlWidget::OnTouchStartedCall(FGeometry InGeometry, const FPointerEvent& InGestureEvent)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()) || ViewIndex != -1) return FEventReply(false);

	ViewIndex = InGestureEvent.GetPointerIndex();
	return FEventReply(true);
}

FEventReply UControlWidget::OnTouchMovedCall(FGeometry InGeometry, const FPointerEvent& InGestureEvent)
{
	if (UGameplayStatics::IsGamePaused(GetWorld())) return FEventReply(false);

	const int32 Index = InGestureEvent.GetPointerIndex();
	if (Index == ViewIndex)
	{
		if (const auto Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
			if (const auto Character = Cast<AMazeCharacter>(Player)) {
				auto Movement = InGestureEvent.GetCursorDelta();
				Movement *= TurnRate;
				Character->TurnAtRate(Movement.X);
				Character->LookUpAtRate(Movement.Y);

				return FEventReply(true);
			}
		}
	} else if (Index == MoveIndex)
	{
		MoveDelta = InGestureEvent.GetScreenSpacePosition() - MoveCenter;
		if (const auto Radius = MoveDelta.Size(); Radius > RestrictRadius) {
			MoveDelta *= (RestrictRadius / Radius);
		}

		return FEventReply(true);
	}

	return FEventReply(false);
}

FEventReply UControlWidget::OnTouchEndedCall(FGeometry InGeometry, const FPointerEvent& InGestureEvent)
{
	const int32 Index = InGestureEvent.GetPointerIndex();

	if (Index == ViewIndex)
	{
		ViewIndex = -1;
		return FEventReply(true);
	} else if (Index == MoveIndex)
	{
		MoveIndex = -1;
		MoveCenter = FVector2D::ZeroVector;
		MoveDelta = FVector2D::ZeroVector;
		if (BorderMove) BorderMove->SetRenderTranslation(MoveCenter);
		return FEventReply(true);
	}
	
	return FEventReply(false);
}

void UControlWidget::OnBtnJumpCall()const
{
	if (const auto Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
		if (const auto Character = Cast<AMazeCharacter>(Player)) {
			Character->Jump();
		}
	}
}

void UControlWidget::AttachMoveWidget(UBorder* Widget)
{
	if (Widget == nullptr) return;

	BorderMove = Widget;
	BorderMove->OnMouseButtonDownEvent.BindDynamic(this, &UControlWidget::OnBorderMoveTouchStartedCall);
	BorderMove->OnMouseButtonUpEvent.BindDynamic(this, &UControlWidget::OnTouchEndedCall);
	BorderMove->OnMouseMoveEvent.BindDynamic(this, &UControlWidget::OnTouchMovedCall);
}

void UControlWidget::ResetControl()
{
	ViewIndex = -1;
	MoveIndex = -1;
	MoveCenter = FVector2D::ZeroVector;
	MoveDelta = FVector2D::ZeroVector;
	if (BorderMove) BorderMove->SetRenderTranslation(MoveCenter);
}
