// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/TES_PlantActor.h"
#include "Data/TES_CropDataAsset.h"

ATES_PlantActor::ATES_PlantActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATES_PlantActor::PlantCrop(UTES_CropDataAsset* Crop)
{
	CropData = Crop;
	CurrentDay = 0;
	bWateredToday = false;
}

void ATES_PlantActor::Water()
{
	bWateredToday = true;
}

void ATES_PlantActor::AdvanceDay()
{
	if(!bWateredToday)return;
	if (!CropData)return;

	CurrentDay++;
	bWateredToday = false;

}

bool ATES_PlantActor::CanHarvest() const
{
	return CropData && CurrentDay>=CropData->TotalGrowthDays;
}

int32 ATES_PlantActor::GetCurrentStage() const
{
	if (!CropData||CropData->GrowthStages<=1)return 0;

	float DaysPerStage = (float)(CropData->TotalGrowthDays) / (CropData->GrowthStages);
	int32 Stage = FMath::FloorToInt(CurrentDay / DaysPerStage);
	return FMath::Min(Stage, CropData->GrowthStages - 1);
}