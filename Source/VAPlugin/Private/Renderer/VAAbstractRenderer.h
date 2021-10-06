// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AuralizationMode/VAAuralizationModeController.h"

#include "VAAbstractRenderer.generated.h"


/**
 * Abstract base class (Actor) representing a renderer in VA
 */
UCLASS(ClassGroup = (VA), Abstract, HideCategories = ("Actor", "Cooking", "Collision", "Replication", "Input", "LOD"))
class AVAAbstractRenderer : public AActor
{
	GENERATED_BODY()

protected:
	// Name of the renderer specified in VACore.ini
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Renderer Name", Category = "General")) // CanEditChange used
	FString RendererName = "MyGenericPathRenderer";

	// Controller for global auraliztion modes
	UPROPERTY(EditAnywhere, Instanced, NoClear, meta = (DisplayName = "Auraliztion Mode Controller", Category = "Auralization Modes"))
	UVAAuralizationModeController* AuralizationModeController;

public:
	// Gets auralization mode controller
	UFUNCTION(BlueprintCallable)
	UVAAuralizationModeController* GetAuralizationModeController() const;
	
public:	
	// Sets default values for this actor's properties
	AVAAbstractRenderer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
