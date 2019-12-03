// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VAUtils.h"
#include "VADefines.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "VAPlugin.h"
//#include "VistaBase/VistaTimeUtils.h"

// #pragma warning(disable:5038) //disable initilization order warning
#include "VA.h"
#include "VANet.h"
// #pragma warning(default:5038)



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



FString VAUtils::addExclamationMarkInChar(FString text)
{
	size_t len = text.Len();
	//char* ret = new char[len + 21];
	FString ret = FString("!!!!!");
	ret.Append(text).Append("!!!!!");
	
	return ret;
	
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
		return true;
	}
	else {
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

FVector VAUtils::computeReflectedPos(AVAReflectionWall* wall, FVector pos)
{
	FVector n = wall->getNormalVec();
	float d = wall->getD();
	float t = d - FVector::DotProduct(n, pos);

	return (pos + 2.0 * t * n);
}


FRotator VAUtils::computeReflectedRot(AVAReflectionWall* wall, FRotator rot)
{
	FVector pos1, pos2, pos1_, pos2_, n, tmp, dir;

	n = wall->getNormalVec();

	dir = rot.Vector();

	pos1 = wall->getSupportVec() + (1000 * n);
	pos2 = pos1 + (500 * dir);

	pos1_ = computeReflectedPos(wall, pos1);
	pos2_ = computeReflectedPos(wall, pos2);

	tmp = pos2_ - pos1_;

	// return FRotator(0, 0, 0);
	return tmp.Rotation();

}

bool VAUtils::scaleVAVec(VAVec3 & vecVA, float scale)
{
	double scaleD = (double)scale;
	vecVA.Set(vecVA.x / scaleD, vecVA.y / scaleD, vecVA.z / scaleD);
	return true;
}

void VAUtils::logStuff(FString text)
{
	UE_LOG(LogTemp, Error, TEXT("[MSchmeling] : %s"), *text);
}

int VAUtils::EPlayActionToVAAction(EPlayAction action)
{
	// enum PlaybackAction
	// {
	// 	VA_PLAYBACK_ACTION_NONE = -1,	//!< No action
	// 	VA_PLAYBACK_ACTION_STOP = 0,	//!< Stop playback
	// 	VA_PLAYBACK_ACTION_PAUSE = 1,	//!< Pause playback
	// 	VA_PLAYBACK_ACTION_PLAY = 2,	//!< Start/resume playback	
	// };

	switch (action)
	{
	case EPlayAction::Stop:
		return 0;
		break;
	case EPlayAction::Pause:
		return 1;
		break;
	case EPlayAction::Play:
		return 2;
		break;
	default:
		return -1;
	}
	return -1;
}



