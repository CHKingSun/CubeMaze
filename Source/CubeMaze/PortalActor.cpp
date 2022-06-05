// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalActor.h"

#include "Components/BoxComponent.h"

// Sets default values
APortalActor::APortalActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Box = CreateDefaultSubobject<UBoxComponent>("TriggerBox");
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Box->SetBoxExtent(FVector(100.f, 100.f, 50.f));  // TODO need change later
	Box->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &APortalActor::OnBoxBeginEverlap);
}

void APortalActor::OnBoxBeginEverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnPortalActorBeginOverlap.Broadcast(this, OtherActor);
}

// Called every frame
void APortalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalActor::SetBindPortal(APortalActor* Portal1, APortalActor* Portal2)
{
	if (Portal1) Portal1->PortalBind = Portal2;
	if (Portal2) Portal2->PortalBind = Portal1;
}

void APortalActor::DrawPortalConnectLine(FColor Color)const
{
	if (PortalBind == nullptr) return;

	const FVector FromPos = GetActorLocation();
	const FVector ToPos = PortalBind->GetActorLocation();

	// DrawDebugLine(GetWorld(), FromPos, ToPos, Color, true, -1, 0, 10);
	DrawDebugLine(GetWorld(), FromPos, ToPos, Color, true, -1, 2, 10);
}

