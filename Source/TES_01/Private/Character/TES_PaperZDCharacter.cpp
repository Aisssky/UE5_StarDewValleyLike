// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TES_PaperZDCharacter.h"

// Sets default values
ATES_PaperZDCharacter::ATES_PaperZDCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATES_PaperZDCharacter::UpdateDirection(FVector2D MovementInput)
{
	if (MovementInput.IsNearlyZero()) {
		return;
	}

	if(FMath::Abs(MovementInput.X)> FMath::Abs(MovementInput.Y))
	{
		CurrentDirection = (MovementInput.X > 0.0f) ? TES_Direction4::Right : TES_Direction4::Left;
	}
	else
	{
		CurrentDirection = (MovementInput.Y > 0.0f) ? TES_Direction4::Up : TES_Direction4::Down;
	}
}
 