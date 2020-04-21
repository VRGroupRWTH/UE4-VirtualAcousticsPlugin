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

void FVAUtils::OpenMessageBox(const FString Text, const bool bError)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	if (bError)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(AddExclamationMarkInChar(Text)));
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Text));
	}

}


FString FVAUtils::AddExclamationMarkInChar(const FString Text)
{
	size_t Length = Text.Len();
	FString ReturnedString = FString("!!!!!");
	ReturnedString.Append(Text).Append("!!!!!");

	return ReturnedString;
}

bool FVAUtils::CheckLibraryHandle(const void* LibraryHandle)
{
	if (LibraryHandle)
	{
		return true;
	}
	return false;
}

bool FVAUtils::FVecToVAVec3(FVector& VecF, VAVec3& VecVA)
{
	if (&VecF == nullptr || &VecVA == nullptr)
	{	
		return false;
	}

	VecVA.Set(VecF.X, VecF.Y, VecF.Z);

	return true;
}


FVector FVAUtils::ToVACoordinateSystem(const FVector VecF, const float Scale)
{
	// ++ VA Server ++//   // ++ Unreal En ++//
	//*****|yP********//   //*****|z****x****//
	//*****|**********//   //*****|***/******//		  / forward
	//*****|********xR//   //*****|*/*******y//		 /
	//*****0----------//   //*****0----------//		/
	//****/***********//   //****************//
	//*zY/************//   //****************//
	const float ScaleInv = 1 / (Scale);
	return FVector(ScaleInv * VecF.Y, ScaleInv * VecF.Z, -ScaleInv * VecF.X);
}

FRotator FVAUtils::ToVACoordinateSystem(const FRotator RotF)
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

	const FQuat Quat = RotF.Quaternion();
	const FQuat Tmp = FQuat(-Quat.Y, -Quat.Z, Quat.X, Quat.W);
	return Tmp.Rotator();
}

bool FVAUtils::FQuatToVAQuat(FQuat& QuatF, VAQuat& QuatVA)
{
	if (&QuatF == nullptr || &QuatVA == nullptr)
	{	
		return false;
	}

	QuatVA.Set(QuatF.X, QuatF.Y, QuatF.Z, QuatF.W);

	return true;
}


FVector FVAUtils::ComputeReflectedPos(const AVAReflectionWall* Wall, const FVector Pos)
{
	const FVector Normal = Wall->GetNormalVector();
	const float D = Wall->GetHessianD();
	const float T = D - FVector::DotProduct(Normal, Pos);

	return (Pos + 2.0 * T * Normal);
}


FRotator FVAUtils::ComputeReflectedRot(const AVAReflectionWall* Wall, const FRotator Rot)
{
	// TODO Maybe make easier computation?
	
	const FVector NormalVec = Wall->GetNormalVector();

	const FVector Direction = Rot.Vector();

	const FVector Pos1 = Wall->GetSupportVector() + (1000 * NormalVec);
	const FVector Pos2 = Pos1 + (500 * Direction);

	const FVector Pos1R = ComputeReflectedPos(Wall, Pos1);
	const FVector Pos2R = ComputeReflectedPos(Wall, Pos2);

	const FVector Tmp = Pos2R - Pos1R;

	return Tmp.Rotation();
}


void FVAUtils::LogStuff(const FString Text, const bool Error)
{
	if (Error)
	{
		UE_LOG(VALog, Error, TEXT("%s"), *Text);
	}
	else
	{
		UE_LOG(VALog, Log, TEXT("%s"), *Text);
	}
}

int FVAUtils::EPlayActionToVAAction(const EPlayAction ActionE)
{
	// enum PlaybackAction
	// {
	// 	VA_PLAYBACK_ACTION_NONE = -1,	//!< No action
	// 	VA_PLAYBACK_ACTION_STOP = 0,	//!< Stop playback
	// 	VA_PLAYBACK_ACTION_PAUSE = 1,	//!< Pause playback
	// 	VA_PLAYBACK_ACTION_PLAY = 2,	//!< Start/resume playback	
	// };

	switch (ActionE)
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

	LogStuff("[EPlayActionToVAAction(EPlayAction ActionE)]: ActionE cannot be casted", true);

	return -1;
}

EPlayAction FVAUtils::VAActionToEPlayAction(const int ActionI)
{
	// enum PlaybackAction
	// {
	// 	VA_PLAYBACK_ACTION_NONE = -1,	//!< No action
	// 	VA_PLAYBACK_ACTION_STOP = 0,	//!< Stop playback
	// 	VA_PLAYBACK_ACTION_PAUSE = 1,	//!< Pause playback
	// 	VA_PLAYBACK_ACTION_PLAY = 2,	//!< Start/resume playback	
	// };

	switch (ActionI)
	{
	case -1:
		LogStuff("[VAUtils::VAActionToEPlayAction]: Detected no action as Play State, transferring to Stop");
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

	LogStuff("[VAUtils::VAActionToEPlayAction(int action)]: VAAction cannot be casted", true);
	return Stop;
}
