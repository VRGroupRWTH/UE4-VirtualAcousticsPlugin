#pragma once

#include "CoreMinimal.h"
#include "Class.h"


UENUM(BlueprintType)
namespace EPlayAction
{
	enum Type
	{
		NoPlayAction = -1	UMETA(Hidden),
		Stop = 0,
		Pause = 1,
		Play = 2
	};
}

UENUM(BlueprintType)
namespace EConnectionSetting
{
	enum Type
	{
		Automatic = 0,
		Localhost = 1,
		Cave = 2,
		Manual = 3
	};
}


UENUM(BlueprintType)
namespace EDirectivitySetting
{
	enum Type
	{
		DefaultHRIR = 0,
		Phoneme = 1,
		NoDirectivity = 2,
		ManualFile = 3
	};
}


UENUM(BlueprintType)
namespace EMovement
{
	enum Type
	{
		MoveWithObject = 0,
		ObjectSpawnPoint = 1,
		AttachToBone = 2,
	};
}

