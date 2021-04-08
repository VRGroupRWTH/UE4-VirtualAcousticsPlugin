// Class for a Sound Source which can handle its reflections 

#pragma once

#include "CoreMinimal.h"

#include <string>

class AVASoundSourceRepresentation;
class UWorld;

class FVASoundSourceBase
{
public:

	// Initialization
	FVASoundSourceBase(UWorld* World, const FVector& PosN, const FRotator& RotN, float PowerN, const std::string& NameN = "", int DirectivityID = -1);
	virtual ~FVASoundSourceBase();

	// Setter
	void SetPosition(FVector NewPosition);
	void SetRotation(FRotator NewRotation);
	void SetVisibility(bool bVisible);

	bool SetDirectivity(int DirectivityID);
	bool RemoveDirectivity();

	bool SetSignalSource(const std::string& SignalSourceID);

	bool SetPower(float PowerN);

	bool MuteSound(bool MutedN = true);

	

	// Getter
	int GetSoundSourceID() const;
	const std::string& GetSoundSourceName() const;

	FVector GetPosition() const;
	FRotator GetRotation() const;

	bool GetVisibility() const;

	float GetPower() const;
	int GetDirectivityID() const;
	const std::string& GetSignalSourceID() const;

private:

	int SoundSourceID;
	std::string Name;

	FVector Position;
	FRotator Rotation;

	bool bShowCones;

	float Power;
	int DirectivityID;
	std::string SignalSourceID;
	
	AVASoundSourceRepresentation* SoundSourceRepresentation;

};
