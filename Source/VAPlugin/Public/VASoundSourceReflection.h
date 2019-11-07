// Sound Source Reflection Class 

#pragma once

#include "VASoundSourceBaseClass.h"
#include "VAReflectionWall.h"
#include "VASoundSource.h"


class VASoundSource;

class VASoundSourceReflection : public VASoundSourceBaseClass
{
public:

	// Calculates the correct position by its own, needs the position of the original soundSource
	VASoundSourceReflection(VASoundSource *parentSource, AVAReflectionWall* wall, 
		std::string sBufferID, std::string name = "", FVector pos = FVector(),
		FRotator rot = FRotator(), float gainFactor = 1.0f);

	virtual void setPos();
	virtual void setPos(FVector pos_);

	virtual void setRot();
	virtual void setRot(FRotator rot_);

	virtual void setVisibility(bool vis_);

	virtual void setDirectivity(VADirectivity* dir);

private:
	// parent Sound Source (where the reflection is created from)
	VASoundSource *parentSource;

	// Wall this Reflection is mirrored on
	AVAReflectionWall* wall;


};