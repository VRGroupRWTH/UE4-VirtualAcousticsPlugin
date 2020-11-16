#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/DeveloperSettings.h"
#include "VASettings.generated.h"


UCLASS(config = Engine, defaultconfig, meta = (DisplayName = "Virtual Acoustics (VA)"))
class UVASettings : public UDeveloperSettings
{
  GENERATED_BODY()

public:

  UPROPERTY(EditAnywhere, config, Category = "General")
    FString VALauncherPath = "../VAServer";
};
