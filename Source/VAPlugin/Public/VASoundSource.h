// Class for a Sound Source which can handle its reflections 

#pragma once

#include "VASoundSourceRepresentation.h"
#include "VASoundSourceBaseClass.h"
#include "VASoundSourceReflection.h"
#include "VASourceComponent.h"

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
	
	UVASourceComponent* getParentComponent();

	int getSoundSourceID();

	void setPlayAction(int action_);
	void setSoundTime(float time);
	void setLoop(bool loop);

	void playSound();
	void stopSound();
	void pauseSound();
	void playSoundFromSecond(float time);

	void muteSound(bool muted_ = true);

	int getPlayState();


	// new functions for runtime usage
	void setNewSound(FString soundFile_);


private: 

	FVector pos;

	FRotator rot;


	int soundSourceID;

	AVASoundSourceRepresentation* soundSourceRepresentation;

	std::string soundSourceName;

	bool showCones;
	TArray<VASoundSourceReflection*> reflections;

	UVASourceComponent* parentComponent;

	VADirectivity* directivity;

	std::string sBufferID;

	bool playing;

	int action;

	static int counter;
	
	bool handleReflections;
	bool loop;
	float gainFactor;
	float soundTimeOffset;

	bool visibility;

};