// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeMazeActor.h"
#include "MazeActor.h"
#include "MazeDataGenerator.h"

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

bool ACubeMazeActor::CheckChildActor()
{
	ActorBottom = Cast<AMazeActor>(MazeBottom->GetChildActor());
	ActorTop = Cast<AMazeActor>(MazeTop->GetChildActor());
	ActorFront = Cast<AMazeActor>(MazeFront->GetChildActor());
	ActorBack = Cast<AMazeActor>(MazeBack->GetChildActor());
	ActorLeft = Cast<AMazeActor>(MazeLeft->GetChildActor());
	ActorRight = Cast<AMazeActor>(MazeRight->GetChildActor());
	return ActorBottom != nullptr && ActorTop != nullptr
		&& ActorFront != nullptr && ActorBack != nullptr
		&& ActorLeft != nullptr && ActorRight != nullptr;
}

bool ACubeMazeActor::SetChildsActorClass()
{
	MazeBottom->SetChildActorClass(MazeClass);
	MazeTop->SetChildActorClass(MazeClass);
	MazeFront->SetChildActorClass(MazeClass);
	MazeBack->SetChildActorClass(MazeClass);
	MazeLeft->SetChildActorClass(MazeClass);
	MazeRight->SetChildActorClass(MazeClass);

	if (CheckChildActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("CubeMazeActor Set Childs Success!"));
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CubeMazeActor Set Childs failed!"));
		return false;
	}
}

void ACubeMazeActor::BindMazeAround()const
{
	auto BindSigle = [](const TObjectPtr<AMazeActor>& InMaze, const TObjectPtr<AMazeActor>& L,
		const TObjectPtr<AMazeActor>& B, const TObjectPtr<AMazeActor>& R, const TObjectPtr<AMazeActor>& T)
	{
		if (InMaze == nullptr) return;
		InMaze->ResetEdgeEntry();
		InMaze->SetMazeAround(EMazeDirection::Left, L);
		InMaze->SetMazeAround(EMazeDirection::Bottom, B);
		InMaze->SetMazeAround(EMazeDirection::Right, R);
		InMaze->SetMazeAround(EMazeDirection::Top, T);
	};

	BindSigle(ActorTop, ActorLeft, ActorBack, ActorRight, ActorFront);
	BindSigle(ActorBottom, ActorLeft, ActorBack, ActorRight, ActorFront);
	BindSigle(ActorFront, ActorLeft, ActorTop, ActorRight, ActorBottom);
	BindSigle(ActorBack, ActorLeft, ActorTop, ActorRight, ActorBottom);
	BindSigle(ActorLeft, ActorTop, ActorBack, ActorBottom, ActorFront);
	BindSigle(ActorRight, ActorTop, ActorBack, ActorBottom, ActorFront);

	ActorFront->BindEntries();
	ActorBack->BindEntries();
	ActorTop->BindEntries();
	ActorBottom->BindEntries();
	ActorLeft->BindEntries();
	ActorRight->BindEntries();
	
	ActorFront->GenerateEntry(true);
}

void ACubeMazeActor::GenerateCubeMaze(bool bResetRandomSeed)
{
	if (!CheckChildActor()) return;
	
	// Initialize Maze Data
	if (bResetRandomSeed) RandomStream.Reset();

	ActorBottom->SetSizeAndRandomSeed(MazeSize.X, MazeSize.Y, RandomStream.RandHelper(0x7fffffff));
	ActorTop->SetSizeAndRandomSeed(MazeSize.X, MazeSize.Y, RandomStream.RandHelper(0x7fffffff));
	ActorFront->SetSizeAndRandomSeed(MazeSize.X, MazeSize.Z, RandomStream.RandHelper(0x7fffffff));
	ActorBack->SetSizeAndRandomSeed(MazeSize.X, MazeSize.Z, RandomStream.RandHelper(0x7fffffff));
	ActorLeft->SetSizeAndRandomSeed(MazeSize.Z, MazeSize.Y, RandomStream.RandHelper(0x7fffffff));
	ActorRight->SetSizeAndRandomSeed(MazeSize.Z, MazeSize.Y, RandomStream.RandHelper(0x7fffffff));

	BindMazeAround();

	ActorBottom->GenerateMaze(bResetRandomSeed);
	ActorTop->GenerateMaze(bResetRandomSeed);
	ActorFront->GenerateMaze(bResetRandomSeed);
	ActorBack->GenerateMaze(bResetRandomSeed);
	ActorLeft->GenerateMaze(bResetRandomSeed);
	ActorRight->GenerateMaze(bResetRandomSeed);

	const FVector SpaceSize(ActorBottom->GetMazeSpaceSize(), ActorFront->GetMazeSpaceSize().Y);
	const auto CenterOffset = SpaceSize / 2.f;
	
	ActorTop->SetActorRelativeTransform(FTransform(FRotator::ZeroRotator, FVector(0.f, 0.f, CenterOffset.Z), FVector::OneVector));
	ActorBottom->SetActorRelativeTransform(FTransform(FRotator::ZeroRotator, FVector(0.f, 0.f, -CenterOffset.Z), FVector(1.f, 1.f, -1.f)));
	ActorFront->SetActorRelativeTransform(FTransform(UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::XAxisVector, -90.f), FVector(0.f, CenterOffset.Y, 0.f), FVector::OneVector));
	ActorBack->SetActorRelativeTransform(FTransform(UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::XAxisVector, -90.f), FVector(0.f, -CenterOffset.Y, 0.f), FVector(1.f, 1.f, -1.f)));
	ActorLeft->SetActorRelativeTransform(FTransform(UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::YAxisVector, 90.f), FVector(-CenterOffset.X, 0.f, 0.f), FVector(1.f, 1.f, -1.f)));
	ActorRight->SetActorRelativeTransform(FTransform(UKismetMathLibrary::RotatorFromAxisAndAngle(FVector::YAxisVector, 90.f), FVector(CenterOffset.X, 0.f, 0.f), FVector::OneVector));
	
	//Edge
	constexpr float MeshSize = 100.f;
	constexpr float EdgeScale = 2.f;
	constexpr float EdgeSize = EdgeScale * MeshSize;
	MazeEdge->ClearInstances();
	FTransform Trans;

	// 这里面包括了8个角落
	Trans.SetScale3D(FVector(SpaceSize.X / MeshSize + 2 * EdgeScale, EdgeScale, EdgeScale));
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
	
	UE_LOG(LogTemp,Warning,TEXT("Generate Cube Maze End!!!!"), this);
}

// Called every frame
void ACubeMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

