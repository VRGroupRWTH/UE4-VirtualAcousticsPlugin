// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SignalSources/VAAudiofileSignalSource.h"
#include "SoundSource/VAAbstractSourceComponent.h"

#include "VAAudiofileSourceComponent.generated.h"

UCLASS( ClassGroup=(VA), meta=(BlueprintSpawnableComponent) )
class VAPLUGIN_API UVAAudiofileSourceComponent : public UVAAbstractSourceComponent
{
	GENERATED_BODY()
public:

	UVAAudiofileSourceComponent();

	//holds most interface calls to the signal source
	UFUNCTION(BlueprintCallable)
	UVAAudiofileSignalSource* GetAudiofileSignalSource() const;

	//some convenience methods, forwarding to the UVAAudiofileSignalSource directly
	UFUNCTION(BlueprintCallable)
	bool Play();
	UFUNCTION(BlueprintCallable)
	bool PlayFromTime(float fTime); //in seconds
	UFUNCTION(BlueprintCallable)
	bool Pause();
	UFUNCTION(BlueprintCallable)
	bool Stop();
	
	

protected:

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnAudiofileChanged(const std::string& newSignalSourceID);

	// *** Event/Delegates *** //

	void BindSignalSourceEvents();
	void UnbindSignalSourceEvents();

private:

	FDelegateHandle AudiofileChangedDelegate;
	
};
