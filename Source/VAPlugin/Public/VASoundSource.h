// Class for a Sound Source which can handle its reflections 

#pragma once

#include "VASoundSourceRepresentation.h"
#include "VASoundSourceReflection.h"
#include "VASourceComponent.h"
#include "VASignalBufferManager.h"

// #include <string>

class UVASourceComponent;

class FVASoundSource
{
public:

	// Initialization
	FVASoundSource(UVASourceComponent* ParentComponent);


	// Setter
	void SetPosition(FVector PosN);
	void SetRotation(FRotator RotN);
	void SetVisibility(bool VisN);
	void SetDirectivity(FVADirectivity* DirN);

	void SetPlayAction(int ActionN) const;
	void SetSoundTime(float TimeN) const;
	void SetLoop(bool bLoopN);

	void SetPower(float PowerN);

	void PlaySound() const;
	void StopSound() const;
	void PauseSound() const;
	void PlaySoundFromSecond(float TimeN) const;

	void MuteSound(bool MutedN = true);

	// Functions for user to (pre-) load new sound files at runtime
	bool SetNewSound(FString SoundFileN);
	bool LoadNewSound(FString SoundFileN);

	

	// Getter
	FVector GetPosition() const;
	FRotator GetRotation() const;

	bool GetVisibility() const;

	int GetSoundSourceID() const;
	int GetPlayState() const;

	float GetPower() const;

	FVADirectivity* GetDirectivity() const;

	UVASourceComponent* GetParentComponent() const;


private:

	static int Counter;

	int SoundSourceID;
	std::string SoundSourceName;

	FVector Position;
	FRotator Rotation;

	bool bPlaying;
	bool bHandleReflections;
	bool bShowCones;
	bool bLoop;

	float Power;

	FVADirectivity* Directivity;
	
	AVASoundSourceRepresentation* SoundSourceRepresentation;
	TArray<FVASoundSourceReflection*> Reflections;


	FVASignalBufferManager	BufferManager;
	FVASignalBuffer*		ActiveBuffer;

	UVASourceComponent* ParentComponent;


};
