// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletPickup.h"
#include "Components/SphereComponent.h"
#include "Project1Character.h"

// Sets default values
ABulletPickup::ABulletPickup()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(50.0f);
    RootComponent = CollisionComponent;

    BulletAmount = 10;

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABulletPickup::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ABulletPickup::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABulletPickup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABulletPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this) && OtherComp)
    {
        // 플레이어인지 판정
        AProject1Character* Character = Cast<AProject1Character>(OtherActor);
        if (Character)
        {
            Character->Bullets += BulletAmount;
            Character->ItemPickup();
            Destroy();
        }
    }
}