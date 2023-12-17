// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidManager.h"
#include "Boids.h"
#include "Obsticle.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ABoidManager::ABoidManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	transform = CreateDefaultSubobject<USceneComponent>("Root Scene Component");
	this->SetRootComponent(transform);
}

// Called when the game starts or when spawned
void ABoidManager::BeginPlay(){
	Super::BeginPlay();	
	QueryParams.AddIgnoredActor(this);
}

void ABoidManager::Raycast(ABoids* Boid){//Raycast Done in Manager instead of the boids for Optimisation
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Append(MyBoids);
	TArray<FHitResult> HitArray;

	const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Boid->Location, Boid->Location, AvoidRadius,
		UEngineTypes::ConvertToTraceType(ECC_Camera), false, ActorsToIgnore, EDrawDebugTrace::None, HitArray, true, FLinearColor::Yellow, FLinearColor::Blue, 0.f);

	if (TraceLines) {
		DrawDebugSphere(GetWorld(), Boid->Location, AvoidRadius, 12, Hit ? FColor::Blue : FColor::Yellow);
	}

	if (Hit) {
		for (FHitResult HitResult : HitArray) {
			Boid->Avoid(HitResult.GetActor()->GetActorLocation());
		}
	}

}

void ABoidManager::Spawn() {
	for (int i = 0; i < spawnCount; i++) {
		FVector  spawnLocation = ((FMath::VRand() * FMath::RandRange(0.0f, spawnRadius)) + GetActorLocation());
		FRotator spawnRotation = GetActorRotation();
		ABoids* newboid = GetWorld()->SpawnActor<ABoids>(spawnLocation, spawnRotation);
		newboid->Manager = this;
		QueryParams.AddIgnoredActor(newboid);
		MyBoids.Add(newboid);		
	}
	TaggedBoid = MyBoids[0];
	//Used for ItermitantThinking
	increment = int(MyBoids.Num() * 0.15);
}

void ABoidManager::spawnObsticles()
{
	for (int i = 0; i < obsticleSpawnCount; i++) {
		FVector  spawnLocation = ((FMath::VRand() * FMath::RandRange(0.0f, spawnRadius)) + GetActorLocation());
		FRotator spawnRotation = GetActorRotation();
		AObsticle* newObsticle = GetWorld()->SpawnActor<AObsticle>(spawnLocation, spawnRotation);
		MyObsticles.Add(newObsticle);
	}
}

void ABoidManager::clearScene(){
	for (int i = 0; i < MyObsticles.Num(); i++) {
		MyObsticles[i]->Destroy();
	
	}
	for (int i = 0; i < MyBoids.Num(); i++) {
		MyBoids[i]->Destroy();
	}
}

// Called every frame
void ABoidManager::Tick(float DeltaTime)
{ 
	Super::Tick(DeltaTime);
	if (DrawBoundBox) {
		DrawDebugBox(GetWorld(), GetActorLocation(), FVector(BoundsRadius, BoundsRadius, BoundsRadius),FColor(1,1,1),false,0.f);
	}
	if (ItermitantThinking == false) {
		 for (ABoids* Boid : MyBoids) {
			Boid->UpdateBoid(DeltaTime);
			Raycast(Boid);
		} 
	}

	if (ItermitantThinking == true) {
		if (MyBoids.Num() > 0) {
			if (reset) {
				reset = false;
				startCounter = 0;
				counter = increment;
			}
			else if ((counter + increment) > MyBoids.Num()) {
				counter = MyBoids.Num();
				startCounter += increment;
				reset = true;
			}
			else {
				counter += increment;
				startCounter += increment;
			}
			for (int i = startCounter; i < counter; i++) {
				MyBoids[i]->UpdateBoid(DeltaTime);
				Raycast(MyBoids[i]);
			}

		}
	}
	
	
	
}

FVector ABoidManager::ClosestBoidPosition(ABoids* thisBoid)
{
	float closestDistnace = maxFleeDistance;
	FVector returnVal = GetActorLocation();
		for (ABoids* Boid : MyBoids) {
			if (Boid == thisBoid || !Boid || Boid == lastTagged) {
			continue;
			}
			float aDist = (Boid->GetActorLocation() - thisBoid->GetActorLocation()).Size();
			if (aDist < closestDistnace) {
				closestDistnace = aDist;
				returnVal = Boid->GetActorLocation();
				returnActor = Boid;
			}
			if (thisBoid == TaggedBoid) {
				if (closestDistnace < catchRange) {
					//Tag new boid
					lastTagged = TaggedBoid;					
					TaggedBoid = Boid;	
					TaggedBoid->waitCounter = timeOutTime;
				}
			}
		}		
		return returnVal;	
}

ABoids* ABoidManager::ClosestBoidActor()
{
	//return Actor is given in ClosestBoidPosition()
	return returnActor;

}

TArray<ABoids*> ABoidManager::GetBoidNeighbourHood(ABoids* thisBoid)
{
	TArray<class ABoids*> returnBoids;

	for (ABoids* Boid : MyBoids) {
		if (Boid == thisBoid || !Boid)	continue;
		float aDist = (Boid->GetActorLocation() - thisBoid->GetActorLocation()).Size();
		if (aDist < neighbourRadius) {
			returnBoids.Add(Boid);
		}
	}
	return returnBoids;
}


