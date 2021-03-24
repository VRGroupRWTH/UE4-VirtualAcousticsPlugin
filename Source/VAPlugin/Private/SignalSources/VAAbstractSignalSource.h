// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VAAbstractSignalSource.generated.h"


UCLASS( ClassGroup=(Custom), Abstract, meta=(BlueprintSpawnableComponent), HideDropdown, HideCategories=("Tags", "AssetUserData", "Activation", "Collision", "Cooking") )
class UVAAbstractSignalSource : public UActorComponent
{
	GENERATED_BODY()

protected:
	std::string sID = "-1";
	bool bInitialized = false;

public:	
	// Sets default values for this component's properties
	UVAAbstractSignalSource() = default;
	// Creates the signal source in VA and sets the ID accordingly
	virtual void Initialize() PURE_VIRTUAL(UVAAbstractSignalSource::Initialize, );

	// Returns the VA signal source identifier (string)
	std::string GetID() const;

	// Checks whether this signal source was properly initialized and has a valid ID
	bool IsValid() const;
};
