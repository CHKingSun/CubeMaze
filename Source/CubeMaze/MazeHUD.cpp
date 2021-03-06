// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeHUD.h"
#include "MazeWidgetBase.h"
#include "Kismet/GameplayStatics.h"

void AMazeHUD::CheckGamePause()
{
	UGameplayStatics::SetGamePaused(GetWorld(), PauseWidgetCount > 0);
}

void AMazeHUD::BindWidgetClass(const FString& WidgetName, TSubclassOf<UMazeWidgetBase> InWidgetClass)
{
	auto& [WidgetClass, Widget] = AllWidgets.FindOrAdd(WidgetName);

	WidgetClass = InWidgetClass;
}

UMazeWidgetBase* AMazeHUD::GetWidget(const FString& WidgetName, bool bPushWidget)
{
	const auto WidStruct = AllWidgets.Find(WidgetName);

	if (WidStruct == nullptr || (*(WidStruct->WidgetClass)) == nullptr) return nullptr;

	if (WidStruct->Widget == nullptr)
	{
		WidStruct->Widget = CreateWidget<UMazeWidgetBase>(GetOwningPlayerController(), WidStruct->WidgetClass, FName(WidgetName));
		WidStruct->Widget->AddToViewport();  // 默认添加到屏幕
	}

	if (bPushWidget) PushWidget(WidStruct->Widget);

	return WidStruct->Widget;
}

void AMazeHUD::RemoveWidget(const FString& WidgetName)
{
	if (const auto WidStruct = AllWidgets.Find(WidgetName))
	{
		if (const auto& Wid = WidStruct->Widget)
		{
			if (Wid->GetWidgetState() == EWidgetState::Push) PopWidget(Wid);
			Wid->RemoveFromParent();
		}
		WidStruct->Widget = nullptr;
	}
}

void AMazeHUD::PushWidget(UMazeWidgetBase* Widget)
{
	// 目前Push过的Widget就不在Push
	if (Widget == nullptr || Widget->GetWidgetState() == EWidgetState::Push) return;

	Widget->OnEnterWidget();

	if (Widget->IsWigetNeedPause())
	{
		PauseWidgetCount += 1;
		CheckGamePause();
	}

	if (Widget->IsWigetNeedMouse())
	{
		if (const auto Controller = GetOwningPlayerController())
		{
			Controller->SetShowMouseCursor(true);
			Controller->SetInputMode(FInputModeGameAndUI().SetWidgetToFocus(Widget->TakeWidget()));
		}
	}
}

void AMazeHUD::PopWidget(UMazeWidgetBase* Widget)
{
	if (Widget == nullptr || Widget->GetWidgetState() == EWidgetState::Pop) return;
	
	if (Widget->IsWigetNeedPause())
	{
		PauseWidgetCount -= 1;
		CheckGamePause();
	}

	if (Widget->IsWigetNeedMouse())
	{
		if (const auto Controller = GetOwningPlayerController())
		{
			// 这里其实可能会出现问题，因为正常情况下应该是返回上一个界面的InputMode
			// 但是Stack没做，所以先这样
			Controller->SetShowMouseCursor(false);
			Controller->SetInputMode(FInputModeGameOnly());
		}
	}
	
	Widget->OnExitWidget();
}
