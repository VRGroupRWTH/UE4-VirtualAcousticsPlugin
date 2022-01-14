// Fill out your copyright notice in the Description page of Project Settings.

#include "Renderer/VABaseRenderer.h"

#include "Utility/VirtualRealityUtilities.h"


#include "VAPlugin.h"
#include "VAUtils.h"


// Sets default values
AVABaseRenderer::AVABaseRenderer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RendererID = "MyBinauralFreeField";
	bRendererInitiallyMuted = false;
	AuralizationModeController = CreateDefaultSubobject<UVAAuralizationModeController>(TEXT("AuralizationModeController"));
	this->SetActorHiddenInGame(true);

}

// Called when the game starts or when spawned
void AVABaseRenderer::BeginPlay()
{
	Super::BeginPlay();

	const std::string sRendererID = TCHAR_TO_UTF8(*RendererID);

	if (AuralizationModeController)
	{
		AuralizationModeController->SetTargetRenderer(sRendererID);
	}

	if ( FVAPlugin::GetUseVA() && UVirtualRealityUtilities::IsMaster() )
	{
		if (AuralizationModeController)
		{
			AuralizationModeController->Initialize();
		}

		if(!FVAPlugin::RendererEnabled(sRendererID))
		{
			FVAUtils::OpenMessageBox("VA Rendering Module: Trying to initialize VA renderer interface with ID '" + RendererID + "'. But renderer was not enabled in VACore ini-file. Commands send via this interface will not be effective.", true);
		}
		else
		{
			SetRendererMute(bRendererInitiallyMuted);
		}
	}
	
}

// Called every frame
void AVABaseRenderer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


UVAAuralizationModeController* AVABaseRenderer::GetAuralizationModeController() const
{
	return AuralizationModeController;
}

bool AVABaseRenderer::SetRendererMute(bool bMute)
{
	const std::string sRendererID = TCHAR_TO_UTF8(*RendererID);
	return FVAPlugin::SetRendererMute(sRendererID, bMute);
}

bool AVABaseRenderer::GetRendererMuted() const
{
	return (bool)FVAPlugin::GetRendererMuted( TCHAR_TO_UTF8(*RendererID) );
}

bool AVABaseRenderer::ToggleRendererMute()
{
	int Muted = FVAPlugin::GetRendererMuted(TCHAR_TO_UTF8(*RendererID));
	if (Muted < 0)
	{
		FVAPlugin::ProcessException("ToggleRendererMute()", "Could not read current renderer mute state.");
		return false;
	}

	bool bNewMuteState = !(bool)Muted;
	if (!SetRendererMute(bNewMuteState))
	{
		FVAPlugin::ProcessException("ToggleRendererMute()", "Could not set new renderer mute state.");
		return !bNewMuteState;
	}
	return bNewMuteState;
}