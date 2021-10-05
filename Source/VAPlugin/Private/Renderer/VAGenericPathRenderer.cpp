// Fill out your copyright notice in the Description page of Project Settings.


#include "Renderer/VAGenericPathRenderer.h"

// Sets default values
AVAGenericPathRenderer::AVAGenericPathRenderer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setting default name for this renderer
	if (RendererName.IsEmpty())
	{
		RendererName = "MyGenericRenderer";
	}

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

