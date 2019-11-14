#include "VASoundSource.h"


int VASoundSource::counter = 0;

VASoundSource::VASoundSource() {

}

/*
VASoundSource::VASoundSource(UVASourceComponent* parentComponent_) :
	parentComponent(parentComponent_)
{
	// TODO
}

*/

VASoundSource::VASoundSource(UVASourceComponent* parentComponent_) :
	parentComponent(parentComponent_)
{

	playing = false;
	action = EPlayAction::Stop;


	showCones = FVAPluginModule::isInDebugMode();

	handleReflections = parentComponent->getHandleReflections();
	loop = parentComponent->getLoop();
	gainFactor = parentComponent->getGainFactor();
	soundTimeOffset = parentComponent->getSoundTimeOffset();

	pos = parentComponent->getPosition();
	rot = parentComponent->getRotation();

	std::string fileName(TCHAR_TO_UTF8(*parentComponent->getFileName()));
	std::string name = "SoundSource_" + ++counter; 
	std::string nameTmp;

	if (FVAPluginModule::getIsMaster()) {
		sBufferID = FVAPluginModule::createNewBuffer(fileName, loop, soundTimeOffset);
		if (sBufferID == "")
		{
			VAUtils::logStuff("Error initializing Buffer in VASoundSource()");
			return;
		}
	
		soundSourceID = FVAPluginModule::createNewSoundSource(sBufferID, name, pos, rot, gainFactor);
		if (soundSourceID == -1)
		{
			VAUtils::logStuff("Error initializing soundSource in VASoundSource()");
			return;
		}
	}

	// Show graphical representation
	soundSourceRepresentation = parentComponent->GetWorld()->SpawnActor<AVASoundSourceRepresentation>(AVASoundSourceRepresentation::StaticClass());
	
	
	soundSourceRepresentation->setPos(getPos());
	soundSourceRepresentation->setRot(getRot());
	soundSourceRepresentation->setVisibility(showCones);

	if(handleReflections) {
		for (auto wall : FVAPluginModule::getReflectionWalls())
		{
			std::string wallName(TCHAR_TO_UTF8(*wall->GetName()));
			nameTmp = name + "_R_" + wallName;

			reflections.Add(new VASoundSourceReflection(this, wall, sBufferID, nameTmp, pos, rot, gainFactor));
		}
	}
}

void VASoundSource::setPos()
{
	setPos(parentComponent->getPosition());
}

void VASoundSource::setPos(FVector pos_)
{
	pos = pos_;

	// FString text = "Setting Position of Source (id: " + FString::FromInt(soundSourceID) + "): ";
	// text.Append(FString::FromInt(pos.X)).Append("/").Append(FString::FromInt(pos.Y)).Append("/").Append(FString::FromInt(pos.Z));
	// VAUtils::logStuff(text);
	
	if (showCones) {
		soundSourceRepresentation->setPos(pos);
	}
	
	if (FVAPluginModule::getIsMaster()) {
		FVAPluginModule::setSoundSourcePos(soundSourceID, pos);
	}

	if (handleReflections) {
		for (auto ref : reflections) {
			ref->setPos(pos);
		}
	}
}

void VASoundSource::setRot()
{
	setRot(parentComponent->getRotation());
}

void VASoundSource::setRot(FRotator rot_)
{
	rot = rot_;

	// FString text = "Setting Rotation of Source (id: " + FString::FromInt(soundSourceID) + "): ";
	// text.Append(FString::FromInt(rot.Roll)).Append("/").Append(FString::FromInt(rot.Pitch)).Append("/").Append(FString::FromInt(rot.Yaw));
	// VAUtils::logStuff(text);

	
	if (FVAPluginModule::getIsMaster()) {
		FVAPluginModule::setSoundSourceRot(soundSourceID, rot);
	}

	if (showCones) {
		soundSourceRepresentation->setRot(rot);
	}

	if (handleReflections)	{
		for (auto ref : reflections) {
			ref->setRot(rot);
		}
	}
}


void VASoundSource::setVisibility(bool vis_)
{
	visibility = vis_;
	
	soundSourceRepresentation->setVisibility(visibility);

	if (handleReflections) {
		for (auto ref : reflections) {
			ref->setVisibility(visibility);
		}
	}
}

bool VASoundSource::getVisibility()
{
	return visibility;
}

void VASoundSource::setDirectivity(VADirectivity * dir)
{
	directivity = dir;

	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	FVAPluginModule::setSoundSourceDirectivity(soundSourceID, dir->getID());

	if (handleReflections) {
		for (auto ref : reflections) {
			ref->setDirectivity(dir);
		}
	}
}

UVASourceComponent* VASoundSource::getParentComponent()
{
	return parentComponent;
}

void VASoundSource::setPlayAction(EPlayAction action_)
{
	if (action == action_ || !FVAPluginModule::getIsMaster()) {
		return;
	}

	action = action_;
	
	FVAPluginModule::setSoundBufferAction(sBufferID, action);
}

void VASoundSource::setSoundTime(float time)
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	FVAPluginModule::setSoundBufferTime(sBufferID, time);
}

void VASoundSource::playSound()
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	setPlayAction(EPlayAction::Play);
}

void VASoundSource::stopSound()
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	setPlayAction(EPlayAction::Stop);
}

void VASoundSource::pauseSound()
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	setPlayAction(EPlayAction::Pause);
}

void VASoundSource::playSoundFromSecond(float time)
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	setSoundTime(time);
	playSound();
}


// EPlayAction VASoundSource::getPlayAction()
// {
// 	return EPlayAction::Play;
// }
