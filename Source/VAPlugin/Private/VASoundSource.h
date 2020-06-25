// Class for a Sound Source which can handle its reflections 

#pragma once

#include "CoreMinimal.h"
#include "VASignalBufferManager.h"

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

	// Setter
	void SetPosition(FVector PosN);
	void SetRotation(FRotator RotN);
	void SetVisibility(bool VisN);
	bool SetDirectivity(FVADirectivity* DirN);

	bool RemoveDirectivity();

	bool SetPlayAction(int ActionN) const;
	bool SetSoundTime(float TimeN) const;
	bool SetLoop(bool bLoopN);

	bool SetPower(float PowerN);

	bool PlaySound() const;
	bool StopSound() const;
	bool PauseSound() const;
	bool PlaySoundFromSecond(float TimeN) const;

	bool MuteSound(bool MutedN = true);

	// Functions for user to (pre-) load sound files at runtime
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
