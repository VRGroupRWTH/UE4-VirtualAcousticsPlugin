#include "VAImageSourceModel.h"

#include "VAPlugin.h"
#include "VAUtils.h"

#include "VAReflectionWall.h"

#include "Engine/World.h"

#include "Kismet/KismetMathLibrary.h"


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVAImageSourceModel::FVAImageSourceModel(UWorld* World, FVASoundSourceBase* ParentSource, const TArray<AVAReflectionWall*>& ReflectionWalls)
	: ParentSource(ParentSource)
{
	if (!ParentSource)
	{
		FVAUtils::LogStuff("[FVAImageSourceModel::FVAImageSourceModel()]: Trying to initialize IS model based on invalid sound source (nullptr)", true);
		return;
	}
	for (auto Wall : ReflectionWalls)
	{
		std::string WallName(TCHAR_TO_UTF8(*Wall->GetName()));
		const std::string ISName = ParentSource->GetSoundSourceName() + "_R_" + WallName;

		const float R = Wall->GetReflectionValueR();
		const float PowerR = ParentSource->GetPower() * R * R;
		ImageSources.Add( MakeShared<FVAImageSource>(Wall, World, ParentSource->GetPosition(), ParentSource->GetRotation(), PowerR, ISName, ParentSource->GetDirectivity()) );
	}
}



// ****************************************************************** // 
// ******* Setter *************************************************** //
// ****************************************************************** //


void FVAImageSourceModel::UpdateISPositions()
{
	for (auto IS : ImageSources)
	{
		IS->SetPosition( ComputeReflectedPos(IS->GetWall(), ParentSource->GetPosition()) );
	}
}

void FVAImageSourceModel::UpdateISRotations()
{
	for (auto IS : ImageSources)
	{
		IS->SetRotation( ComputeReflectedRot(IS->GetWall(), ParentSource->GetRotation()) );
	}
}

void FVAImageSourceModel::SetISVisibility(const bool bVisible)
{
	for (auto IS : ImageSources)
	{
		IS->SetVisibility(bVisible);
	}
}


bool FVAImageSourceModel::MuteIS(const bool bMute)
{
	bool bFullSuccess = true;
	for (auto IS : ImageSources)
	{
		if (!IS->MuteSound(bMute))
		{
			bFullSuccess = false;
		}
	}

	return bFullSuccess;
}


bool FVAImageSourceModel::UpdateISSignalSource()
{
	bool bFullSuccess = true;
	const std::string& SignalSourceID = ParentSource->GetSignalSourceID();
	for (auto IS : ImageSources)
	{
		if (!FVAPlugin::SetSoundSourceSignalSource(IS->GetSoundSourceID(), SignalSourceID))
		{
			bFullSuccess = false;
		}
	}

	return bFullSuccess;
}


bool FVAImageSourceModel::UpdateISDirectivities()
{
	bool bFullSuccess = true;
	for (auto IS : ImageSources)
	{
		if (!IS->SetDirectivity(ParentSource->GetDirectivity()))
		{
			bFullSuccess = false;
		}
	}

	return bFullSuccess;
}

bool FVAImageSourceModel::RemoveISDirectivities()
{
	bool bFullSuccess = true;
	for (auto IS : ImageSources)
	{
		if (!IS->RemoveDirectivity())
		{
			bFullSuccess = false;
		}
	}

	return bFullSuccess;
}

bool FVAImageSourceModel::UpdateISPower()
{
	bool bFullSuccess = true;
	for (auto IS : ImageSources)
	{
		const float R = IS->GetWall()->GetReflectionValueR();
		const float Power = ParentSource->GetPower() * R * R;
		if (!IS->SetPower(Power))
		{
			bFullSuccess = false;
		}
	}

	return bFullSuccess;
}


// ****************************************************************** // 
// ******* Static: Mirrored Pos/Rot ********************************* //
// ****************************************************************** //

FVector FVAImageSourceModel::ComputeReflectedPos(const AVAReflectionWall* Wall, const FVector& Pos)
{
	const FVector Normal = Wall->GetNormalVector();
	const float D = Wall->GetHessianD();
	const float T = D - FVector::DotProduct(Normal, Pos);

	return (Pos + 2.0 * T * Normal);
}

FRotator FVAImageSourceModel::ComputeReflectedRot(const AVAReflectionWall* Wall, const FRotator& Rot)
{
	const FVector WallNormalVec = Wall->GetNormalVector();
	const FVector StartPos = Wall->GetSupportVector() + (1000 * WallNormalVec);

	const FVector ForwardVector = UKismetMathLibrary::GetForwardVector(Rot);
	const FVector UpVector = UKismetMathLibrary::GetUpVector(Rot);

	const FVector PosForward = StartPos + (500 * ForwardVector);
	const FVector PosUp = StartPos + (500 * UpVector);

	const FVector StartPosR = ComputeReflectedPos(Wall, StartPos);
	const FVector PosForwardR = ComputeReflectedPos(Wall, PosForward);
	const FVector PosUpR = ComputeReflectedPos(Wall, PosUp);

	const FVector DirForwardR = PosForwardR - StartPosR;
	const FVector DirUpR = PosUpR - StartPosR;

	return UKismetMathLibrary::MakeRotFromXZ(DirForwardR, DirUpR);
}

// ****************************************************************** // 
// ******* Image Source Class *************************************** //
// ****************************************************************** //

FVAImageSourceModel::FVAImageSource::FVAImageSource(AVAReflectionWall* Wall, UWorld* World, const FVector& Position, const FRotator& Rotation, float Power, const std::string& Name, FVADirectivity*  Directivity /* = nullptr */)
	: FVASoundSourceBase(World, Position, Rotation, Power, Name, Directivity)
	, Wall(Wall)
{
	if (Wall == nullptr)
	{
		FVAUtils::OpenMessageBox("[FVAImageSourceModel::FVAImageSource::FVAImageSource()]: Cannot create image source without a valid reflection wall", true);
	}
}

AVAReflectionWall* FVAImageSourceModel::FVAImageSource::GetWall()
{
	return Wall;
}
