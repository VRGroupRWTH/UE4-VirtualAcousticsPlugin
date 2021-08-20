#pragma once

#include "Sockets.h"

class FVAServerLauncher
{
public:
	// Remote Start VAServer
	bool RemoteStartVAServer(const FString& Host, int Port,
	                                const FString& VersionName);

	bool StartVAServerLauncher();

	bool SendFileToVAServer(const FString& RelativeFilename);


	//this closes the connection to the server launcher script, which will quit the VAServer instance
	void ReleaseVAServerLauncherConnection();

	bool IsVAServerLauncherConnected();
	

private:

	TArray<uint8> ConvertString(const FString& String);

	//Socket connection to the VAServer Launcher, has to be held open until the program ends
	FSocket* VAServerLauncherSocket=nullptr;
};
