// Class to manage all the Signal Buffer

#pragma once

#include <map>
#include <string>

class FVAAudiofileManager
{
public:
	FVAAudiofileManager() = default;
	
	// Returns the signal source ID referring to the given filename. Creates a new signal source if required.
	//   Returns "-1", if the signal source could not be created
	std::string GetAudiofileSignalSourceID(FString Filename);

	// Creates an audio file from the given filename (if not existing already) and stores it in the map
	//   Returns true on success.
	bool PreLoadAudiofile(FString Filename);

private:

	//Maps a filename to a given signal source ID
	std::map<FString, std::string> AudiofileIDMap;
};
