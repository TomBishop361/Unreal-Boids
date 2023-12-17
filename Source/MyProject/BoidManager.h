// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidManager.generated.h"

UCLASS()
class MYPROJECT_API ABoidManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidManager();

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		bool TraceLines = false;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		bool DrawBoundBox = false;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		int spawnCount = 30;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		int obsticleSpawnCount = 30;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")

		float spawnRadius = 500.0f;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Tag Settings")
		float maxFleeDistance = 900000.0f;


	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Tag Settings")
		int catchRange = 150;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Tag Settings")
		float chaserMultiplier = 2.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Tag Settings")
		float timeOutTime = 2.0f;


	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Flock Settings")
		float neighbourRadius = 900;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Optimisation Settings")
		bool ItermitantThinking = false;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Flock Settings")
		float seperationWeight = 1;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Flock Settings")
		float CohesionWeight = 1;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Flock Settings")
		float alignmentWeight = 1;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Boid Settings")
		int behaviour = 0;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Boid Settings")
		float speed = 100;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Boid Settings")
		float BoundsRadius = 1000;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Boid Settings")
		float BoundStrictness = 1;
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Boid Settings")
		float AvoidRadius = 300.f;

	UFUNCTION(BlueprintCallable)
		void Spawn();
	UFUNCTION(BlueprintCallable)
		void spawnObsticles();
	UFUNCTION(BlueprintCallable)
		void clearScene();


	USceneComponent* transform;
	//Raycast Var
	FCollisionQueryParams QueryParams;
	int flipFLop = 1;
	//Object Generation Vars
	TArray<class AObsticle*> MyObsticles;
	TArray<class ABoids*> MyBoids;
	//Tag Vars
	class ABoids* lastTagged;
	class ABoids* TaggedBoid;
	class ABoids* returnActor;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Raycast
	void Raycast(ABoids* Boid);
	TEnumAsByte<ECollisionChannel> BoidChannel;
	//Itermitent Thinking
	int counter = 100;
	int increment;
	int startCounter = 0;
	bool reset = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FVector ClosestBoidPosition(ABoids* thisBoid);
	ABoids* ClosestBoidActor();
	TArray<ABoids*>GetBoidNeighbourHood(class ABoids* thisBoid);
};
