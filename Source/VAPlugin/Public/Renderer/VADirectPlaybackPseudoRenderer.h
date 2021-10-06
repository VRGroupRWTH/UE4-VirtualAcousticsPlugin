// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Renderer/VAGenericPathRenderer.h"
#include "VADirectPlaybackPseudoRenderer.generated.h"

/**
 * Pseudo renderer to playback a signal on a specific channel without any processing.
 * Based on GenericPathRenderer using NUll-IRs and Dirac impulses
 */
UCLASS(ClassGroup = (VA))
class VAPLUGIN_API AVADirectPlaybackPseudoRenderer : public AVAGenericPathRenderer
{
	GENERATED_BODY()
	
public:
	// Activates channel with given ID (starting at 1)
	UFUNCTION(BlueprintCallable)
	void ActivateChannel(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const;

	// Deactivates channel with given ID (starting at 1)
	UFUNCTION(BlueprintCallable)
	void DeactivateChannel(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const;

	// Activates the channel with given ID (starting at 1) and deactivates all others
	UFUNCTION(BlueprintCallable)
	void SetSingleChannelActive(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, int ChannelID) const;

};
