// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VAUtils.h"
#include "VADefines.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "VAPlugin.h"
//#include "VA.h"
//#include "VANet.h"
//#include "VistaBase/VistaTimeUtils.h"


void VAUtils::openMessageBox(FString text, bool error)
{
	//char* text;

	if (!FVAPluginModule::isMasterAndUsed()) {
		return;
	}
	
	if (error) {
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(addExclamationMarkInChar(text)));
		//text = addExclamationMarkInChar(text);
	}
	else {
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(text));
		//text = text_p;
	}
	//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ANSI_TO_TCHAR(text)), &FText::FromString(ANSI_TO_TCHAR(title)));
	//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ANSI_TO_TCHAR(text)));
}

void VAUtils::openMessageBoxV(FString text, bool error)
{
	if (M_VERBOSE)
		openMessageBox(text, error);
}

FString VAUtils::addExclamationMarkInChar(FString text)
{
	size_t len = text.Len();
	//char* ret = new char[len + 21];
	FString ret = FString();
	for (int i = 0; i < 10; i++)
	{
		ret[i] = '!';
		ret[i + 10 + len] = '!';
	}

	for (int i = 0; i < len; i++)
	{
		ret[10 + i] = text[i];
	}

	ret[len + 20] = '\0';

	/*strcpy(ret, text);
	ret[len] = a;
	ret[len + 1] = '\0';*/

	return ret;
}

bool VAUtils::checkLibraryHandle(void* LibraryHandle)
{
	if (LibraryHandle) {
		openMessageBoxV("Handle OK");
		return true;
	}
	else {
		openMessageBox("Handle not OK", true);
		return false;
	}
}

bool VAUtils::fVecToVAVec3(FVector& VecF, VAVec3& VecVA)
{
	if (&VecF == NULL || &VecVA == 0)
		return false;

	VecVA.Set(VecF.X, VecF.Y, VecF.Z);

	return true;
}

bool VAUtils::fVecToVAVec3Rot(FVector& vecF, VAVec3& vecVA)
{
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|y*********//   //*****|z****x****//
	//*****|**********//   //*****|***/******//
	//*****|*********x//   //*****|*/*******y//
	//*****0----------//   //*****0----------//
	//****/***********//   //****************//
	//**z/************//   //****************//

	if (&vecF == NULL || &vecVA == NULL)
		return false;

	//VecVA.Set(-VecF.Z, VecF.X, VecF.Y);
	vecVA.Set(vecF.Y, vecF.Z, -vecF.X);

	return true;
}

FVector VAUtils::toVACoordinateSystem(FVector vecF)
{
	return FVector(vecF.Y, vecF.Z, -vecF.X);
}

FRotator VAUtils::toVACoordinateSystem(FRotator rotF)
{
	return FRotator(rotF.Yaw, -rotF.Roll, rotF.Pitch);
}

bool VAUtils::fQuatToVAQuat(FQuat& QuatF, VAQuat& QuatVA)
{
	if (&QuatF == NULL || &QuatVA == NULL)
		return false;

	QuatVA.Set(QuatF.X, QuatF.Y, QuatF.Z, QuatF.W);

	return true;
}

bool VAUtils::fQuatToVAQuatRot(FQuat& QuatF, VAQuat& QuatVA)
{
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|y*********//   //*****|z****x****//
	//*****|**********//   //*****|***/******//
	//*****|*********x//   //*****|*/*******y//
	//*****0----------//   //*****0----------//
	//****/***********//   //****************//
	//**z/************//   //****************//

	if (&QuatF == NULL || &QuatVA == NULL)
		return false;

	QuatVA.Set(QuatF.Z, -QuatF.X, -QuatF.Y, QuatF.W);

	return true;
}

bool VAUtils::rotateFRotator(FRotator & rot)
{
	// X = Y // Y = Z // Z = -X
	// rot1.Pitch = rot.Yaw;
	// rot1.Yaw = -rot.Roll;
	// rot1.Roll = rot.Pitch;

	// FRotator rot1 = FRotator(rot.Yaw, -rot.Roll, rot.Pitch);

	rot = FRotator(rot.Yaw, -rot.Roll, rot.Pitch);
	return true;
}

bool VAUtils::rotateFVec(FVector & vec)
{
	// X = Y // Y = Z // Z = -X
	vec = FVector(vec.Y, vec.Z, -vec.X);
	return true;
}

bool VAUtils::scaleVAVec(VAVec3 & vecVA, float scale)
{
	double scaleD = (double)scale;
	vecVA.Set(vecVA.x / scaleD, vecVA.y / scaleD, vecVA.z / scaleD);
	return true;
}


/*
bool VAUtils::getViewingPos(FVector* vec, FQuat* quat)
{
	if (vec == NULL || quat == NULL)
		return false;

	FRotator* rot = new FRotator();

	//UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(*rot, *vec);
	*quat = rot->Quaternion();
	delete rot;

	return true;
}

bool VAUtils::getViewingPos(VAVec3* vec, VAQuat* quat)
{
	if (vec == NULL || quat == NULL)
		return false;

	FVector* vecF = new FVector();
	FQuat* quatF = new FQuat();
	FRotator* rot = new FRotator();

	if(FVAPluginModule::isViewModeHMD())
	{
		//UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(*rot, *vecF);
	}

	*quatF = rot->Quaternion();

	fQuatToVAQuat(quatF, quat);
	fVecToVAVec3(vecF, vec);

	delete vecF, quatF, rot;
	return true;
}

bool VAUtils::getViewingPosVA(VAVec3* vec, VAQuat* quat)
{
	if (vec == NULL || quat == NULL)
		return false;

	FVector* vecF = new FVector();
	FQuat* quatF = new FQuat();
	FRotator* rot = new FRotator();

	//UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(*rot, *vecF);
	*quatF = rot->Quaternion();

	fQuatToVAQuatRot(quatF, quat);
	fVecToVAVec3Rot(vecF, vec);

	delete vecF, quatF, rot;
	return true;
}
*/

