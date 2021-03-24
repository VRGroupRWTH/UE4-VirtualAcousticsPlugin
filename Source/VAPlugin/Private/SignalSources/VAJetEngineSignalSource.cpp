// Fill out your copyright notice in the Description page of Project Settings.


#include "VAJetEngineSignalSource.h"

#include "VAUtils.h"
#include "VAPlugin.h"


void UVAJetEngineSignalSource::Initialize()
{
	if (bInitialized)
	{
		FVAUtils::LogStuff("[UVAJetEngineSignalSource::Initialize()]: Signal source is already initialized, aborting...", true);
		return;
	}


	sID = FVAPlugin::CreateSignalSourcePrototype(this);
	if (sID == "-1")
	{
		FVAUtils::LogStuff("[UVAJetEngineSignalSource::Initialize()]: Error initializing Jet Engine Signal Source", true);
		return;
	}


	bInitialized = true;

	if (!FVAPlugin::SetSignalSourceParameter(sID, "rpm", JetRPM))
	{
		FVAUtils::LogStuff("[UVAJetEngineSignalSource::Initialize()]: Could not set Jet RPM during initialization", true);
	}
}

std::string UVAJetEngineSignalSource::GetPrototypeName()
{
	return "jet_engine";
}

bool UVAJetEngineSignalSource::SetJetRPM(float fRPM)
{
	if (!bInitialized)
		return false;

	if (!FVAPlugin::SetSignalSourceParameter(sID, "rpm", fRPM))
		return false;
	
	JetRPM = fRPM;
	return true;
}

float UVAJetEngineSignalSource::GetJetRPM() const
{
	return JetRPM;
}
