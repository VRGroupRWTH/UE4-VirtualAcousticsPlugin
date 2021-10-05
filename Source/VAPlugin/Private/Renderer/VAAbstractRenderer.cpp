// Fill out your copyright notice in the Description page of Project Settings.


#include "Renderer/VAAbstractRenderer.h"

#include "VAPlugin.h"


// Sets default values
AVAAbstractRenderer::AVAAbstractRenderer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AuralizationModeController = CreateDefaultSubobject<UVAAuralizationModeController>(TEXT("AuralizationModeController"));

}

// Called when the game starts or when spawned
void AVAAbstractRenderer::BeginPlay()
{
	Super::BeginPlay();

	if (AuralizationModeController)
	{
		AuralizationModeController->SetTargetRenderer(TCHAR_TO_UTF8(*RendererName));
	}

	if (FVAPlugin::GetIsMaster())
	{
		if (AuralizationModeController)
		{
			AuralizationModeController->Initialize();
		}
	}
	
}

// Called every frame
void AVAAbstractRenderer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


UVAAuralizationModeController* AVAAbstractRenderer::GetAuralizationModeController() const
{
	return AuralizationModeController;
}
