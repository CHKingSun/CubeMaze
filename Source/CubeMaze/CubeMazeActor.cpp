// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeMazeActor.h"
#include "MazeActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACubeMazeActor::ACubeMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Default Value
	RandomStream.Initialize(1024);
	MazeSize = FIntVector(12);

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

bool ACubeMazeActor::CheckChildActor() const
{
	return Cast<AMazeActor>(MazeBottom->GetChildActor()) != nullptr
		&& Cast<AMazeActor>(MazeTop->GetChildActor()) != nullptr
		&& Cast<AMazeActor>(MazeFront->GetChildActor()) != nullptr
		&& Cast<AMazeActor>(MazeBack->GetChildActor()) != nullptr
		&& Cast<AMazeActor>(MazeLeft->GetChildActor()) != nullptr
		&& Cast<AMazeActor>(MazeRight->GetChildActor()) != nullptr;
}

void ACubeMazeActor::UpdateCubeMaze(bool bResetRandomSeed)
{
	if (!CheckChildActor()) return;
	
	// Initialize Maze Data
	if (bResetRandomSeed) RandomStream.Reset();

	FVector SpaceSize;
	FVector2D SpaceXY = InitializeChildMaze(MazeBottom, MazeSize.X, MazeSize.Y);
	SpaceSize.X = SpaceXY.X;
	SpaceSize.Y = SpaceXY.Y;
	InitializeChildMaze(MazeTop, MazeSize.X, MazeSize.Y);
	SpaceXY = InitializeChildMaze(MazeFront, MazeSize.X, MazeSize.Z);
	SpaceSize.Z = SpaceXY.Y;
	InitializeChildMaze(MazeBack, MazeSize.X, MazeSize.Z);
	InitializeChildMaze(MazeLeft, MazeSize.Z, MazeSize.Y);
	InitializeChildMaze(MazeRight, MazeSize.Z, MazeSize.Y);
	const auto CenterOffset = SpaceSize / 2.f;
	
	MazeBottom->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -CenterOffset.Z), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::XAxisVector, 180.f));
	MazeTop->SetRelativeLocationAndRotation(FVector(0.f, 0.f, CenterOffset.Z), FRotator::ZeroRotator);
	MazeFront->SetRelativeLocationAndRotation(FVector(0.f, CenterOffset.Y, 0.f), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::XAxisVector, -90.f));
	MazeBack->SetRelativeLocationAndRotation(FVector(0.f, -CenterOffset.Y, 0.f), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::XAxisVector, 90.f));
	MazeLeft->SetRelativeLocationAndRotation(FVector(-CenterOffset.X, 0.f, 0.f), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::YAxisVector, -90.f));
	MazeRight->SetRelativeLocationAndRotation(FVector(CenterOffset.X, 0.f, 0.f), UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::YAxisVector, 90.f));

	//Edge
	const float MeshSize = 100.f;
	const float EdgeScale = 2.f;
	const float EdgeSize = EdgeScale * MeshSize;
	MazeEdge->ClearInstances();
	FTransform Trans;
	
	Trans.SetScale3D(FVector(SpaceSize.X / MeshSize + 2 * EdgeScale, EdgeScale, EdgeScale));  // 2 cantains 2 vertex
	Trans.SetTranslation(FVector(-CenterOffset.X - EdgeSize, CenterOffset.Y, CenterOffset.Z));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset.X - EdgeSize, -CenterOffset.Y - EdgeSize, CenterOffset.Z));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset.X - EdgeSize, -CenterOffset.Y - EdgeSize, -CenterOffset.Z - EdgeSize));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset.X - EdgeSize, CenterOffset.Y, -CenterOffset.Z - EdgeSize));
	MazeEdge->AddInstance(Trans);
	
	Trans.SetScale3D(FVector(EdgeScale, SpaceSize.Y / MeshSize, EdgeScale));
	Trans.SetTranslation(FVector(CenterOffset.X, -CenterOffset.Y, CenterOffset.Z));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(CenterOffset.X, -CenterOffset.Y, -CenterOffset.Z - EdgeSize));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset.X - EdgeSize, -CenterOffset.Y, -CenterOffset.Z - EdgeSize));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset.X - EdgeSize, -CenterOffset.Y, CenterOffset.Z));
	MazeEdge->AddInstance(Trans);
	
	Trans.SetScale3D(FVector(EdgeScale, EdgeScale, SpaceSize.Z / MeshSize));
	Trans.SetTranslation(FVector(CenterOffset.X, CenterOffset.Y, -CenterOffset.Z));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(CenterOffset.X, -CenterOffset.Y - EdgeSize, -CenterOffset.Z));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset.X - EdgeSize, -CenterOffset.Y - EdgeSize, -CenterOffset.Z));
	MazeEdge->AddInstance(Trans);
	Trans.SetTranslation(FVector(-CenterOffset.X - EdgeSize, CenterOffset.Y, -CenterOffset.Z));
	MazeEdge->AddInstance(Trans);
	
}

FVector2D ACubeMazeActor::InitializeChildMaze(const TObjectPtr<UChildActorComponent> Child, int32 MRow, int32 MCol) const
{
	FVector2D SpaceSize;
	if (const auto MazeActor = Cast<AMazeActor>(Child->GetChildActor()))
	{
		MazeActor->UpdateSizeAndRandomSeed(MRow, MCol, RandomStream.RandHelper(0x7fffffff));  // Helper的数需要>0
		SpaceSize = MazeActor->GetMazeSpaceSize();
	}
	return SpaceSize;
}

// Called every frame
void ACubeMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

