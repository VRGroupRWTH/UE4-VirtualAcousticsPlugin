// Class for a Sound Source which can handle its reflections 

#pragma once

#include "VASoundSourceRepresentation.h"
#include "VASoundSourceBaseClass.h"
#include "VASoundSourceReflection.h"
#include "VASourceComponent.h"
#include "VASignalBufferManager.h"

// #include <string>

class UVASourceComponent;

class VASoundSource
{
public:
	VASoundSource();

	VASoundSource(UVASourceComponent* parentComponent_);

	// VASoundSource(UVASourceComponent* parentComponent_, FVector pos = FVector(), 
	// 	FRotator rot = FRotator(), float gainFactor = 1.0f, 
	// 	bool loop = false, float soundOffset = 0.0f, 
	// 	bool handleReflections = true);

	void setPos(FVector pos_);
	FVector getPos();

	void setRot(FRotator rot_);
	FRotator getRot();

	void setVisibility(bool vis_);
	bool getVisibility();

	void setDirectivity(VADirectivity* dir);
	VADirectivity* getDirectivity();

	UVASourceComponent* getParentComponent();

	int getSoundSourceID();

	void setPlayAction(int action_);
	void setSoundTime(float time);
	void setLoop(bool loop);

	void setPower(float power);

	void playSound();
	void stopSound();
	void pauseSound();
	void playSoundFromSecond(float time);

	void muteSound(bool muted_ = true);

	int getPlayState();


	// new functions for runtime usage
	bool setNewSound(FString soundFile_);

	bool loadNewSound(FString soundFile_);


private:

	FVector pos;
	FRotator rot;


	int soundSourceID;

	VASignalBufferManager bufferManager;
	VASignalBuffer* activeBuffer;

	AVASoundSourceRepresentation* soundSourceRepresentation;

	std::string soundSourceName;

	bool showCones;
	TArray<VASoundSourceReflection*> reflections;

	UVASourceComponent* parentComponent;

	VADirectivity* directivity;

	bool playing;


	static int counter;

	bool handleReflections;
	float power;
	float soundTimeOffset;

	bool visibility;
};
