// Class for a Sound Source which can handle its reflections 

#pragma once

#include "CoreMinimal.h"

#include <string>

class AVASoundSourceRepresentation;
class FVADirectivity;
class UWorld;

class FVASoundSourceBase
{
public:

	// Initialization
	FVASoundSourceBase(UWorld* World, const FVector& PosN, const FRotator& RotN, float PowerN, const std::string& NameN = "", FVADirectivity* DirectivityID = nullptr);
	virtual ~FVASoundSourceBase();

	// Setter
	void SetPosition(FVector NewPosition);
	void SetRotation(FRotator NewRotation);
	void SetVisibility(bool bVisible);

	bool SetDirectivity(FVADirectivity* NewDirectivity);
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
	FVADirectivity* GetDirectivity() const;
	FString GetDirectivityFilename() const;
	const std::string& GetSignalSourceID() const;

private:

	int SoundSourceID;
	std::string Name;

	FVector Position;
	FRotator Rotation;

	bool bShowCones;

	float Power;
	FVADirectivity* Directivity;
	std::string SignalSourceID;
	
	AVASoundSourceRepresentation* SoundSourceRepresentation;

};
