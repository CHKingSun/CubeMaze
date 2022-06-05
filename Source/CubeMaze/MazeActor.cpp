// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeActor.h"

#include "MazeCharacter.h"
#include "MazeDataGenerator.h"
#include "PortalActor.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMazeActor::AMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	UE_LOG(LogTemp,Warning,TEXT("MazeActor Initialize 0X%p!!!!"), this);

	// Default Value
	RandomSeed = 1024;
	MazeRow = 12;
	MazeCol = 12;
	bNeedEdge = true;
	CheckMazeData();

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	MazeFloor = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floor"));
	MazeFloor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeWall = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Wall");
	MazeWall->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MazeEdge = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Edge");
	MazeEdge->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	EntryLeft = CreateDefaultSubobject<UChildActorComponent>("EntryLeft");
	EntryLeft->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	EntryRight = CreateDefaultSubobject<UChildActorComponent>("EntryRight");
	EntryRight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	EntryTop = CreateDefaultSubobject<UChildActorComponent>("EntryTop");
	EntryTop->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	EntryBottom = CreateDefaultSubobject<UChildActorComponent>("EntryBottom");
	EntryBottom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AMazeActor::FinishDestroy()
{
	UE_LOG(LogTemp,Warning,TEXT("MazeActor Destroy 0X%p !!!!"),this);
	Super::FinishDestroy();
}

// Called when the game starts or when spawned
void AMazeActor::BeginPlay()
{
	Super::BeginPlay();

	if (!CheckChildActor()) return;
	// 在构造函数生成的话可能会因为蓝图编译而失效，所以放到这里绑定。
	for (const auto& Entry : EntryActors)
	{
		Entry->OnPortalActorBeginOverlap.AddDynamic(this, &AMazeActor::OnEntryBeginOvelap);
	}
}

void AMazeActor::GenerateMaze(bool bResetRandomSeed)
{
	// CheckMazeData();
	MazeData->Generate(bResetRandomSeed);

	// 这里先不加上边界的尺寸
	const FVector2D MazeSpaceSize = GetMazeRawSpaceSize();
	const FVector2D CenterOffset = MazeSpaceSize / 2.f;

	// Floor
	MazeFloor->ClearInstances();
	MazeFloor->AddInstance(FTransform(
		FRotator::ZeroRotator,
		FVector(-CenterOffset.X, -CenterOffset.Y, 0),
		FVector(MazeSpaceSize.X / MeshSize, MazeSpaceSize.Y / MeshSize, FloorZScale)
	));

	// Walls
	MazeWall->ClearInstances();
	FTransform Trans;
	for (auto& SpaceData : (*MazeData))
	{
		switch (SpaceData.DataType)
		{
		case EMazePlaceType::Space: break;
		case EMazePlaceType::Wall:
			if (SpaceData.MeshIndex == -2)
			{
				if (SpaceData.X % 2 == 0)
				{
					const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X);
					const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.Y) + SpaceSize.Y; 
					Trans.SetTranslationAndScale3D(FVector(-CenterOffset.X + OffsetX, -CenterOffset.Y + OffsetY, MeshSize * 0.5f), FVector(SpaceScale.X, WallScale.Y, WallScale.Z));	
				}
				else
				{
					const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X) + SpaceSize.X;
					const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.Y); 
					Trans.SetTranslationAndScale3D(FVector(-CenterOffset.X + OffsetX, -CenterOffset.Y + OffsetY, MeshSize * 0.5f), FVector(WallScale.X, SpaceScale.Y, WallScale.Z));	
				}

				SpaceData.MeshIndex = MazeWall->AddInstance(Trans);
			}
			break;
		case EMazePlaceType::Junction:
			if (SpaceData.MeshIndex == -2)
			{
				const auto OffsetX = SpaceData.X / 2 * (SpaceSize.X + WallSize.X) + SpaceSize.X;
				const auto OffsetY = SpaceData.Y / 2 * (SpaceSize.Y + WallSize.Y) + SpaceSize.Y; 
				Trans.SetTranslationAndScale3D(FVector(-CenterOffset.X + OffsetX, -CenterOffset.Y + OffsetY, MeshSize * 0.5f), WallScale);	

				SpaceData.MeshIndex = MazeWall->AddInstance(Trans);
			}
			break;
		default: ;
		}
	}

	// Edge
	UpdateMazeEdge(bNeedEdge);
}

void AMazeActor::UpdateMazeEdge(bool bNEdge)
{
	bNeedEdge = bNEdge;
	MazeEdge->ClearInstances();
	if (!bNeedEdge) return;

	FTransform Trans;
	FVector2D MazeSpaceSize = GetMazeRawSpaceSize();
	const FVector2D CenterOffset = MazeSpaceSize / 2.f;

	auto UpdateMazeEdge = [&](int32 InEntry, const FVector& InStartLoc, int32 InDir, const TObjectPtr<USceneComponent> InBox)
	{
		const int32 SideLength = InDir == 1 ? MazeRow : MazeCol;  // 也可以把MazeRow和MazeCol做成FIntVector2D
		FVector CurScale(EdgeScale);
		if (InEntry == -1)
		{
			CurScale[InDir] = MazeSpaceSize[InDir] / MeshSize;
			Trans.SetTranslationAndScale3D(InStartLoc, CurScale);
			MazeEdge->AddInstance(Trans);
		} else
		{
			FVector Movement(0.f);
			CurScale[InDir] = (WallScale[InDir] + SpaceScale[InDir]) * InEntry;
			Trans.SetTranslationAndScale3D(InStartLoc, CurScale);
			MazeEdge->AddInstance(Trans);
			
			Movement[InDir] = CurScale[InDir] * MeshSize;
			Trans.AddToTranslation(Movement);
			CurScale[InDir] = SpaceScale[InDir];
			CurScale.Z = FloorZScale;
			Trans.SetScale3D(CurScale);
			MazeEdge->AddInstance(Trans);

			FVector BoxOffset(0.f, 0.f, MeshSize);
			BoxOffset[InDir] = MeshSize;
			InBox->SetRelativeLocation(Trans.GetLocation() + BoxOffset);
			InBox->SetActive(true);
			
			Movement[InDir] = SpaceScale[InDir] * MeshSize;
			Trans.AddToTranslation(Movement);
			CurScale.Z = EdgeScale.Z;
			CurScale[InDir] = (WallScale[InDir] + SpaceScale[InDir]) * (SideLength - InEntry - 1);
			Trans.SetScale3D(CurScale);
			MazeEdge->AddInstance(Trans);
		}
	};
	
	int32 Entry = MazeData->GetEdgeEntry(EMazeDirection::Left);
	UpdateMazeEdge(Entry, FVector(-CenterOffset.X - EdgeSize.X, -CenterOffset.Y, 0.f), 1, EntryLeft);
	Entry = MazeData->GetEdgeEntry(EMazeDirection::Right);
	UpdateMazeEdge(Entry, FVector(CenterOffset.X, -CenterOffset.Y, 0.f), 1, EntryRight);
	Entry = MazeData->GetEdgeEntry(EMazeDirection::Bottom);
	UpdateMazeEdge(Entry, FVector(-CenterOffset.X, -CenterOffset.Y - EdgeSize.Y, 0.f), 0, EntryBottom);
	Entry = MazeData->GetEdgeEntry(EMazeDirection::Top);
	UpdateMazeEdge(Entry, FVector(-CenterOffset.X, CenterOffset.Y, 0.f), 0, EntryTop);

	// Corner
	Trans.SetScale3D(EdgeScale);
	Trans.SetLocation(FVector(-CenterOffset.X - EdgeSize.X, -CenterOffset.Y - EdgeSize.Y, 0.f));
	MazeEdge->AddInstance(Trans);
	Trans.SetLocation(FVector(CenterOffset.X, -CenterOffset.Y - EdgeSize.Y, 0.f));
	MazeEdge->AddInstance(Trans);
	Trans.SetLocation(FVector(CenterOffset.X, CenterOffset.Y, 0.f));
	MazeEdge->AddInstance(Trans);
	Trans.SetLocation(FVector(-CenterOffset.X - EdgeSize.X, CenterOffset.Y , 0.f));
	MazeEdge->AddInstance(Trans);
}

FVector2D AMazeActor::GetMazeRawSpaceSize() const
{
	return FVector2D(MazeCol * SpaceSize.X + (MazeCol - 1) * WallSize.X, MazeRow * SpaceSize.Y + (MazeRow - 1) * WallSize.Y);
}

FVector2D AMazeActor::GetMazeSpaceSize() const
{
	FVector2D Ret = GetMazeRawSpaceSize();
	if (bNeedEdge) Ret += FVector2D(EdgeSize.X * 2, EdgeSize.Y * 2);

	return Ret;
}

void AMazeActor::CheckMazeData()
{
	if (MazeData == nullptr)
	{
		MazeData = NewObject<UMazeDataGenerator>();
		UE_LOG(LogTemp,Warning,TEXT("MazeActor 0X%p Generate MazeData 0X%p!!!!"), this, MazeData.Get());
		MazeData->ResetMaze(MazeRow, MazeCol, RandomSeed);
	}
}

bool AMazeActor::CheckChildActor()
{
	EntryActors[EMazeDirection::Left] = Cast<APortalActor>(EntryLeft->GetChildActor());
	EntryActors[EMazeDirection::Bottom] = Cast<APortalActor>(EntryBottom->GetChildActor());
	EntryActors[EMazeDirection::Right] = Cast<APortalActor>(EntryRight->GetChildActor());
	EntryActors[EMazeDirection::Top] = Cast<APortalActor>(EntryTop->GetChildActor());

	for (const auto& Entry : EntryActors)
	{
		if (Entry == nullptr) return false;
	}
	return true;
}

void AMazeActor::OnEntryBeginOvelap(APortalActor* OverlappedActor, AActor* OtherActor)
{
	const FVector LeftRotAxis(0.f, 1.f, 0.f);
	const FVector RightRotAxis(0.f, -1.f, 0.f);
	const FVector BottomRotAxis(-1.f, 0.f, 0.f);
	const FVector TopRotAxis(1.f, 0.f, 0.f);

	float Duration = 10;
	
	FVector RotAxis;
	// if (OverlappedComponent == EntryLeft)
	// {
	// 	UKismetSystemLibrary::PrintString(GetWorld(), FString("Left!!!"), true, true, FLinearColor(0, 0.66, 1), Duration);
	// 	if (MazeData->GetMazeAround(UMazeDataGenerator::Left) == nullptr) return;
	// 	RotAxis = LeftRotAxis;
	// } else if (OverlappedComponent == EntryRight)
	// {
	// 	UKismetSystemLibrary::PrintString(GetWorld(), FString("Right!!!"), true, true, FLinearColor(0, 0.66, 1), Duration);
	// 	if (MazeData->GetMazeAround(UMazeDataGenerator::Right) == nullptr) return;
	// 	RotAxis = RightRotAxis;
	// } else if (OverlappedComponent == EntryBottom)
	// {
	// 	UKismetSystemLibrary::PrintString(GetWorld(), FString("Bottom!!!"), true, true, FLinearColor(0, 0.66, 1), Duration);
	// 	if (MazeData->GetMazeAround(UMazeDataGenerator::Bottom) == nullptr) return;
	// 	RotAxis = BottomRotAxis;
	// } else if (OverlappedComponent == EntryTop)
	// {
	// 	UKismetSystemLibrary::PrintString(GetWorld(), FString("Top!!!"), true, true, FLinearColor(0, 0.66, 1), Duration);
	// 	if (MazeData->GetMazeAround(UMazeDataGenerator::Top) == nullptr) return;
	// 	RotAxis = TopRotAxis;
	// }

	const FQuat ActorQuat = GetActorQuat();
	RotAxis = ActorQuat.RotateVector(RotAxis);
	RotAxis.Normalize();
	UKismetSystemLibrary::PrintString(GetWorld(), RotAxis.ToString(), true, true, FLinearColor(0, 0.66, 1), 10);
	const FQuat Rot(RotAxis, FMath::DegreesToRadians(90));
	// UKismetSystemLibrary::PrintString(GetWorld(), Rot.GetAxisZ().ToString(), true, true, FLinearColor(0, 0.66, 1), Duration);

	if (const auto ParentActor = GetParentActor())
	{
		if (auto Character = Cast<AMazeCharacter>(OtherActor))
		{
			// Character->SetActorEnableCollision(false);  // 防止移动时产生碰撞
			// Character->SetActorLocation(FVector(0.f, 0.f, 1000.f));
			// ParentActor->AddActorLocalRotation(Rot);
			// Character->SetActorEnableCollision(true);
		}
	}
}

// Called every frame
void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeActor::SetSizeAndRandomSeed(int32 MCol, int32 MRow, int32 RSeed)
{
	RandomSeed = RSeed;
	MazeRow = MRow;
	MazeCol = MCol;
	// CheckMazeData();
	MazeData->ResetMaze(MazeRow, MazeCol, RandomSeed);
}
