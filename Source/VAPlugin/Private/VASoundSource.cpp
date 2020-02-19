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
	EPlayAction action = EPlayAction::Stop;

	showCones = FVAPluginModule::isInDebugMode();

	handleReflections = parentComponent->getHandleReflections();
	bool loop = parentComponent->getLoop();
	gainFactor = parentComponent->getGainFactor();
	soundTimeOffset = parentComponent->getSoundTimeOffset();

	pos = parentComponent->getPosition();
	rot = parentComponent->getRotation();

	std::string fileName(TCHAR_TO_UTF8(*parentComponent->getSoundFile()));
	std::string name = "SoundSource";
	std::string nameTmp;

	if (FVAPluginModule::getIsMaster()) {

		activeBuffer = bufferManager.getBufferByFileName(parentComponent->getSoundFile());

		if (activeBuffer == nullptr)
		{
			VAUtils::logStuff("[VASoundSource::VASoundSource(UVASourceComponent*)] Error initializing Buffer", true);
			return;
		}

		// Only Change if necessary
		if (loop) {
			activeBuffer->setLoop(loop);
		}

		// Only Change if necessary
		if (soundTimeOffset > 0.0f) {
			activeBuffer->setSoundTimeOffset(soundTimeOffset);
		}

		soundSourceID = FVAPluginModule::createNewSoundSource(activeBuffer->getID(), name, pos, rot, gainFactor);
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

			std::string ActiveBufferName;
			if(FVAPluginModule::getIsMaster())
			{
				ActiveBufferName = activeBuffer->getID();
			}
			else
			{
				ActiveBufferName = "BufferNameNotNeededOnSlave";
			}

			reflections.Add(new VASoundSourceReflection(this, wall, ActiveBufferName, nameTmp, pos, rot, gainFactor));
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

VADirectivity* VASoundSource::getDirectivity()
{
	return directivity;
}


UVASourceComponent* VASoundSource::getParentComponent()
{
	return parentComponent;
}

void VASoundSource::setPlayAction(int action)
{
	if (!FVAPluginModule::getIsMaster() || getPlayState() == action) {
		return;
	}

	activeBuffer->setSoundBufferAction(action);
	
}

void VASoundSource::setSoundTime(float time)
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}

	activeBuffer->setSoundTimeOffset(time);
}

void VASoundSource::setLoop(bool loop)
{
	if (!FVAPluginModule::getIsMaster()) {
		return;
	}
	activeBuffer->setLoop(loop);
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
	return activeBuffer->getSoundBufferAction();
}

void VASoundSource::muteSound(bool muted_)
{
	FVAPluginModule::setSoundSourceMuted(soundSourceID, muted_);

	for (auto iter : reflections) {
		FVAPluginModule::setSoundSourceMuted(iter->getSoundSourceID(), muted_);
	}
}


bool VASoundSource::setNewSound(FString soundFile_)
{
	// create new buffer
	VASignalBuffer* tmp_buffer = bufferManager.getBufferByFileName(soundFile_);

	// Check if is valid
	if (tmp_buffer == nullptr) {
		VAUtils::logStuff("[VASoundSource::setNewSound()] Buffer from file " + soundFile_ + " was loaded incorrectly!");
		return false;
	}
	
	// stop 
	stopSound();

	// If the same Buffer is already loaded only stop sound (prev)
	if (tmp_buffer == activeBuffer) {
		return true;
	}

	std::string newBufferID = tmp_buffer->getID();

	// Link to source buffer
	FVAPluginModule::setNewBufferForSoundSource(soundSourceID, newBufferID);
	for (auto iter : reflections) {
		FVAPluginModule::setNewBufferForSoundSource(iter->getSoundSourceID(), newBufferID);
	}

	activeBuffer = tmp_buffer;

	return true;
}

bool VASoundSource::loadNewSound(FString soundFile_)
{
	VASignalBuffer* tmp_buffer = bufferManager.getBufferByFileName(soundFile_);
	if (tmp_buffer != nullptr) {
		return true;
	}
	return false;
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
