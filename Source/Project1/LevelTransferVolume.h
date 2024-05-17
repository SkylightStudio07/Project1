// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransferVolume.generated.h"

UCLASS()
class PROJECT1_API ALevelTransferVolume : public AActor
{
	GENERATED_BODY()
	

	
public:	
	// Sets default values for this actor's properties
	ALevelTransferVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	UPROPERTY(EditAnywhere, Category = MapData, meta = (AllowPrivateAccess = "true"))
		FString TransferLevelName;
	UPROPERTY()
		class UBoxComponent* TransferVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
