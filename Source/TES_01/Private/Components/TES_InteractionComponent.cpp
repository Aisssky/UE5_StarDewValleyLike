// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TES_InteractionComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

UTES_InteractionComponent::UTES_InteractionComponent()
{
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetSphereRadius(150.f);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

}

void UTES_InteractionComponent::Interact()
{
	InteractableQueue.RemoveAll([](AActor* Actor) { return !IsValid(Actor); });

	if (InteractableQueue.IsEmpty()) return;

	AActor* Owner = GetOwner();
	if (!Owner)return;

	AActor* Closest = nullptr;
	float MinDistSq = MAX_FLT;
	FVector OwnerLoc = Owner->GetActorLocation();

	for(AActor* Candidate: InteractableQueue)
	{
		if (!Candidate) continue;
		float DistSq = FVector::DistSquared(OwnerLoc, Candidate->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			Closest = Candidate;
		}
	}
	if (Closest)
	{
		// Perform interaction with the closest actor
		UE_LOG(LogTemp, Log, TEXT("%s is interacting with %s"), *Owner->GetName(), *Closest->GetName());
	}
}


// Called when the game starts
void UTES_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		InteractionSphere->AttachToComponent(Owner->GetRootComponent(),FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &UTES_InteractionComponent::OnBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &UTES_InteractionComponent::OnEndOverlap);
}

void UTES_InteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!Other || Other == GetOwner())return;
	if (InteractableQueue.Contains(Other))return;

	InteractableQueue.Add(Other);

}
void UTES_InteractionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	InteractableQueue.Remove(Other);
}


