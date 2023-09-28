// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChevronCreator.generated.h"

UCLASS()
class CIVILFXCORE_API AChevronCreator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChevronCreator();

	void BuildChevronMesh(const TArray<FVector>& InLocations, FLinearColor ChevronColor, int32 ChevronWidthMultiplier=1, float ChevronSpeed=1.5f, bool bRetraceLocation=true, float HeightOffset=100.f);

private:
	static class UStaticMesh* MeshToBuild;
	static float MeshLength_Meter;

	class UMaterialInstanceDynamic* Mat;


};
