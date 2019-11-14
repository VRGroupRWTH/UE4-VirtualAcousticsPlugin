// Class for a Sound Source which can handle its reflections 

#pragma once

#include "VASoundSourceBaseClass.h"
#include "VASoundSourceReflection.h"
#include "VASourceComponent.h"

// #include <string>

class UVASourceComponent;


class VASoundSource : public VASoundSourceBaseClass
{
public:
	VASoundSource();

	VASoundSource(UVASourceComponent* parentComponent_);
	
	// VASoundSource(UVASourceComponent* parentComponent_, FVector pos = FVector(), 
	// 	FRotator rot = FRotator(), float gainFactor = 1.0f, 
	// 	bool loop = false, float soundOffset = 0.0f, 
	// 	bool handleReflections = true);

	virtual void setPos();
	virtual void setPos(FVector pos_);

	virtual void setRot();
	virtual void setRot(FRotator rot_);


	virtual void setVisibility(bool vis_);
	virtual bool getVisibility();

	virtual void setDirectivity(VADirectivity* dir);
	
	UVASourceComponent* getParentComponent();


	void setPlayAction(EPlayAction action_);
	void setSoundTime(float time);

	void playSound();
	void stopSound();
	void pauseSound();
	void playSoundFromSecond(float time);
	// EPlayAction getPlayAction();



private: 
	TArray<VASoundSourceReflection*> reflections;

	UVASourceComponent* parentComponent;

	VADirectivity* directivity;

	std::string sBufferID;

	bool playing;

	enum EPlayAction action;

	static int counter;
	
	bool handleReflections;
	bool loop;
	float gainFactor;
	float soundTimeOffset;

	bool visibility;

};