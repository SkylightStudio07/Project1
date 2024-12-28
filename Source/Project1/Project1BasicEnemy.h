// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Project1BasicEnemy.generated.h"

UCLASS()
class PROJECT1_API AProject1BasicEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProject1BasicEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	void MoveToPlayer(); // 플레이어 추적 함수

	AActor* PlayerActor; 

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float EnemyMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float EnemyHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* SkeletalMesh;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

	void OnDeathFinished();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
		bool IsDead;

	UPROPERTY(EditAnywhere, Category = "Enemy")
		float TimeBeforeRemoval;

	float MaxHP;

	FTimerHandle TimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
		UCapsuleComponent* BulletCollisionCapsule;

};
