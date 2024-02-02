// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SceneManagerCFX.generated.h"


/**
 * 
 */
UCLASS()
class CIVILFXCORE_API USceneManagerCFX : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void SetSceneFOV(float InFoV);
	UFUNCTION(BlueprintCallable)
	float GetSceneFOV() const;

	UFUNCTION(BlueprintCallable)
	void SetExistingFoliageEnabled(bool bInEnabled);
	UFUNCTION(BlueprintCallable)
	void SetProposedFoliageEnabled(bool bInEnabled);
	UFUNCTION(BlueprintCallable)
	void SetProposedLandscapingEnabled(bool bInEnabled);
	UFUNCTION(BlueprintCallable)
	void SetExistingLightsEnabled(bool bInEnabled);
	UFUNCTION(BlueprintCallable)
	void SetProposedLightsEnabled(bool bInEnabled);
	bool GetExistingFoliageEnabled() const;
	bool GetProposedFoliageEnabled() const;
	bool GetProposedLandscapingEnabled() const;
	bool GetExistingLightsEnabled() const;
	bool GetProposedLightsEnabled() const;


	UFUNCTION(BlueprintCallable)
	void SetLabelsEnabled(bool bInEnabled);
	bool GetLabelsEnabled() const;

	UFUNCTION(BlueprintCallable)
	void SetRayTracingEnabled(bool bInEnabled);
	bool GetRayTracingEnabled() const;

	UFUNCTION(BlueprintCallable)
	void SetTrafficEnabled(bool bInEnabled);
	UFUNCTION(BlueprintCallable)
	bool GetTrafficEnabled() const;

	//UFUNCTION(BlueprintCallable)
	//void SetAAOption(int32 OptionIndex);

private:
	float FoV;
	bool bExistingFoliageEnabled;
	bool bProposedFoliageEnabled;
	bool bProposedLandscapingEnabled;
	bool bExistingLightsEnabled;
	bool bProposedLightsEnabled;
	bool bLabelsEnabled;
	bool bPedEnabled;
	bool bRTEnabled;
	bool bTrafficEnabled = true;
#if WITH_EDITORONLY_DATA
	int32 CachedMSAACount;
#endif

	UPROPERTY(Transient)
	TArray<AActor*> CachedLabels;
	UPROPERTY(Transient)
	TArray<AActor*> CachedFoliages;
};
