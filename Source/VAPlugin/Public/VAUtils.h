// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#include "VA.h"
#include "VANet.h"


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


	// ++ Open Message Box with Text. bool error expresses whether to use excl. marks ++ //
	static void openMessageBox(char* text, bool error = false);
	static void openMessageBoxV(char* text, bool error = false);
	static char* addExclamationMarkInChar(char* text);

	// ++ Checking LibraryHandle ++ //
	static bool checkLibraryHandle(void* libraryHandle);

	// ++ Copy FVector* to VAVec3* ++ //
	static bool fVecToVAVec3(FVector& vecF, VAVec3& vecVA);
	// ++ Copy FVector* to VAVec3* and while switching Coordinate System from Unreal To VAServer ++ //
	static bool fVecToVAVec3Rot(FVector& vecF, VAVec3& vecVA);

	static FVector toVACoordinateSystem(FVector vecF);
	static FRotator toVACoordinateSystem(FRotator rotF);

	// ++ Copy FQuat* to QuatVA* ++ //
	static bool fQuatToVAQuat(FQuat& quatF, VAQuat& quatVA);
	// ++ Copy FQuat* to QuatVA* and while switching Coordinate System from Unreal To VAServer ++ //
	// TODO: MIGHT BE INCORRECT!! ++ // 
	static bool fQuatToVAQuatRot(FQuat& quatF, VAQuat& quatVA);

	static bool rotateFRotator(FRotator& rot);
	static bool rotateFVec(FVector& vec);

	static bool scaleVAVec(VAVec3& vecVA, float scale);


	/*
	static bool getViewingPos(FVector* vec, FQuat* quat);
	static bool getViewingPos(VAVec3* vec, VAQuat* quat);
	static bool getViewingPosVA(VAVec3* vec, VAQuat* quat);
	*/

private:


};
