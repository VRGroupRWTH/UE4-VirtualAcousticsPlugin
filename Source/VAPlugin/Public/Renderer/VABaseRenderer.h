// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AuralizationMode/VAAuralizationModeController.h"

#include "VABaseRenderer.generated.h"


/**
 * Actor representing a renderer in VA. Can adjust auralization modes and muting during runtime.
 * For more functionality refer to an Actor representing a specific renderer.
 */
UCLASS(ClassGroup = (VA), HideCategories = ("Actor", "Cooking", "Collision", "Replication", "Rendering", "Input", "LOD"))
class AVABaseRenderer : public AActor
{
	GENERATED_BODY()

protected:
	// Name of the renderer specified in VACore.ini
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Renderer ID", Category = "General"))
	FString RendererID;

	// Controller for global auraliztion modes
	UPROPERTY(EditAnywhere, Instanced, NoClear, meta = (DisplayName = "Auraliztion Mode Controller", Category = "Auralization Modes"))
	UVAAuralizationModeController* AuralizationModeController;

public:
	// Gets auralization mode controller
	UFUNCTION(BlueprintCallable)
	UVAAuralizationModeController* GetAuralizationModeController() const;
	
public:
	// Sets default values for this actor's properties
	AVABaseRenderer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
