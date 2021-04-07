// Fill out your copyright notice in the Description page of Project Settings.


#include "SignalSources/VAJetEngineSignalSource.h"

#include "VAUtils.h"
#include "VAPlugin.h"


void UVAJetEngineSignalSource::Initialize()
{
	if (bInitialized)
	{
		FVAUtils::LogStuff("[UVAJetEngineSignalSource::Initialize()]: Signal source is already initialized, aborting...", true);
		return;
	}


	ID = FVAPlugin::CreateSignalSourcePrototype(this);
	if (!IsValidID(ID))
	{
		FVAUtils::LogStuff("[UVAJetEngineSignalSource::Initialize()]: Error initializing Jet Engine Signal Source", true);
		return;
	}


	bInitialized = true;

	if (!FVAPlugin::SetSignalSourceParameter(ID, "rpm", JetRPM))
	{
		FVAUtils::LogStuff("[UVAJetEngineSignalSource::Initialize()]: Could not set Jet RPM during initialization", true);
	}
}

std::string UVAJetEngineSignalSource::GetPrototypeName()
{
	return "jet_engine";
}

bool UVAJetEngineSignalSource::SetJetRPM(float RPM)
{
	if (!bInitialized)
		return false;

	if (!FVAPlugin::SetSignalSourceParameter(ID, "rpm", RPM))
		return false;
	
	JetRPM = RPM;
	return true;
}

float UVAJetEngineSignalSource::GetJetRPM() const
{
	return JetRPM;
}
