#include "VAServerLauncher.h"

#include <string>

#include "Misc/FileHelper.h"
#include "SocketSubsystem.h"
#include "GeneralProjectSettings.h"

#include "Utility/VirtualRealityUtilities.h"

#include "VAUtils.h"
#include "VASettings.h"
#include "VAPlugin.h"


bool FVAServerLauncher::RemoteStartVAServer(const FString& Host, const int Port, const FString& VersionName)
{
	if (!UVirtualRealityUtilities::IsMaster())
	{
		return false;
	}

	if (VAServerLauncherSocket != nullptr)
	{
		return true;
	}

	FVAUtils::LogStuff("[FVAServerLauncher::RemoteStartVAServer()]: Try to remotely start the VAServer at address " + 
		Host + ":" + FString::FromInt(Port) + " for version: " + VersionName, false);


	//Connect
	const FString SocketName(TEXT("VAServerStarterConnection"));
	VAServerLauncherSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(
		NAME_Stream, SocketName, false);

	TSharedPtr<FInternetAddr> InternetAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bValidIP;
	InternetAddress->SetIp(*Host, bValidIP);
	InternetAddress->SetPort(Port);

	if (!bValidIP)
	{
		FVAUtils::LogStuff("[FVAServerLauncher::RemoteStartVAServer()]: The Ip cannot be parsed!", true);
		return false;
	}

	if (VAServerLauncherSocket == nullptr || !VAServerLauncherSocket->Connect(*InternetAddress))
	{
		FVAUtils::LogStuff("[FVAServerLauncher::RemoteStartVAServer()]: Cannot connect to Launcher!", true);
		return false;
	}
	FVAUtils::LogStuff("[FVAServerLauncher::RemoteStartVAServer()]: Successfully connected to Launcher", false);

	//Send requested version
	TArray<uint8> RequestData = ConvertString(VersionName);	
	int BytesSend = 0;
	VAServerLauncherSocket->Send(RequestData.GetData(), RequestData.Num(), BytesSend);
	FVAUtils::LogStuff("[FVAServerLauncher::RemoteStartVAServer()]: Send " + FString::FromInt(BytesSend) + 
		" bytes to the VAServer Launcher, with version name: " + VersionName + " Waiting for answer.", false);

	//Receive response
	const int32 BufferSize = 16;
	int32 BytesRead = 0;
	uint8 Response[16];
	if (VAServerLauncherSocket->Recv(Response, BufferSize, BytesRead) && BytesRead == 1)
	{
		switch (Response[0])
		{
		case 'g':
			FVAUtils::LogStuff("[FVAServerLauncher::RemoteStartVAServer()]: Received go from launcher, VAServer seems to be correctly started.", false);
			break;
		case 'n':
			FVAUtils::OpenMessageBox("[FVAServerLauncher::RemoteStartVAServer()]: VAServer cannot be launched, invalid VAServer binary file or cannot be found",
			                        true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'i':
			FVAUtils::OpenMessageBox("[FVAServerLauncher::RemoteStartVAServer()]: VAServer cannot be launched, invalid file entry in the config", true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'a':
			FVAUtils::OpenMessageBox("[FVAServerLauncher::RemoteStartVAServer()]: VAServer was aborted", true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'f':
			FVAUtils::OpenMessageBox("[FVAServerLauncher::RemoteStartVAServer()]: VAServer cannot be launched, requested version \"" + 
				VersionName + "\" is not available/specified", true);
			VAServerLauncherSocket = nullptr;
			return false;
		default:
			FVAUtils::OpenMessageBox("[FVAServerLauncher::RemoteStartVAServer()]: Unexpected response from VAServer Launcher: " + 
				FString(reinterpret_cast<char*>(&Response[0])), true);
			VAServerLauncherSocket = nullptr;
			return false;
		}
	}
	else
	{
		FVAUtils::LogStuff("[FVAServerLauncher::RemoteStartVAServer()]: Error while receiving response from VAServer Launcher", true);
		VAServerLauncherSocket = nullptr;
		return false;
	}
	return true;
}

bool FVAServerLauncher::StartVAServerLauncher()
{
  //check whether we can also start the VSServer Launcher python script.

	if (!UVirtualRealityUtilities::IsMaster())
	{
		return false;
	}

  const UVASettings* Settings = GetDefault<UVASettings>();
  FString LauncherScriptDir = Settings->VALauncherPath;

  if(FPaths::IsRelative(LauncherScriptDir))
  {
    FString ProjectDir = FPaths::ProjectDir();
    ProjectDir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*ProjectDir);
    LauncherScriptDir = FPaths::ConvertRelativePathToFull(ProjectDir, LauncherScriptDir);
  }

  LauncherScriptDir = FPaths::Combine(LauncherScriptDir, TEXT("LaunchScript"));
  FString LauncherScript = TEXT("VirtualAcousticsStarterServer.py");
  if (FPaths::FileExists(FPaths::Combine(LauncherScriptDir, LauncherScript)))
  {
    FString command = "cd/d "+ LauncherScriptDir+" & start py " + LauncherScript;
    system(TCHAR_TO_ANSI(*command));
    return true;
  }
  else
  {
    FVAUtils::LogStuff("[FVAServerLauncher::StartVAServerLauncher] Unable to automatically start the launcher script, looked for "+LauncherScript+" at "+LauncherScriptDir+". If you want to use this convenience function change the VALauncher Path in the Engine/Virtual Acoustics(VA) section of the project settings. However, nothing bad will happen without.");
  }
  return false;
}

bool FVAServerLauncher::SendFileToVAServer(const FString& RelativeFilename)
{

	if (!UVirtualRealityUtilities::IsMaster())
	{
		return false;
	}
	
	if(VAServerLauncherSocket==nullptr)
	{
		FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: No connection to VAServer Starter, so no files can be send to VAServer!", true);
		return false;
	}

	if(!GetDefault<UVASettings>()->VALauncherCopyFiles)
	{
		FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: Setting to not send files over the network to VAServer is set, so not sending anything!", false);
		return false;
	}

	if(!FPaths::FileExists(FPaths::Combine(FPaths::ProjectContentDir(),RelativeFilename)))
	{
		FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: File to send("+RelativeFilename+") could not be found and therefore not send!", true);
		return false;
	}

	TArray<uint8> FileBinaryArray;
   FFileHelper::LoadFileToArray(FileBinaryArray, *FPaths::Combine(FPaths::ProjectContentDir(),RelativeFilename));

	const FString ProjectName = GetDefault<UGeneralProjectSettings>()->ProjectName;
	FString MetaInfo = "file:"+RelativeFilename+":"+FString::FromInt(FileBinaryArray.Num())+":"+ProjectName;
	TArray<uint8> MetaInfoBinary = ConvertString(MetaInfo);

	int32 BytesSend;
	VAServerLauncherSocket->Send(MetaInfoBinary.GetData(), MetaInfoBinary.Num(), BytesSend);
	//Receive response
	const int32 BufferSize = 16;
	int32 BytesRead = 0;
	uint8 Response[16];
	if (VAServerLauncherSocket->Recv(Response, BufferSize, BytesRead))
	{
		FString ResponseString = ByteArrayToString(Response, BytesRead);
		if(ResponseString=="ack"){
			//VAServer waits for file
			int32 BytesAlreadySend = 0;
			while(BytesAlreadySend<FileBinaryArray.Num())
			{
				//send 1024 byte packages
				int32 BytesToSend = (FileBinaryArray.Num()-BytesAlreadySend>1024?1024:FileBinaryArray.Num()-BytesAlreadySend);
				VAServerLauncherSocket->Send(&FileBinaryArray[BytesAlreadySend],BytesToSend, BytesSend);
				BytesAlreadySend += BytesSend;
			}
			FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: Entire file ("+RelativeFilename+") send!", false);
			VAServerLauncherSocket->Recv(Response, BufferSize, BytesRead);
			if(BytesRead==3 && Response[0]=='a' && Response[1]=='c' && Response[2]=='k')
			{
				FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: File was received by VAServerLauncher successfully!", false);
			}
			else
			{
				FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: File was NOT received by VAServerLauncher!", true);
				return false;
			}
		}
		else if (ResponseString=="exists"){
			FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: File already exists with same size, no need ro re-send!", false);
		}
		else
		{
			FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: Server Launcher does not want to receive a file, answer: "+ResponseString, true);
			return false;	
		}
	}
	else
	{
		FVAUtils::LogStuff("[FVAServerLauncher::SendFileToVAServer()]: Server Launcher does not want to receive a file, no answer!", true);
		return false;	
	}

	std::string SearchPath = "../tmp/"+std::string(TCHAR_TO_UTF8(*ProjectName));
	FVAPlugin::AddVAServerSearchPath(SearchPath);

	return true;
}

void FVAServerLauncher::ReleaseVAServerLauncherConnection()
{
	if (!UVirtualRealityUtilities::IsMaster())
	{
		return;
	}

	if(VAServerLauncherSocket!=nullptr){
		VAServerLauncherSocket->Close();
		VAServerLauncherSocket = nullptr;
	}
}

bool FVAServerLauncher::IsVAServerLauncherConnected()
{
	return VAServerLauncherSocket!=nullptr;
}

TArray<uint8> FVAServerLauncher::ConvertString(const FString& String)
{
	TArray<uint8> RequestData;
	for (TCHAR Character : String.GetCharArray())
	{
		const uint8 InByte = static_cast<uint8>(Character);
		if (InByte != 0)
		{
			RequestData.Add(static_cast<uint8>(Character));
		}
	}
	return RequestData;
}

FString FVAServerLauncher::ByteArrayToString(const uint8* In, int32 Count)
{
	FString Result;
	Result.Empty(Count);

	while (Count)
	{
		// Put the byte into an int16
		int16 Value = *In;

		Result += TCHAR(Value);

		++In;
		Count--;
	}
	return Result;
}