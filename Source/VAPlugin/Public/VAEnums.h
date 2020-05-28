#pragma once

#include "CoreMinimal.h"
#include "Class.h"


UENUM(BlueprintType)
namespace EAddress
{
	enum Type
	{
		Automatic,
		Localhost,
		Cave,
		Manual
	};
}

UENUM(BlueprintType)
namespace EDir
{
	enum Type
	{
		DefaultHRIR,
		ManualFile,
		Phoneme,
		NoDirectivity
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


UENUM(BlueprintType)
namespace EPlayAction
{
	enum Type
	{
		NoPlayAction = -1	UMETA(Hidden),
		Stop = 0			,
		Pause = 1			,
		Play = 2			
	};
}
