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

	if (!FVAPluginModule::getIsMaster()) {
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


FVector VAUtils::toVACoordinateSystem(FVector vecF, float scale)
{
	// https://www.google.com/imgres?imgurl=https%3A%2F%2Fi.stack.imgur.com%2FIn6Ee.png&imgrefurl=https%3A%2F%2Fstackoverflow.com%2Fquestions%2F31191752%2Fright-handed-euler-angles-xyz-to-left-handed-euler-angles-xyz&tbnid=JnhPiTTmhH_YaM&vet=12ahUKEwiX_vKzqt7nAhXVBFAKHaElBsEQMygCegUIARDTAQ..i&docid=XMAPpE84VLlJ5M&w=600&h=281&q=rotation%20transform%20left%20handed%20right%20handed&ved=2ahUKEwiX_vKzqt7nAhXVBFAKHaElBsEQMygCegUIARDTAQ
	// OLD 
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|y*********//   //*****|z****x****//
	//*****|**********//   //*****|***/******//
	//*****|*********x//   //*****|*/*******y//
	//*****0----------//   //*****0----------//
	//****/***********//   //****************//
	//**z/************//   //****************//
	// return FVector(vecF.Y, vecF.Z, -vecF.X);
	
	// NEW
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|yP********//   //*****|zY********//
	//*****|**********//   //*****|**********//
	//*****|********xR//   //*****|********xR//
	//*****0----------//   //*****0----------//
	//****/***********//   //****/***********//
	//*zY/************//   //*yP/************//
	// const float scale_ = 1 / (scale);
	// return FVector(scale_ * vecF.X, scale_ * vecF.Z, scale_ * vecF.Y);


	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|y****x****//   //*****|zY********//
	//*****|***/******//   //*****|**********//
	//*****|*/*******z//   //*****|********xR//
	//*****0----------//   //*****0----------//
	//****************//   //****/***********//
	//****************//   //*yP/************//
	const float scale_ = 1 / (scale);
	return FVector(-scale_ * vecF.Y, scale_ * vecF.Z, scale_ * vecF.X);
}

FRotator VAUtils::toVACoordinateSystem(FRotator rotF)
{
	// OLD 
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|y*********//   //*****|z****x****//
	//*****|**********//   //*****|***/******//
	//*****|*********x//   //*****|*/*******y//
	//*****0----------//   //*****0----------//
	//****/***********//   //****************//
	//**z/************//   //****************//
	// return FRotator(rotF.Yaw, -rotF.Roll, rotF.Pitch);
	
	// NEW
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|yY********//   //*****|zY********//
	//*****|**********//   //*****|**********//
	//*****|********xP//   //*****|********xR//
	//*****0----------//   //*****0----------//
	//****/***********//   //****/***********//
	//*zR/************//   //*yP/************//
	// return FRotator(rotF.Pitch, rotF.Yaw, rotF.Roll);

	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|yY****xP**//   //*****|zY********//
	//*****|***/******//   //*****|**********//
	//*****|*/******zR//   //*****|********xR//
	//*****0----------//   //*****0----------//
	//****************//   //****/***********//
	//****************//   //*yP/************//

	// logStuff(FString("toVACoordinateSystem: R: " + FString::SanitizeFloat(rotF.Roll) + " - P: " + FString::SanitizeFloat(rotF.Pitch) + " - Y: " + FString::SanitizeFloat(rotF.Yaw)));
	return FRotator(rotF.Pitch, -rotF.Yaw, -rotF.Roll);

}

bool VAUtils::fQuatToVAQuat(FQuat& QuatF, VAQuat& QuatVA)
{
	if (&QuatF == NULL || &QuatVA == NULL)
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
	if (error) {
		UE_LOG(VALog, Error, TEXT("%s"), *text);
	}
	else {
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
		return EPlayAction::Stop;
		break;
	case 1:
		return EPlayAction::Pause;
		break;
	case 2:
		return EPlayAction::Play;
		break;
	default:
		break;
	}

	logStuff("[VAUtils::VAActionToEPlayAction(int action)]: VAAction cannot be transverted", true);
	return EPlayAction::Stop;
}



