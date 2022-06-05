// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeActor.h"

#include "MazeCharacter.h"
#include "MazeDataGenerator.h"
#include "EntryPortalActor.h"
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

void AMazeActor::ResetEdgeEntry()
{
	// CheckChildActor();
	MazeData->ResetEdgeEntry();
	for (const auto& Entry : EntryActors) Entry->SetEntryPosition(MazeData->GetEdgeEntry(Entry->GetEntryDirection()));
}

void AMazeActor::BindEntries()
{
	for (const auto& Dir : EMazeDirection::Directions)
	{
		auto Portal = AroundMazes[Dir] ? AroundMazes[Dir]->GetEntryToMazeAround(this) : nullptr;
		APortalActor::SetBindPortal(EntryActors[Dir], Portal);
	}
}

void AMazeActor::GenerateEntry(bool bRecursive)
{
	bool bNeedUpdate = false;
	for (const auto& Dir : EMazeDirection::Directions)
	{
		if (MazeData->GetEdgeEntry(Dir) != -1) continue;
		bNeedUpdate = true;
		
		int32 Index = EntryActors[Dir]->GetBindPortalEntry();
		if (Index == -1)
		{
			Index = MazeData->GenerateEdgeEntry(Dir);
		}
		else
		{
			MazeData->SetEdgeEntry(Dir, Index);
		}
		EntryActors[Dir]->SetEntryPosition(Index);
	}
	if (!bNeedUpdate || !bRecursive) return;

	for (const auto& Maze : AroundMazes)
	{
		if (Maze) Maze->GenerateEntry(bRecursive);
	}
}

TObjectPtr<AEntryPortalActor> AMazeActor::GetEntryToMazeAround(const TObjectPtr<AMazeActor>& Maze)const
{
	if (Maze == nullptr) return nullptr;
	for (const auto& Dir : EMazeDirection::Directions)
	{
		if (Maze == AroundMazes[Dir]) return EntryActors[Dir];
	}
	return nullptr;
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
	EntryActors[EMazeDirection::Left] = Cast<AEntryPortalActor>(EntryLeft->GetChildActor());
	EntryActors[EMazeDirection::Bottom] = Cast<AEntryPortalActor>(EntryBottom->GetChildActor());
	EntryActors[EMazeDirection::Right] = Cast<AEntryPortalActor>(EntryRight->GetChildActor());
	EntryActors[EMazeDirection::Top] = Cast<AEntryPortalActor>(EntryTop->GetChildActor());

	bool bRet = true;
	for (const auto& Dir : EMazeDirection::Directions)
	{
		const auto& Entry = EntryActors[Dir];
		if (Entry == nullptr)
		{
			bRet = false;
		}
		else
		{
			Entry->SetEntryDirection(Dir);
		}
	}
	return bRet;
}

bool AMazeActor::SetChildsActorClass()
{
	EntryLeft->SetChildActorClass(EntryClass);
	EntryRight->SetChildActorClass(EntryClass);
	EntryBottom->SetChildActorClass(EntryClass);
	EntryTop->SetChildActorClass(EntryClass);

	if (CheckChildActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("MazeActor Set Childs Success!"));
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MazeActor Set Childs failed!"));
		return false;
	}
}

void AMazeActor::OnEntryBeginOvelap(APortalActor* OverlappedActor, AActor* OtherActor)
{
	const auto Entry = Cast<AEntryPortalActor>(OverlappedActor);
	if (Entry == nullptr) return;

	const auto Dir = Entry->GetEntryDirection();
	const auto& AroundMaze = AroundMazes[Dir];
	if (AroundMaze == nullptr) return;

	FVector UpVector = FVector::UpVector;
	const FVector Scale = AroundMaze->GetActorScale3D();  // 镜像的是ChildActorComponent
	UpVector *= FVector(FMath::Sign(Scale.X), FMath::Sign(Scale.Y), FMath::Sign(Scale.Z));  // 处理镜像问题
	UpVector = AroundMaze->GetActorQuat().RotateVector(UpVector);  // 找到要旋转的Maze的Z轴

	// FString Str("Entry: ");
	// switch (Dir)
	// {
	// case EMazeDirection::Left: Str += "Left"; break;
	// case EMazeDirection::Bottom: Str += "Bottom"; break;
	// case EMazeDirection::Right: Str += "Right"; break;
	// case EMazeDirection::Top: Str += "Top"; break;
	// case EMazeDirection::DirectionSize: break;
	// default: ;
	// }
	// Str += ", Forward: " + UpVector.ToString();
	// UKismetSystemLibrary::PrintString(GetWorld(), Str, true, true, FLinearColor(0, 0.66, 1), 2);

	
	if (const auto ParentActor = GetParentActor())
	{
		if (auto Character = Cast<AMazeCharacter>(OtherActor))
		{
			FVector Axis = UpVector ^ FVector::UpVector;  // 求出轴向
			Axis.Normalize();
			const FQuat Rot = FQuat(Axis, FMath::DegreesToRadians(90));
			// Character->AddActorLocalOffset(FVector(0.f, 0.f, 1000.f));  // 防止移动时产生碰撞
			ParentActor->AddActorWorldRotation(Rot);
			Character->SetActorLocation(FVector(0.f, 0.f, 1000.f));

			UKismetSystemLibrary::PrintString(GetWorld(), AroundMaze->GetActorTransform().GetScaledAxis(EAxis::Z).ToString(), true, true, FLinearColor(0, 0.66, 1), 2);
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
