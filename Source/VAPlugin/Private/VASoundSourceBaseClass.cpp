#include "VASoundSourceBaseClass.h"


VASoundSourceBaseClass::VASoundSourceBaseClass()
{
}

FVector VASoundSourceBaseClass::getPos()
{
	return pos;
}

FRotator VASoundSourceBaseClass::getRot()
{
	return rot;
}

int VASoundSourceBaseClass::getSoundSourceID()
{
	return soundSourceID;
}





