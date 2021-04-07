// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAEnums.h"
#include "../../Private/SignalSources/VAAudiofileManager.h"

#include <string>

#include "CoreMinimal.h"
#include "SignalSources/VAAbstractSignalSource.h"
#include "VAAudiofileSignalSource.generated.h"


/**
 * 
 */
UCLASS(ClassGroup = (VA))
class VAPLUGIN_API UVAAudiofileSignalSource : public UVAAbstractSignalSource
{
	GENERATED_BODY()
	
public:
	UVAAudiofileSignalSource() = default;

	// Creates the signal source in VA and sets the ID accordingly
	void Initialize() override;

	// *** Playback Settings *** //

	UFUNCTION(BlueprintCallable)
	bool Play();
	UFUNCTION(BlueprintCallable)
	bool PlayFromTime(float fTime);
	UFUNCTION(BlueprintCallable)
	bool Pause();
	UFUNCTION(BlueprintCallable)
	bool Stop();

	// *** Audiofile related *** //
	
	UFUNCTION(BlueprintCallable)
	// (Pre-) loads an audiofile for later usage
	//   Internally, VA creates a signal source and the ID is stored. See FVAAudiofileManager
	//   @return True on success
	bool PreLoadAudiofile(FString Filename);

	UFUNCTION(BlueprintCallable)
	// Switches the internal signal source to match the corresponding audiofile. Creates a new signal source, if audiofile is not pre-loaded.
	//    Additionally, copies sets the loop and play back action of the new internal signal source accordingly.
	//    Raises an even broadcasting the ID of the internal audiofile signal source which is "-1", if signal source could not be created.
	//    @return True on success
	bool SetAudiofile(FString Filename);

	// *** Setter *** //

	UFUNCTION(BlueprintCallable)
	bool SetLoop(bool bLoopN);
	UFUNCTION(BlueprintCallable)
	bool SetPlayBackPosition(float Time);
	UFUNCTION(BlueprintCallable)
	bool SetPlayAction(int Action);


	// *** Getter *** //

	UFUNCTION(BlueprintCallable)
	FString GetFilename() const;
	UFUNCTION(BlueprintCallable)
	bool GetLoop() const;
	UFUNCTION(BlueprintCallable)
	EPlayAction::Type GetPlayActionEnum() const;

	int GetPlayAction() const;

	// *** Events/Delegates *** //

	DECLARE_EVENT_OneParam(UVAAudiofileSignalSource, FChangedAudiofileEvent, const std::string&)
	//Get the delegate for the event broadcasted on an audiofile change, which provides the new signal source ID
	FChangedAudiofileEvent& OnAudiofileChanged() { return AudiofileChangedEvent; }

protected:
	// Copies the loop bool and play action of the current signal source to the one with given ID
	// @return True on success
	bool CopySignalSourceSettings(const std::string& ID);

private:
	FChangedAudiofileEvent AudiofileChangedEvent;


protected:
	// Action of the sound source at the first tick
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Action", Category = "Audio file"))
		TEnumAsByte<EPlayAction::Type> StartingPlayAction = EPlayAction::Type::Stop;

	// Name of Sound file. Folder are possible too: "folder/soundfile.wav"
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Filename", Category = "Audio file"))
		FString Filename = "WelcomeToVA.wav";

	// Sets Buffer to a specific time stamp when playing back at the first tick (see Action)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Play from x [s]", Category = "Audio file"))
		float StartingTime = 0.0f;

	// Check if the sound should be played back in a loop
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop", Category = "Audio file"))
		bool bLoop = false;

private:
	FVAAudiofileManager AudiofileManager;
};
