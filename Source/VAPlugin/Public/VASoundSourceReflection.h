// Sound Source Reflection Class 

#pragma once

#include "VASoundSourceRepresentation.h"
#include "VASoundSourceBaseClass.h"
#include "VAReflectionWall.h"
#include "VASoundSource.h"


class VASoundSource;

class VASoundSourceReflection // : public VASoundSourceBaseClass
{
public:

	// Calculates the correct position by its own, needs the position of the original soundSource
	VASoundSourceReflection(VASoundSource *parentSource, AVAReflectionWall* wall, 
		std::string sBufferID, std::string name = "", FVector pos = FVector(),
		FRotator rot = FRotator(), float gainFactor = 1.0f);

	
	void setPos(FVector pos_);
	FVector getPos();

	void setRot(FRotator rot_);
	FRotator getRot();
	
	void setVisibility(bool vis_);

	void setDirectivity(VADirectivity* dir);
	
	int getSoundSourceID();

private:
	int soundSourceID;
	std::string soundSourceName;

	FVector pos;
	FRotator rot;

	// parent Sound Source (where the reflection is created from)
	VASoundSource *parentSource;

	// Wall this Reflection is mirrored on
	AVAReflectionWall* wall;

	// Sound Source Representation
	AVASoundSourceRepresentation* soundSourceRepresentation;


	bool showCones;



};