// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundSource/VAJetEngineSourceComponent.h"


// Sets default values for this component's properties
UVAJetEngineSourceComponent::UVAJetEngineSourceComponent() : Super()
{
	SignalSource = CreateDefaultSubobject<UVAJetEngineSignalSource>("JetEngineSignalSource");
}

UVAJetEngineSignalSource* UVAJetEngineSourceComponent::GetJetEngineSignalSource() const
{
	return Cast<UVAJetEngineSignalSource>(SignalSource);
}


