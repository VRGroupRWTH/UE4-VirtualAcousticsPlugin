// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "VASourceComponent.h"

DECLARE_LOG_CATEGORY_EXTERN(VALog, Log, All);

//forward declarations:
class VAQuat;
class VAVec3;

class VAUtils
{
public:

	enum viewEnum
	{
		unclear,
		ThirdPerson,
		HMD,
		Cave
	};


	// Open Message Box with Text. bool error expresses whether to use excl. marks ++ //
	static void openMessageBox(FString text, bool error = false);
	static FString addExclamationMarkInChar(FString text);

	// Checking LibraryHandle  //
	static bool checkLibraryHandle(void* LibraryHandle);

	// Copy FVector* to VAVec3* //
	static bool fVecToVAVec3(FVector& VecF, VAVec3& VecVA);

	// Copy FQuat* to QuatVA* //
	static bool fQuatToVAQuat(FQuat& quatF, VAQuat& quatVA);

	// Transforms Vec & Rotator in VA Coordinate System // 
	static FVector toVACoordinateSystem(FVector vecF, float scale);
	static FRotator toVACoordinateSystem(FRotator rotF);

	// Compute reflected Position from wall
	static FVector computeReflectedPos(AVAReflectionWall* wall, FVector pos);
	static FRotator computeReflectedRot(AVAReflectionWall* wall, FRotator rot);

	// Log into VALog
	static void logStuff(FString text, bool error = false);

	// Transform EPlayAction TODO: Get rid of
	static int EPlayActionToVAAction(EPlayAction action);
	static EPlayAction VAActionToEPlayAction(int action);


private:
};
