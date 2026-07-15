// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TES_CropDataAsset.generated.h"

UCLASS(BlueprintType)
class TES_01_API UTES_CropDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crop")
	FText CropName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crop")
	int32 Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crop")
	int32 TotalGrowthDays = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crop")
	int32 GrowthStages = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crop")
	FName HarvestItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crop")
	int32 HarvestMin = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crop")
	int32 HarvestMax = 3;

	
};
