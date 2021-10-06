// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Renderer/VABaseRenderer.h"
#include "VAGenericPathRenderer.generated.h"

class UVAAbstractSourceComponent;
class AVAReceiverActor;

UCLASS(ClassGroup = (VA))
class VAPLUGIN_API AVAGenericPathRenderer : public AVABaseRenderer
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVAGenericPathRenderer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Set the IR of a single channel using a .wav file (ChannelIDs starts at 1!)
	UFUNCTION(BlueprintCallable)
	void SetChannelIR(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, const FString& WaveFilename, int ChannelID) const;

	// Set the IR of all channels using a .wav file
	UFUNCTION(BlueprintCallable)
	void SetAllIRs(const AVAReceiverActor* VAReceiver, const UVAAbstractSourceComponent* VASource, const FString& WaveFilename) const;

};
