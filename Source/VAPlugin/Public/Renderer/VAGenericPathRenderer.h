// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Renderer/VAAbstractRenderer.h"
#include "VAGenericPathRenderer.generated.h"

UCLASS()
class VAPLUGIN_API AVAGenericPathRenderer : public AVAAbstractRenderer
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

};
