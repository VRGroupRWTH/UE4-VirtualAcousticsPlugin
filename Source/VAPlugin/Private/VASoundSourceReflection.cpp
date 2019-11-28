#include "VASoundSourceReflection.h"
#include "VAPlugin.h"

VASoundSourceReflection::VASoundSourceReflection(
	VASoundSource *parentSource_, 
	AVAReflectionWall* wall_, 
	std::string sBufferID,
	std::string name,
	FVector pos,
	FRotator rot, 
	float gainFactor) :
	parentSource(parentSource_), 
	wall(wall_)
{
	showCones = parentSource->getVisibility();

	pos = VAUtils::computeReflectedPos(wall, pos);
	rot = VAUtils::computeReflectedRot(wall, rot);

	if (FVAPluginModule::getIsMaster()) {
		soundSourceID = FVAPluginModule::createNewSoundSource(sBufferID, name, pos, rot, gainFactor);
		if (soundSourceID == -1)
		{
			VAUtils::logStuff("Error initializing soundSource in VASoundSourceReflection()");
			return;
		}
	}


	soundSourceRepresentation = parentSource->getParentComponent()->GetWorld()->SpawnActor<AVASoundSourceRepresentation>(AVASoundSourceRepresentation::StaticClass());
	
	
	soundSourceRepresentation->setPos(pos);
	soundSourceRepresentation->setRot(rot);
	soundSourceRepresentation->setVisibility(showCones);


}


void VASoundSourceReflection::setPos(FVector pos_)
{
	pos = VAUtils::computeReflectedPos(wall, pos_);

	if (FVAPluginModule::getIsMaster()) {
		FVAPluginModule::setSoundSourcePos(soundSourceID, pos);
	}

	if (showCones) {
		soundSourceRepresentation->setPos(pos);
	}
}


void VASoundSourceReflection::setRot(FRotator rot_)
{
	rot = VAUtils::computeReflectedRot(wall, rot_);

	if (FVAPluginModule::getIsMaster()) {
		FVAPluginModule::setSoundSourceRot(soundSourceID, rot);
	}

	if (showCones) {
		soundSourceRepresentation->setRot(rot);
	}
}

void VASoundSourceReflection::setVisibility(bool vis_)
{
	showCones = vis_;
	soundSourceRepresentation->setVisibility(vis_);
}

void VASoundSourceReflection::setDirectivity(VADirectivity* dir)
{
	FVAPluginModule::setSoundSourceDirectivity(soundSourceID, dir->getID());
}

FVector VASoundSourceReflection::getPos()
{
	return pos;
}

FRotator VASoundSourceReflection::getRot()
{
	return rot;
}

int VASoundSourceReflection::getSoundSourceID()
{
	return soundSourceID;
}
