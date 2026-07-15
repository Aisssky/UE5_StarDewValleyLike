// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TES_PaperZDCharacter.generated.h"

UENUM(BlueprintType)
enum class TES_Direction4 : uint8
{
	Down = 0,
	Left = 1,
	Right = 3,
	Up = 2
};


UCLASS(Abstract,Blueprintable)
class TES_01_API ATES_PaperZDCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATES_PaperZDCharacter();

	UFUNCTION(BlueprintCallable, Category = "Direction")
	void UpdateDirection(FVector2D MovementInput);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Direction")
	TES_Direction4 GetCurrentDirection() const { return CurrentDirection; }
protected:
	
	UPROPERTY(BlueprintReadOnly,Category="Direction")
	TES_Direction4 CurrentDirection=TES_Direction4::Down;

	

};
