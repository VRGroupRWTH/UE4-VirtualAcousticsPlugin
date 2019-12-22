#include "VASoundSource.h"

#include "VASourceComponent.h"
#include "VAPlugin.h"

int VASoundSource::counter = 0;

VASoundSource::VASoundSource() {

}

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
	std::string name = "SoundSource";
	std::string nameTmp;

	if (FVAPluginModule::getIsMaster()) {
		sBufferID = FVAPluginModule::createNewBuffer(parentComponent->getFileName(), loop, soundTimeOffset);
		if (sBufferID == "")
		{
			VAUtils::logStuff("[VASoundSource::VASoundSource(UVASourceComponent*)] Error initializing Buffer", true);
			return;
		}
	
		soundSourceID = FVAPluginModule::createNewSoundSource(sBufferID, name, pos, rot, gainFactor);
		if (soundSourceID == -1)
		{
			VAUtils::logStuff("[VASoundSource::VASoundSource(UVASourceComponent*)] Error initializing soundSource", true);
			return;
		}
	}

	// Show graphical representation
	soundSourceRepresentation = parentComponent->GetWorld()->SpawnActor<AVASoundSourceRepresentation>(AVASoundSourceRepresentation::StaticClass());
	
	
	soundSourceRepresentation->setPos(parentComponent->getPosition());
	soundSourceRepresentation->setRot(parentComponent->getRotation());
	soundSourceRepresentation->setVisibility(showCones);

	if(handleReflections) {
		TArray<AVAReflectionWall*> wallArray = FVAPluginModule::getReflectionWalls();
		for (auto wall : wallArray)
		{
			std::string wallName(TCHAR_TO_UTF8(*wall->GetName()));
			nameTmp = name + "_R_" + wallName;

			reflections.Add(new VASoundSourceReflection(this, wall, sBufferID, nameTmp, pos, rot, gainFactor));
		}
	}
}


void VASoundSource::setPos(FVector pos_)
{
	pos = pos_;
	
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


void VASoundSource::setRot(FRotator rot_)
{
	rot = rot_;
	
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
	showCones = vis_;
	
	soundSourceRepresentation->setVisibility(showCones);

	if (handleReflections && reflections.Num() != 0) {
		for (auto ref : reflections) {
			ref->setVisibility(showCones);
		}
	}
}

bool VASoundSource::getVisibility()
{
	return showCones;
}

void VASoundSource::setDirectivity(VADirectivity* dir)
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

void VASoundSource::setPlayAction(int action_)
{
	if (!FVAPluginModule::getIsMaster() || getPlayState() == action_) {
		return;
	}

	action = action_;
	
	FVAPluginModule::setSoundBufferAction(sBufferID, static_cast<EPlayAction>(action));
}

void VASoundSource::setSoundTime(float time)
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	FVAPluginModule::setSoundBufferTime(sBufferID, time);
}

void VASoundSource::setLoop(bool loop)
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	FVAPluginModule::setSoundBufferLoop(sBufferID, loop);
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

int VASoundSource::getPlayState()
{
	return FVAPluginModule::getSoundBufferAction(sBufferID);
}

void VASoundSource::setNewSound(FString soundFile_)
{
	// stop 
	stopSound();

	// create new buffer
	std::string newBufferID = FVAPluginModule::createNewBuffer(soundFile_);

	// link buffer
	FVAPluginModule::setNewBufferForSoundSource(soundSourceID, newBufferID);
	for (auto iter : reflections) {
		FVAPluginModule::setNewBufferForSoundSource(iter->getSoundSourceID(), newBufferID);
	}

	// TODO: delete buffer?
}


FVector VASoundSource::getPos()
{
	return pos;
}

FRotator VASoundSource::getRot()
{
	return rot;
}

int VASoundSource::getSoundSourceID()
{
	return soundSourceID;
}
