// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundSource/VAAudiofileSourceComponent.h"


// Sets default values for this component's properties
UVAAudiofileSourceComponent::UVAAudiofileSourceComponent() : Super()
{
	SignalSource = CreateDefaultSubobject<UVAAudiofileSignalSource>("AudiofileSignalSource");
}

UVAAudiofileSignalSource* UVAAudiofileSourceComponent::GetAudiofileSignalSource() const
{
	return Cast<UVAAudiofileSignalSource>(SignalSource);
}

bool UVAAudiofileSourceComponent::Play()
{
	return GetAudiofileSignalSource()->Play();
}

bool UVAAudiofileSourceComponent::PlayFromTime(float fTime)
{
	return GetAudiofileSignalSource()->PlayFromTime(fTime);
}

bool UVAAudiofileSourceComponent::Pause()
{
	return GetAudiofileSignalSource()->Pause();
}

bool UVAAudiofileSourceComponent::Stop()
{
	return GetAudiofileSignalSource()->Stop();
}

// Called when the game starts
void UVAAudiofileSourceComponent::BeginPlay()
{
	Super::BeginPlay();
	BindSignalSourceEvents();
}

void UVAAudiofileSourceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindSignalSourceEvents();
	Super::EndPlay(EndPlayReason);
}



void UVAAudiofileSourceComponent::OnAudiofileChanged(const std::string& newSignalSourceID)
{
	SetSignalSourceID(newSignalSourceID);
}


void UVAAudiofileSourceComponent::BindSignalSourceEvents()
{
	if (!GetAudiofileSignalSource()->OnAudiofileChanged().IsBoundToObject(this))
	{
		AudiofileChangedDelegate = GetAudiofileSignalSource()->OnAudiofileChanged().AddUObject(this, &UVAAudiofileSourceComponent::OnAudiofileChanged);
	}
}

void UVAAudiofileSourceComponent::UnbindSignalSourceEvents()
{
	if (AudiofileChangedDelegate.IsValid())
	{
		GetAudiofileSignalSource()->OnAudiofileChanged().Remove(AudiofileChangedDelegate);
		AudiofileChangedDelegate.Reset();
	}
}
