// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TES_PaperZDCharacter.h"
#include "InputActionValue.h"
#include "TES_PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS(Blueprintable)
class TES_01_API ATES_PlayerCharacter : public ATES_PaperZDCharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly,Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	void OnMoveTriggered(const FInputActionValue& value);
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
