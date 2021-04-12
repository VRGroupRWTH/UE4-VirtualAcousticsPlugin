// Class for a Sound Source which can handle its reflections 

#pragma once

#include "CoreMinimal.h"

#include "Templates/SharedPointer.h"

#include <string>

class AVASoundSourceRepresentation;
class FVADirectivity;
class UWorld;

class FVASoundSource
{
public:

	// Initialization
	FVASoundSource(UWorld* World, const FVector& PosN, const FRotator& RotN, float PowerN, const std::string& NameN = "", TSharedPtr<FVADirectivity> DirectivityID = nullptr);
	virtual ~FVASoundSource();

	// Setter
	void SetPosition(FVector NewPosition);
	void SetRotation(FRotator NewRotation);
	void SetVisibility(bool bVisible);

	bool SetDirectivity(TSharedPtr<FVADirectivity> NewDirectivity);
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
	TSharedPtr<FVADirectivity> GetDirectivity() const;
	FString GetDirectivityFilename() const;
	const std::string& GetSignalSourceID() const;

private:

	int SoundSourceID;
	std::string Name;

	FVector Position;
	FRotator Rotation;

	bool bShowCones;

	float Power;
	TSharedPtr<FVADirectivity> Directivity;
	std::string SignalSourceID;
	
	AVASoundSourceRepresentation* SoundSourceRepresentation;

};
