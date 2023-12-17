// Fill out your copyright notice in the Description page of Project Settings.


#include "Obsticle.h"

// Sets default values
AObsticle::AObsticle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	UStaticMesh* SphereMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube'")).Object;

	Mesh->SetStaticMesh(SphereMesh);
	this->SetRootComponent(Mesh);

}

// Called when the game starts or when spawned
void AObsticle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObsticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

