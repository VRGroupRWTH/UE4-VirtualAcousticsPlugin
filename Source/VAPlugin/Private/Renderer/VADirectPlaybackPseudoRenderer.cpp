// Fill out your copyright notice in the Description page of Project Settings.


#include "Renderer/VADirectPlaybackPseudoRenderer.h"

#include "VAReceiverActor.h"
#include "SoundSource/VAAbstractSourceComponent.h"



void AVADirectPlaybackPseudoRenderer::ActivateChannel(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const
{
	Super::SetChannelIR(VAReceiver, VASource, "DiracIR.wav", ChannelID);
}

void AVADirectPlaybackPseudoRenderer::DeactivateChannel(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const
{
	Super::SetChannelIR(VAReceiver, VASource, "NullIR.wav", ChannelID);
}

void AVADirectPlaybackPseudoRenderer::SetSingleChannelActive(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const
{
	Super::SetAllIRs(VAReceiver, VASource, "NullIR.wav");
	ActivateChannel(VAReceiver, VASource, ChannelID);
}

