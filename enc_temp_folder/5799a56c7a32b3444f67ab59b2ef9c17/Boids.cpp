// Fill out your copyright notice in the Description page of Project Settings.

#include"BoidManager.h"
#include "Boids.h"

// Sets default values

ABoids::ABoids()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cone"));
	UStaticMesh* ConeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/ConeBoid.ConeBoid'")).Object;
	
	Mesh->SetStaticMesh(ConeMesh);
	
	this->SetRootComponent(Mesh);
}

void ABoids::Avoid(FVector ObsticlePosition)
{
	FVector returnVector = FVector::ZeroVector;
	FVector relativePosition = ObsticlePosition - Location;		
	//Using Dot Product of its local position and the Boids Direction Vectors, find the direction the obstacle is (left,right/up,down)
	bool isRightward = FVector::DotProduct(relativePosition, GetActorRightVector()) > 0;
	bool isUpward = FVector::DotProduct(relativePosition, GetActorUpVector()) > 0;
	bool isForward = FVector::DotProduct(relativePosition, GetActorForwardVector()) > 0;

	// Adjust the return vector based on the relative position of the obstacle.

	if (isRightward) {
		returnVector += FVector(0, -1, 0);
	}
	if (!isRightward) {
		returnVector += FVector(0, 1, 0);
	}
	if (isForward) {
		returnVector += FVector(-1, 0, 0);
	}
	if (!isForward) {
		returnVector += FVector(1, 0, 0);
	}
	if (isUpward) {
		returnVector += FVector(0, 0, -1);
	}
	if (!isUpward) {
		returnVector += FVector(0, 0, -1);
	}
		
	//Return fector is a average direction to avoid all obstacles in the sphere
	targetVelocity += returnVector;
}

// Called when the game starts or when spawned
void ABoids::BeginPlay()
{
	Super::BeginPlay();
	
}

//Tag
FVector ABoids::Seek(FVector position){
	FVector newVelocity = position - Location;
	newVelocity.Normalize();
	return newVelocity;
}

FVector ABoids::flee(FVector position){
	FVector newVelocity = Location - position;
	newVelocity.Normalize();
	return newVelocity;
}


FVector ABoids::Evade(FVector position, ABoids* boid){
	FVector projectedPos = boid->Location + (boid->GetActorForwardVector() * 200);
	return flee(projectedPos);
}

FVector ABoids::Persue(FVector position,ABoids* boid){
	FVector projectedPos = boid->Location + (boid->GetActorForwardVector() * 200);
	return Seek(projectedPos);
}

FVector ABoids::wander(float radius, float distance, float jitter)
{
	FVector myLocation = Location;
	if (FVector::Dist(myLocation, wanderDestination) < 100.f) {
		//Get a new destination in front of the boid
		FVector projectedPos = myLocation + (GetActorForwardVector() * distance);

		wanderDestination = projectedPos + (FMath::VRand() * FMath::RandRange(0.0f, jitter));
	}

	//continue going current destination
	FVector jitterDestination = Seek(wanderDestination) + (FMath::VRand() * FMath::RandRange(0.0f, jitter));
	return jitterDestination;
}

//Flocking
FVector ABoids::alignment(TArray<ABoids*> neighbours){
	if (neighbours.Num() == 0) return FVector::ZeroVector;
	FVector newVelocity;
	for (ABoids* Boid : neighbours) {
		newVelocity += Boid->currentVelocity;
	}
	newVelocity /= neighbours.Num();
	newVelocity.Normalize();
	return newVelocity;
}

FVector ABoids::cohesion(TArray<ABoids*> neighbours){
	if (neighbours.Num() == 0) return FVector::ZeroVector;
	FVector avgLocation = FVector::ZeroVector;
	float inversVal = 1 / neighbours.Num();	
	for (ABoids* Boid : neighbours) {
		avgLocation += Boid->Location *inversVal;
	}	
	return Seek(avgLocation);
}

FVector ABoids::seperation(TArray<ABoids*> neighbours)
{	
	FVector avgFlee;	
	for (ABoids* Boid : neighbours) {
		avgFlee += flee(Boid->Location);
	}
	avgFlee.Normalize();
	return avgFlee;
}

//Checks if the projected location is inside the boundry
FVector ABoids::BoundCheck(FVector CurrentPos){
	//Asigns Varaibles so that its not called multiple times in the function each frame (minor optimisation)
	float turnFactor = Manager->BoundStrictness;
	float BoundsRadius = Manager->BoundsRadius;
	FVector projectedPos = CurrentPos + (GetActorForwardVector() * 200);
	FVector returnVec = FVector::ZeroVector;
	if (projectedPos.X > BoundsRadius){
		returnVec += FVector(-turnFactor, 0, 0);
	}
	if (projectedPos.X < (BoundsRadius * -1)) {
		returnVec += FVector(+turnFactor, 0, 0);
	}
	if (projectedPos.Y > BoundsRadius) {
		returnVec += FVector(0, -turnFactor, 0);
	}
	if (projectedPos.Y < (BoundsRadius * -1)){
		returnVec += FVector(0, turnFactor, 0);
	}
	if (projectedPos.Z > BoundsRadius ) {
		returnVec += FVector(0, 0, -turnFactor);
	}
	if (projectedPos.Z < (BoundsRadius * -1)) {
		returnVec += FVector(0, 0, turnFactor);
	}
	//Return turn Vector boid to turn it back towards the inner bounds
	returnVec.Normalize();
	return returnVec;
}



// Called every frame
void ABoids::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GetActorLocation is only ever called once per frame For optimisation
	

}


void ABoids::UpdateBoid(float DeltaTime) {
	Location = GetActorLocation();
	switch (Manager->behaviour) {		
	case 0: //Tag case
		closestBoidPos = Manager->ClosestBoidPosition(this);
		if (Manager->TaggedBoid == this) {
			//Chaser
			targetVelocity = Seek(closestBoidPos);
			targetVelocity *= Manager->chaserMultiplier;			
		}
		else {//Flee'r
			FVector CurrentPos = Location;	
			targetVelocity = flee(closestBoidPos);
			targetVelocity += BoundCheck(Location);
		}
		if (waitCounter > 0) {
			waitCounter -= DeltaTime;
		}

		break;

	case 1: //Flocking & Wondering case
		closestBoidPosArray = Manager->GetBoidNeighbourHood(this);

		if (targetVelocity.Length() < 100)
		{
			targetVelocity += seperation(closestBoidPosArray) * Manager->seperationWeight;

		}


		if (targetVelocity.Length() < 100)
		{
			targetVelocity += cohesion(closestBoidPosArray) * Manager->CohesionWeight;
		}


		if (targetVelocity.Length() < 100)
		{
			targetVelocity += alignment(closestBoidPosArray) * Manager->alignmentWeight;
		}

		

		if (targetVelocity.Size() < 1.0f) {
			targetVelocity += wander(100.0f, 200.0f, 50.0f);
		}
		targetVelocity.Normalize();

		targetVelocity += BoundCheck(Location);
	break;	

	case 2:
		closestBoidPos = Manager->ClosestBoidPosition(this);
		closestBoidActor = Manager->ClosestBoidActor();
		if (Manager->TaggedBoid == this) {
			//Chaser
			targetVelocity = Persue(closestBoidPos,closestBoidActor);
			targetVelocity *= Manager->chaserMultiplier;
		}
		else {//Flee'r
			FVector CurrentPos = Location;
			targetVelocity = flee(closestBoidPos);

			if (closestBoidActor == Manager->TaggedBoid) { // if closes boid is tagged then EVADE
				targetVelocity = Evade(closestBoidPos, closestBoidActor);
			}				
			targetVelocity += BoundCheck(Location);
		}

		break;
	}


	//Calculates Velocity
	FVector newForce = targetVelocity - currentVelocity;
	currentVelocity += newForce * DeltaTime;
	
	//Calculates next update location
	FVector newlocation = Location;
	newlocation += (currentVelocity * Manager->speed * DeltaTime);

	//Calculate Approximate rotation so boid looks where it is traveling
	FQuat Rotation = (GetActorRotation().Quaternion() + currentVelocity.Rotation().Quaternion() * DeltaTime) ;

	SetActorLocationAndRotation(newlocation,Rotation);
	


}

