// Fill out your copyright notice in the Description page of Project Settings.


#include "Renderer/VAGenericPathRenderer.h"

#include "VAUtils.h"
#include "VAReceiverActor.h"
#include "SoundSource/VAAbstractSourceComponent.h"

#include "VAStruct.h"

// Sets default values
AVAGenericPathRenderer::AVAGenericPathRenderer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Setting default name for this renderer
	RendererID = "MyGenericRenderer";

}

// Called when the game starts or when spawned
void AVAGenericPathRenderer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVAGenericPathRenderer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Helper function creating and sending VAStruct
void SetIR(FString RendererName, const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, const FString& WaveFilename, int ChannelID)
{
	if (!VAReceiver)
	{
		FVAPlugin::ProcessException("AVAGenericPathRenderer::SetIR()", "Pointer to VAReceiver is Null.");
		return;
	}
	if (!VASource)
	{
		FVAPlugin::ProcessException("AVAGenericPathRenderer::SetIR()", "Pointer to VASource is Null.");
		return;
	}

	CVAStruct RendererParameters;
	RendererParameters["receiver"] = VAReceiver->GetReceiverID();
	RendererParameters["source"] = VASource->GetSoundSourceID();
	RendererParameters["filepath"] = TCHAR_TO_UTF8(*WaveFilename);
	if (ChannelID > 0)
	{
		RendererParameters["channel"] = ChannelID;
	}
	FVAPlugin::SetRendererParameters(TCHAR_TO_UTF8(*RendererName), RendererParameters);
}

void AVAGenericPathRenderer::SetChannelIR(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, const FString& WaveFilename, int ChannelID) const
{
	if (ChannelID <= 0)
	{
		FVAPlugin::ProcessException("AVAGenericPathRenderer::SetChannelIR()", "Channel ID must be >= 1");
		return;
	}
	SetIR(RendererID, VAReceiver, VASource, WaveFilename, ChannelID);
}

void AVAGenericPathRenderer::SetAllIRs(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, const FString& WaveFilename) const
{
	SetIR(RendererID, VAReceiver, VASource, WaveFilename, -1);
}

