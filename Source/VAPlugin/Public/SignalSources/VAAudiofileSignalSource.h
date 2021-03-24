// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAEnums.h"

#include "CoreMinimal.h"
#include "SignalSources/VAAbstractSignalSource.h"
#include "VAAudiofileSignalSource.generated.h"

/**
 * 
 */
UCLASS()
class VAPLUGIN_API UVAAudiofileSignalSource : public UVAAbstractSignalSource
{
	GENERATED_BODY()

protected:
	// Action of the sound source at the first tick
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Action", Category = "Audio file"))
		TEnumAsByte<EPlayAction::Type> StartingPlayAction = EPlayAction::Type::Stop;

	// Name of Sound file. Folder are possible too: "folder/soundfile.wav"
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Filename", Category = "Audio file"))
		FString SoundFile = "WelcomeToVA.wav";

	// Sets Buffer to a specific time stamp when playing back at the first tick (see Action)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Play from x [s]", Category = "Audio file"))
		float StartingTime = 0.0f;

	// Check if the sound should be played back in a loop
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop", Category = "Audio file"))
		bool bLoop = false;
	
public:
	UVAAudiofileSignalSource() = default;

	// Creates the signal source in VA and sets the ID accordingly
	void Initialize() override;

	UFUNCTION(BlueprintCallable)
	bool Play();
	UFUNCTION(BlueprintCallable)
	bool PlayFromTime(float fTime);
	UFUNCTION(BlueprintCallable)
	bool Pause();
	UFUNCTION(BlueprintCallable)
	bool Stop();

	// Setter
	bool SetLoop(bool bLoopN);
	bool SetPlayBackPosition(float fTime);
	bool SetPlayAction(int iAction);


	// Getter
	FString GetFileName() const;
	bool GetLoop() const;
	int GetPlayAction() const;
};
