// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TES_PlantActor.generated.h"

class UTES_CropDataAsset;

UCLASS(Blueprintable)
class TES_01_API ATES_PlantActor : public AActor
{
	GENERATED_BODY()
	
public:	

	ATES_PlantActor();

	void PlantCrop(UTES_CropDataAsset* Crop);

	void Water();

	void AdvanceDay();

	bool CanHarvest() const;

	int32 GetCurrentStage() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant")
	TObjectPtr<UTES_CropDataAsset> CropData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plant")
	int32 CurrentDay = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plant")
	bool bWateredToday = false;

};
