// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "VAAbstractSignalSource.generated.h"


UCLASS( ClassGroup=(VA), Abstract, HideCategories=("Tags", "AssetUserData", "Activation", "Collision", "Cooking") )
class UVAAbstractSignalSource : public UObject
{
	GENERATED_BODY()

protected:
	// ID representing the internal va signal source. Invalid ID before initialization.
	std::string ID;
	bool bInitialized = false;

public:	
	// Sets default values for this component's properties
	UVAAbstractSignalSource();
	// Creates the signal source in VA and sets the ID accordingly
	virtual void Initialize() PURE_VIRTUAL(UVAAbstractSignalSource::Initialize, );

	//Used for prototype interface of VA, to implement arbitrary signal source.
	//  Only overload in derived class if corresponding signal source can be created via prototype function
	virtual std::string GetPrototypeName();

	// Returns the VA signal source identifier (string)
	std::string GetID() const;

	// Checks whether this signal source was properly initialized and has a valid ID
	bool IsValid() const;

	static bool IsValidID(const std::string& ID);
};
