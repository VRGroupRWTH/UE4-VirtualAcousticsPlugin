// Fill out your copyright notice in the Description page of Project Settings.


#include "Renderer/VADirectPlaybackPseudoRenderer.h"

#include "VAReceiverActor.h"
#include "SoundSource/VAAbstractSourceComponent.h"


#define DIRAC_IR_WAV "direct_playback_renderer/DiracIR.wav"
#define NULL_IR_WAV "direct_playback_renderer/NullIR.wav"

void AVADirectPlaybackPseudoRenderer::ActivateChannel(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const
{
	Super::SetChannelIR(VAReceiver, VASource, DIRAC_IR_WAV, ChannelID);
}

void AVADirectPlaybackPseudoRenderer::DeactivateChannel(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const
{
	Super::SetChannelIR(VAReceiver, VASource, NULL_IR_WAV, ChannelID);
}

void AVADirectPlaybackPseudoRenderer::SetSingleChannelActive(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const
{
	Super::SetAllIRs(VAReceiver, VASource, NULL_IR_WAV);
	ActivateChannel(VAReceiver, VASource, ChannelID);
}

