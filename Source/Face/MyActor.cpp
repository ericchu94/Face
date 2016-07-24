// Fill out your copyright notice in the Description page of Project Settings.

#include "Face.h"
#include "MyActor.h"


// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

float AMyActor::GetMouthLeft(float DeltaSeconds)
{
	return GetMouth(DeltaSeconds);
}

float AMyActor::GetMouthRight(float DeltaSeconds)
{
	return GetMouth(DeltaSeconds);
}


float AMyActor::GetMouth(float DeltaSeconds)
{
	RunningTime += DeltaSeconds;
	float IntPart;
	float FractPart = FMath::Modf(RunningTime, &IntPart);

	if (FMath::Fmod(IntPart, 2) < 1)
		return FractPart;

	return 1 - FractPart;
}
