// Class to represent a VA Sound Source Signal Buffer for the VA Server

#pragma once


class VASignalBuffer
{
public:
	VASignalBuffer(FString fileName_);

	void setLoop(bool);
	bool getLoop();

	void setSoundTimeOffset(float soundTimeOffset);

	void setSoundBufferAction(int action);
	int getSoundBufferAction();

	std::string getID();
	bool isValid();

	FString getFileName();

protected:

	std::string bufferID;
	FString fileName;
	bool valid;
	bool loop;
};
