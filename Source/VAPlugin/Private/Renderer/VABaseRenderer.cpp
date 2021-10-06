// Fill out your copyright notice in the Description page of Project Settings.


#include "Renderer/VAAbstractRenderer.h"

#include "VAPlugin.h"
#include "VAUtils.h"


// Sets default values
AVAAbstractRenderer::AVAAbstractRenderer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RendererID = "MyBinauralFreeField";
	AuralizationModeController = CreateDefaultSubobject<UVAAuralizationModeController>(TEXT("AuralizationModeController"));
	this->SetActorHiddenInGame(true);

}

// Called when the game starts or when spawned
void AVAAbstractRenderer::BeginPlay()
{
	Super::BeginPlay();

	const std::string sRendererID = TCHAR_TO_UTF8(*RendererID);

	if (AuralizationModeController)
	{
		AuralizationModeController->SetTargetRenderer(sRendererID);
	}

	if (FVAPlugin::GetIsMaster())
	{
		if (AuralizationModeController)
		{
			AuralizationModeController->Initialize();
		}

		if(!FVAPlugin::RendererEnabled(sRendererID))
		{
			FVAUtils::OpenMessageBox("VA Rendering Module: Trying to initialize VA renderer interface with ID '" + RendererID + "'. But renderer was not enabled in VACore ini-file. Commands send via this interface will not be effective.", true);
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
