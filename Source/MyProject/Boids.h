// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boids.generated.h"

UCLASS()
class MYPROJECT_API ABoids : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoids();
	FVector currentVelocity = FVector::ZeroVector;
	class ABoidManager* Manager;
	void Avoid(FVector ObsticlePosition);
	float waitCounter = 0;
	FVector targetVelocity;
	FVector Location;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<ABoids*> closestBoidPosArray;
	
	FVector closestBoidPos;
	ABoids* closestBoidActor;

	//Tag
	FVector Seek(FVector position);
	FVector flee(FVector position);

	FVector Evade(FVector position, ABoids* boid);
	FVector Persue(FVector position,ABoids* boid);
	
	//Flocking
	FVector wander(float radius, float distance, float jitter);
	FVector wanderDestination;

	FVector alignment(TArray<ABoids*> neighbours);
	FVector cohesion(TArray<ABoids*> neighbours);
	FVector seperation(TArray<ABoids*> neighbours);
	
	//Bound Checker
	FVector BoundCheck(FVector CurrentPos);
		

	UStaticMeshComponent* Mesh;
	//float speed = 200.0f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void UpdateBoid(float DeltaTime);


};
