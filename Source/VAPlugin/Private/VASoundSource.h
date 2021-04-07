// Class for a Sound Source which can handle its reflections 

#pragma once

#include "CoreMinimal.h"

class UVASourceComponent;
class FVASoundSourceReflection;
class AVASoundSourceRepresentation;
class FVADirectivity;
class AVAReflectionWall;

class FVASoundSource
{
public:

	// Initialization
	FVASoundSource(UVASourceComponent* ParentComponent, TArray<AVAReflectionWall*> WallArray);
	~FVASoundSource();

	// Setter
	void SetPosition(FVector PosN);
	void SetRotation(FRotator RotN);
	void SetVisibility(bool VisN);
	bool SetDirectivity(FVADirectivity* DirN);

	bool SetSignalSourceToImageSources(const std::string& SignalSourceID);

	bool RemoveDirectivity();

	bool SetPower(float PowerN);

	bool MuteSound(bool MutedN = true);

	

	// Getter
	FVector GetPosition() const;
	FRotator GetRotation() const;

	bool GetVisibility() const;

	int GetSoundSourceID() const;

	float GetPower() const;

	FVADirectivity* GetDirectivity() const;

	UVASourceComponent* GetParentComponent() const;


private:

	int SoundSourceID;

	FVector Position;
	FRotator Rotation;

	bool bHandleReflections;
	bool bShowCones;

	float Power;

	FVADirectivity* Directivity;
	
	AVASoundSourceRepresentation* SoundSourceRepresentation;
	TArray<FVASoundSourceReflection*> ImageSources;


	UVASourceComponent* ParentComponent;

};
