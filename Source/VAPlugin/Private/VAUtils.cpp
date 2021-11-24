// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VAUtils.h"

#include "VAPlugin.h"

#include "VA.h"
#include "Utility/VirtualRealityUtilities.h"

DEFINE_LOG_CATEGORY(VALog);

void FVAUtils::OpenMessageBox(const FString Text, const bool bError)
{
	if (!UVirtualRealityUtilities::IsMaster())
	{
		return;
	}

	FText Title = FText::FromString(FString(bError?"ERROR":"Message"));
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Text), &Title);
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
