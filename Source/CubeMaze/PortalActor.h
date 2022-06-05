// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

class APortalActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FPortalActorBeginOverlapSignature, APortalActor*, OverlappedActor, AActor*, OtherActor );

UCLASS()
class CUBEMAZE_API APortalActor : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category=Portal, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> Box;

	UPROPERTY(Category=Portal, BlueprintReadWrite, EditInstanceOnly)
	TObjectPtr<APortalActor> PortalBind;

public:

	UPROPERTY(Category=Portal, BlueprintAssignable)
	FPortalActorBeginOverlapSignature OnPortalActorBeginOverlap;
	
public:	
	// Sets default values for this actor's properties
	APortalActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginEverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(Category=Portal, BlueprintCallable, BlueprintPure=false)
	void DrawPortalConnectLine(FColor Color = FColor(169, 7, 228))const;

	UFUNCTION(Category=Portal, BlueprintCallable)
	static void SetBindPortal(APortalActor* Portal1, APortalActor* Portal2);

	const TObjectPtr<APortalActor>& GetBindPortal()const { return PortalBind; }

};
