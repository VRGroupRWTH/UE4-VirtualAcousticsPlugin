// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SignalSources/VAJetEngineSignalSource.h"
#include "SoundSource/VAAbstractSourceComponent.h"

#include "VAJetEngineSourceComponent.generated.h"

UCLASS( ClassGroup=(VA), meta=(BlueprintSpawnableComponent) )
class VAPLUGIN_API UVAJetEngineSourceComponent : public UVAAbstractSourceComponent
{
	GENERATED_BODY()
public:

	UVAJetEngineSourceComponent();

	//holds most interface calls to the signal source
	UFUNCTION(BlueprintCallable)
	UVAJetEngineSignalSource* GetJetEngineSignalSource() const;

};
