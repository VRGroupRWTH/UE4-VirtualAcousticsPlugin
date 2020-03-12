// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VAUtils.h"

#include "VAReflectionWall.h"
#include "VASourceComponent.h"

#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "VAPlugin.h"
//#include "VistaBase/VistaTimeUtils.h"

// #pragma warning(disable:5038) //disable initilization order warning
#include "VA.h"
#include "VANet.h"
// #pragma warning(default:5038)

DEFINE_LOG_CATEGORY(VALog);

void VAUtils::openMessageBox(FString text, bool error)
{
	//char* text;

	if (!FVAPluginModule::getIsMaster())
	{
		return;
	}

	if (error)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(addExclamationMarkInChar(text)));
		//text = addExclamationMarkInChar(text);
	}
	else
	{
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
	if (LibraryHandle)
	{
		return true;
	}
	return false;
}

bool VAUtils::fVecToVAVec3(FVector& VecF, VAVec3& VecVA)
{
	if (&VecF == nullptr || &VecVA == nullptr)
		return false;

	VecVA.Set(VecF.X, VecF.Y, VecF.Z);

	return true;
}


FVector VAUtils::toVACoordinateSystem(FVector vecF, float scale)
{
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|yP********//   //*****|z****x****//
	//*****|**********//   //*****|***/******//		  / forward
	//*****|********xR//   //*****|*/*******y//		 /
	//*****0----------//   //*****0----------//		/
	//****/***********//   //****************//
	//*zY/************//   //****************//
	const float scale_ = 1 / (scale);
	return FVector(scale_ * vecF.Y, scale_ * vecF.Z, -scale_ * vecF.X);
}

FRotator VAUtils::toVACoordinateSystem(FRotator rotF)
{
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|yP********//   //*****|zY**xR****//
	//*****|**/******//   //*****|**/*******//		  / forward
	//*****|*/******xR//   //*****|*/******yP//		 /
	//*****0----------//   //*****0----------//		/
	//****/***********//   //****************//
	//*zY/************//   //****************//
	//
	//	https://gamedev.stackexchange.com/questions/157946/converting-a-quaternion-in-a-right-to-left-handed-coordinate-system
	//
	//			  Unreal			VAServer
	//	forward		x					-z
	//	up			z					y
	//	right		y					x
	//
	//
	//	All *(-1) due to changing direction
	//Quaternion(
	// 	-input.y,			(where VA x?)
	// 	-input.z,			(where VA y?)
	// 	input.x,			(where VA z?)
	// 	input.w		
	// )

	FQuat Quat = rotF.Quaternion();
	FQuat Tmp = FQuat(-Quat.Y, -Quat.Z, Quat.X, Quat.W);
	return Tmp.Rotator();
}

bool VAUtils::fQuatToVAQuat(FQuat& QuatF, VAQuat& QuatVA)
{
	if (&QuatF == nullptr || &QuatVA == nullptr)
		return false;

	QuatVA.Set(QuatF.X, QuatF.Y, QuatF.Z, QuatF.W);

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


void VAUtils::logStuff(FString text, bool error)
{
	if (error)
	{
		UE_LOG(VALog, Error, TEXT("%s"), *text);
	}
	else
	{
		UE_LOG(VALog, Log, TEXT("%s"), *text);
	}
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
	case Stop:
		return 0;
		break;
	case Pause:
		return 1;
		break;
	case Play:
		return 2;
		break;
	default:
		break;
	}
	return -1;
}

EPlayAction VAUtils::VAActionToEPlayAction(int action)
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
	case -1:
		logStuff("[VAUtils::VAActionToEPlayAction]: Detected no action as Play State, transfering to Stop");
	case 0:
		return Stop;
		break;
	case 1:
		return Pause;
		break;
	case 2:
		return Play;
		break;
	default:
		break;
	}

	logStuff("[VAUtils::VAActionToEPlayAction(int action)]: VAAction cannot be transverted", true);
	return Stop;
}
