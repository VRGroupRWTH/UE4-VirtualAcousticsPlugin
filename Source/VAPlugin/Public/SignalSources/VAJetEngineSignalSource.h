// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SignalSources/VAAbstractSignalSource.h"
#include "VAJetEngineSignalSource.generated.h"

/**
 * 
 */
UCLASS()
class VAPLUGIN_API UVAJetEngineSignalSource : public UVAAbstractSignalSource
{
	GENERATED_BODY()

protected:
	// Set rotations per minute of the jet
	UPROPERTY(EditAnywhere, meta = (DisplayName = "RPM", Category = "Jet Engine"))
		float JetRPM = 1000.0f;
	
public:
	UVAJetEngineSignalSource() = default;
	// Creates the signal source in VA and sets the ID accordingly
	void Initialize() override;

	// Sets the rounds per minute (RPM) of the jet
	bool SetJetRPM(float fRPM);
	// Returns the rounds per minute (RPM) of the jet
	float GetJetRPM() const;

};
