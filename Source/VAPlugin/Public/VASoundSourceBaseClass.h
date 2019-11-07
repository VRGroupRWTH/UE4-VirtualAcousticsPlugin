// Base Class for Sound Source and Sound Source Reflection

#pragma once

#include "VASoundSourceRepresentation.h"

class VASoundSourceBaseClass
{
public:
	VASoundSourceBaseClass();

	virtual void setPos() = 0;
	virtual void setPos(FVector pos_) = 0;
	virtual FVector getPos();

	virtual void setRot() = 0;
	virtual void setRot(FRotator rot_) = 0;
	virtual FRotator getRot();

	virtual void setVisibility(bool vis_) = 0;

	virtual int getSoundSourceID();

	virtual void setDirectivity(VADirectivity* dir) = 0;


protected:
	FVector pos;

	FRotator rot;


	int soundSourceID;

	AVASoundSourceRepresentation* soundSourceRepresentation;

	std::string soundSourceName;

	bool showCones;


};