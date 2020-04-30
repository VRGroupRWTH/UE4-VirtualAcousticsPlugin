// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"

DECLARE_LOG_CATEGORY_EXTERN(VALog, Log, All);

//forward declarations:
class VAQuat;
class VAVec3;
class AVAReflectionWall;

class FVAUtils
{
public:



	// Open Message Box with Text. bool error expresses whether to use excl. marks ++ //
	static void OpenMessageBox(FString Text, bool bError = false);
	static FString AddExclamationMarkAroundChar(FString Text);

	// Checking LibraryHandle  //
	static bool CheckLibraryHandle(const void* LibraryHandle);

	// Copy FVector* to VAVec3* //
	static bool FVecToVAVec3(FVector& VecF, VAVec3& VecVA);

	// Copy FQuat* to QuatVA* //
	static bool FQuatToVAQuat(FQuat& QuatF, VAQuat& QuatVA);

	// Transforms Vec & Rotators in VA Coordinate System // 
	static FVector ToVACoordinateSystem(FVector VecF, float Scale);
	static FRotator ToVACoordinateSystem(FRotator RotF);

	// Compute reflected Position from wall
	static FVector ComputeReflectedPos(const AVAReflectionWall* Wall, FVector Pos);
	static FRotator ComputeReflectedRot(const AVAReflectionWall* Wall, FRotator Rot);

	// Log into VALog
	static void LogStuff(FString Text, bool Error = false);

};
