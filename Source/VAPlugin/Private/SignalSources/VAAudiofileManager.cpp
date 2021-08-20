#include "VAAudiofileManager.h"

#include "VAUtils.h"
#include "VAPlugin.h"


std::string FVAAudiofileManager::GetAudiofileSignalSourceID(FString Filename)
{
	auto it = AudiofileIDMap.find(Filename);

	if (it != AudiofileIDMap.end())
	{
		FVAUtils::LogStuff("[FVAAudiofileManager::GetAudiofileSignalSourceID()]: Signal source based on audiofile " + Filename + " was found!", false);
		return it->second;
	}
	FVAUtils::LogStuff("[FVAAudiofileManager::GetAudiofileSignalSourceID()]: Signal source based on audiofile " +
		Filename + " cannot be found! Creating one now...", false);

	//also try to send the file over to the VAServer
	FVAPlugin::VAServerLauncher.SendFileToVAServer(Filename);
	
	std::string ID = FVAPlugin::CreateSignalSourceBuffer(Filename);
	if (ID == "-1")
	{
		FVAUtils::LogStuff("[FVAAudiofileManager::GetAudiofileSignalSourceID()]: Could not create signal source from audiofile " + Filename, true);
	}
	return ID;
}

bool FVAAudiofileManager::PreLoadAudiofile(FString Filename)
{
	if (AudiofileIDMap.count(Filename))
	{
		FVAUtils::LogStuff("[FVAAudiofileManager::PreLoadAudiofile()]: Trying to pre-load file that has already been loaded: " + Filename, false);
		return true;
	}

	//also try to send the file over to the VAServer
	FVAPlugin::VAServerLauncher.SendFileToVAServer(Filename);
	
	std::string ID = FVAPlugin::CreateSignalSourceBuffer(Filename);
	if (ID == "-1")
	{
		FVAUtils::LogStuff("[FVAAudiofileManager::PreLoadAudiofile()]: Could not create signal source from audiofile " + Filename, true);
		return false;
	}

	FVAUtils::LogStuff("[FVAAudiofileManager::PreLoadAudiofile()]: Created signal source from audiofile " + Filename, false);
	AudiofileIDMap[Filename] = ID;
	return true;
}
