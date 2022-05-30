// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeMazeActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACubeMazeActor::ACubeMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Default Value
	RandomStream.Initialize(1024);
	MazeSize = 12;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	MazeBottom = CreateDefaultSubobject<UChildActorComponent>("MazeBottom");
	MazeBottom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeTop = CreateDefaultSubobject<UChildActorComponent>("MazeTop");
	MazeTop->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeFront = CreateDefaultSubobject<UChildActorComponent>("MazeFront");
	MazeFront->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeBack = CreateDefaultSubobject<UChildActorComponent>("MazeBack");
	MazeBack->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeLeft = CreateDefaultSubobject<UChildActorComponent>("MazeLeft");
	MazeLeft->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeRight = CreateDefaultSubobject<UChildActorComponent>("MazeRight");
	MazeRight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeEdge = CreateDefaultSubobject<UInstancedStaticMeshComponent>("MazeEdge");
	MazeEdge->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
}

// Called when the game starts or when spawned
void ACubeMazeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACubeMazeActor::UpdateCubeMaze(bool bResetRandomSeed)
{
	if (bResetRandomSeed) RandomStream.Reset();
	constexpr float MeshSize = 100.f;
	const float SideLength = MazeSize * 2 - 1;
	CenterOffset = SideLength * MeshSize / 2.f;

	// Maze
	SetChildSizeAndRandomSeed(MazeBottom);
	MazeBottom->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -CenterOffset), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::XAxisVector, 180.f));
	SetChildSizeAndRandomSeed(MazeTop);
	MazeTop->SetRelativeLocationAndRotation(FVector(0.f, 0.f, CenterOffset), FRotator::ZeroRotator);
	SetChildSizeAndRandomSeed(MazeFront);
	MazeFront->SetRelativeLocationAndRotation(FVector(0.f, CenterOffset, 0.f), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::XAxisVector, -90.f));
	SetChildSizeAndRandomSeed(MazeBack);
	MazeBack->SetRelativeLocationAndRotation(FVector(0.f, -CenterOffset, 0.f), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::XAxisVector, 90.f));
	SetChildSizeAndRandomSeed(MazeLeft);
	MazeLeft->SetRelativeLocationAndRotation(FVector(-CenterOffset, 0.f, 0.f), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::YAxisVector, -90.f));
	SetChildSizeAndRandomSeed(MazeRight);
	MazeRight->SetRelativeLocationAndRotation(FVector(CenterOffset, 0.f, 0.f), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::YAxisVector, 90.f));

	//Edge
	const float EdgeScale = 1.5f;
	const float EdgeSize = EdgeScale * MeshSize;
	MazeEdge->ClearInstances();
	FTransform Trans;

	Trans.SetScale3D(FVector(SideLength + 2 * EdgeScale, EdgeScale, EdgeScale));  // 2 cantains 2 vertex
	Trans.SetTranslation(FVector(-CenterOffset - EdgeSize, CenterOffset, CenterOffset));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset - EdgeSize, -CenterOffset - EdgeSize, CenterOffset));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset - EdgeSize, -CenterOffset - EdgeSize, -CenterOffset - EdgeSize));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset - EdgeSize, CenterOffset, -CenterOffset - EdgeSize));
	MazeEdge->AddInstance(Trans);

	Trans.SetScale3D(FVector(EdgeScale, SideLength, EdgeScale));
	Trans.SetTranslation(FVector(CenterOffset, -CenterOffset, CenterOffset));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(CenterOffset, -CenterOffset, -CenterOffset - EdgeSize));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset - EdgeSize, -CenterOffset, -CenterOffset - EdgeSize));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset - EdgeSize, -CenterOffset, CenterOffset));
	MazeEdge->AddInstance(Trans);

	Trans.SetScale3D(FVector(EdgeScale, EdgeScale, SideLength));
	Trans.SetTranslation(FVector(CenterOffset, CenterOffset, -CenterOffset));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(CenterOffset, -CenterOffset - EdgeSize, -CenterOffset));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset - EdgeSize, -CenterOffset - EdgeSize, -CenterOffset));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset - EdgeSize, CenterOffset, -CenterOffset));
	MazeEdge->AddInstance(Trans);
	
}

void ACubeMazeActor::SetChildSizeAndRandomSeed(const TObjectPtr<UChildActorComponent> Child) const
{
	if (const auto MazeActor = Cast<AMazeActor>(Child->GetChildActor()))
	{
		MazeActor->UpdateSizeAndRandomStream(MazeSize, RandomStream.GetUnsignedInt());
	}
}

// Called every frame
void ACubeMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

