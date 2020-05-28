#include "VAPlugin.h"

// #pragma warning(disable:5038) //disable initilization order warning
#include "VA.h"
#include "VANet.h"
// #pragma warning(default:5038)

#include "VAUtils.h"

#include "VAReflectionWall.h"
#include "VAReceiverActor.h"
#include "VASourceComponent.h"

#include "Engine.h"
#include "Networking.h"
#include "Core.h"

#include "Interfaces/IPluginManager.h"
#include "Cluster/IDisplayClusterClusterManager.h"
#include "Containers/UnrealString.h"

#include "IDisplayCluster.h"
#include "SocketSubsystem.h"


#if WITH_EDITOR
#include "Editor.h"
#endif



//#include "VistaBase/VistaTimeUtils.h"


#define LOCTEXT_NAMESPACE "FVAPluginModule"

// Library Handles for dll loading of VA Classes
void* FVAPlugin::LibraryHandleNet;
void* FVAPlugin::LibraryHandleBase;
void* FVAPlugin::LibraryHandleVistaAspects;
void* FVAPlugin::LibraryHandleVistaBase;
void* FVAPlugin::LibraryHandleVistaInterProcComm;

// Vars for setting of usage
bool FVAPlugin::bPluginInitialized = false;
bool FVAPlugin::bUseVA = true;
bool FVAPlugin::bDebugMode = true;
bool FVAPlugin::bIsMaster = false;


// Interface Classes to Server 
IVANetClient* FVAPlugin::VANetClient;
IVAInterface* FVAPlugin::VAServer;

// Link to the current receiver actor 
AVAReceiverActor* FVAPlugin::ReceiverActor;

// Scale of the UE4 world (how many units is 1m in "real life")
float FVAPlugin::WorldScale = 100.0f;

// tmp Var for easier usage
TSharedPtr<VAQuat> FVAPlugin::TmpVAQuatSharedPtr;
TSharedPtr<VAVec3> FVAPlugin::TmpVAVec3SharedPtr;

VAQuat* FVAPlugin::TmpVAQuat;
VAVec3* FVAPlugin::TmpVAVec3;

FSocket* FVAPlugin::VAServerLauncherSocket = nullptr;


// ****************************************************************** // 
// ******* REMAKE!!!!!! ********************************************* //
// ****************************************************************** //

void FVAPlugin::ProcessException(const FString Location, const CVAException Exception)
{
	ProcessException(Location, FString(Exception.ToString().c_str()));
}

void FVAPlugin::ProcessException(const FString Location, const FString ExceptionString)
{
	FString output = "[FVAPlugin::ProcessException()]: Exception from [";
	output.Append(Location).Append("] with error message: ").Append(ExceptionString);
	FVAUtils::LogStuff(output, true);
	FVAUtils::OpenMessageBox(output, true);
}


// ****************************************************************** // 
// ******* Initialization Functions ********************************* //
// ****************************************************************** //

void FVAPlugin::StartupModule()
{
	// ++ Get Paths of DLL Handles ++ //
	const FString BaseDir = IPluginManager::Get().FindPlugin("VAPlugin")->GetBaseDir();
	FString PathNet, PathBase, PathVistaAspects, PathVistaBase, PathVistaInterProcComm;

#if PLATFORM_WINDOWS
	PathBase = FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VABase.dll"));
	PathVistaAspects = FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaAspects.dll"));
	PathVistaBase = FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaBase.dll"));
	PathVistaInterProcComm = FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaInterProcComm.dll"));
	PathNet = FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VANet.dll"));


	// ++ Load DLL Handles ++ // 
	LibraryHandleBase					= !PathBase.IsEmpty() ? FPlatformProcess::GetDllHandle(*PathBase) : nullptr;
	LibraryHandleVistaBase				= !PathVistaBase.IsEmpty() ? FPlatformProcess::GetDllHandle(*PathVistaBase) : nullptr;
	LibraryHandleVistaAspects			= !PathVistaAspects.IsEmpty() ? FPlatformProcess::GetDllHandle(*PathVistaAspects) : nullptr;
	LibraryHandleVistaInterProcComm		= !PathVistaInterProcComm.IsEmpty() ? FPlatformProcess::GetDllHandle(*PathVistaInterProcComm) : nullptr;
	LibraryHandleNet					= !PathNet.IsEmpty() ? FPlatformProcess::GetDllHandle(*PathNet) : nullptr;

	// ++ Check Handles ++ //
	if (!CheckLibraryHandles())
	{
		FVAUtils::OpenMessageBox("[FVAPlugin::StartupModule()]: Stop initializing because of incomplete libraryHandles");
		return;
	}

#elif PLATFORM_LINUX
	pathBase				=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VABase.so"));
	pathVistaAspects		=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaAspects.so"));
	pathVistaBase			=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaBase.so"));
	pathVistaInterProcComm	=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaInterProcComm.so"));
	pathNet					=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VANet.so"));
#endif 

	
#if WITH_EDITOR
	FEditorDelegates::BeginPIE.AddRaw(this, &FVAPlugin::BeginSession);
	FEditorDelegates::EndPIE.AddRaw(this, &FVAPlugin::EndSession);
#endif

	// TODO new 
	TmpVAQuatSharedPtr = MakeShared<VAQuat>();
	TmpVAVec3SharedPtr = MakeShared<VAVec3>();

	TmpVAQuat = TmpVAQuatSharedPtr.Get();
	TmpVAVec3 = TmpVAVec3SharedPtr.Get();
}

void FVAPlugin::BeginSession(const bool bSomething)
{
	bPluginInitialized = false;
	SetUseVA(true);
	SetDebugMode(true);
	bIsMaster = IDisplayCluster::Get().GetClusterMgr() != nullptr && IDisplayCluster::Get().GetClusterMgr()->IsMaster();
}

void FVAPlugin::EndSession(const bool bSomething)
{
	if (VAServerLauncherSocket != nullptr)
	{
		VAServerLauncherSocket->Close();
		VAServerLauncherSocket = nullptr;
	}
}

void FVAPlugin::ShutdownModule()
{
	DisconnectServer();

#if PLATFORM_WINDOWS
	FPlatformProcess::FreeDllHandle(LibraryHandleNet);
	FPlatformProcess::FreeDllHandle(LibraryHandleBase);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaAspects);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaBase);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaInterProcComm);
#endif // PLATFORM_WINDOWS

	if (VAServerLauncherSocket != nullptr)
	{
		VAServerLauncherSocket->Close();
		VAServerLauncherSocket = nullptr;
	}
}

void FVAPlugin::AskForSettings(const FString Host, const int Port, const bool bAskForDebugMode, const bool bAskForUseVA)
{
	if (bPluginInitialized == true)
	{
		return;
	}

	bIsMaster = IDisplayCluster::Get().GetClusterMgr() != nullptr && IDisplayCluster::Get().GetClusterMgr()->IsMaster();

	if (!bIsMaster)
	{
		return;
	}

	if (bAskForUseVA)
	{
		const EAppReturnType::Type ReturnUseVA = FMessageDialog::Open(EAppMsgType::YesNo,
		                                                      FText::FromString(
			                                                      "Use VA Server (" + Host + ":" + FString::FromInt(Port) +
			                                                      ")? If yes, make sure to have it switched on."));
		if (ReturnUseVA == EAppReturnType::Type::Yes)
		{
			SetUseVA(true);
		}
		else
		{
			SetUseVA(false);
			SetDebugMode(false);
			bPluginInitialized = true;
			return;
		}
	}

	if (bAskForDebugMode)
	{
		const EAppReturnType::Type ReturnDebugMode = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString("Start in Debug mode?"));
		if (ReturnDebugMode == EAppReturnType::Type::Yes)
		{
			bDebugMode = true;
		}
		else
		{
			bDebugMode = false;
		}
	}
	else
	{
		bDebugMode = false;
	}


	bPluginInitialized = true;
}

bool FVAPlugin::CheckLibraryHandles()
{
	bool bSuc = true;
	if (!LibraryHandleNet)
	{
		FVAUtils::LogStuff("[FVAPlugin::CheckLibraryHandles()]: Could not load Net", true);
		bSuc = false;
	}

	if (!LibraryHandleBase)
	{
		FVAUtils::LogStuff("[FVAPlugin::CheckLibraryHandles()]: Could not load Base", true);
		bSuc = false;
	}

	if (!LibraryHandleVistaAspects)
	{
		FVAUtils::LogStuff("[FVAPlugin::CheckLibraryHandles()]: Could not load Vista Aspects", true);
		bSuc = false;
	}

	if (!LibraryHandleVistaBase)
	{
		FVAUtils::LogStuff("[FVAPlugin::CheckLibraryHandles()]: Could not load Vista Base", true);
		bSuc = false;
	}

	if (!LibraryHandleVistaInterProcComm)
	{
		FVAUtils::LogStuff("[FVAPlugin::CheckLibraryHandles()]: could not load Vista InterProcComm", true);
		bSuc = false;
	}
	return bSuc;
}


// ****************************************************************** // 
// ******* General Server Functions ********************************* //
// ****************************************************************** //

bool FVAPlugin::ConnectServer(const FString HostF, const int Port)
{
	if (!bIsMaster || !bUseVA)
	{
		return false;
	}

	if (IsConnected())
	{
		ResetServer();
		return true;
	}

	FVAUtils::LogStuff("[FVAPlugin::ConnectServer()]: Connecting to VAServer. Be sure to have it switched on", false);

	try
	{
		VANetClient = IVANetClient::Create();

		const std::string HostS(TCHAR_TO_UTF8(*HostF));
		VANetClient->Initialize(HostS, Port);
		if (!VANetClient->IsConnected())
		{
			FVAUtils::OpenMessageBox("[FVAPlugin::ConnectServer()]: Could not connect to VA Server", true);
			bUseVA = false;
			return false;
		}

		VAServer = VANetClient->GetCoreInstance();
		VAServer->Reset();
	}
	catch (CVAException& e)
	{
		bUseVA = false;
		ProcessException("FVAPluginModule::connectServer()", FString(e.ToString().c_str()));

		return false;
	}

	FVAUtils::LogStuff("[FVAPlugin::ConnectServer()]: Connected to the VA Server", false);

	return true;
}

bool FVAPlugin::ResetServer()
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	try
	{
		FVAUtils::LogStuff("[FVAPlugin::ResetServer()]: Resetting Server now.", false);
		VAServer->Reset();
	}
	catch (CVAException& e)
	{
		bUseVA = false;
		ProcessException("FVAPluginModule::resetServer()", FString(UTF8_TO_TCHAR(e.ToString().c_str())));

		return false;
	}

	return true;
}

bool FVAPlugin::IsConnected()
{
	if (!bIsMaster || !bUseVA)
	{
		return false;
	}

	if (VANetClient == nullptr || VAServer == nullptr)
	{
		return false;
	}

	try
	{
		return VANetClient->IsConnected();
	}
	catch (CVAException& e)
	{
		bUseVA = false;
		ProcessException("FVAPluginModule::isConnected() ", FString(e.ToString().c_str()));
		
		return false;
	}
}

bool FVAPlugin::DisconnectServer()
{
	if (!ShouldInteractWithServer())
	{
		return true;
	}
	
	FVAUtils::LogStuff("[FVAPluginModule::disconnectServer()]: Disconnecting now", false);

	if (VAServerLauncherSocket != nullptr)
	{
		VAServerLauncherSocket->Close();
		VAServerLauncherSocket = nullptr;
	}
	else
	{
		VAServer->Finalize();

		if (VANetClient != nullptr)
		{
			if (VANetClient->IsConnected())
			{
				VANetClient->Disconnect();
			}
		}
	}
	return true;
}

bool FVAPlugin::RemoteStartVAServer(const FString& Host, const int Port, const FString& VersionName)
{
	bIsMaster = IDisplayCluster::Get().GetClusterMgr() != nullptr && IDisplayCluster::Get().GetClusterMgr()->IsMaster();

	if (!bIsMaster)
	{
		return false;
	}

	if (VAServerLauncherSocket != nullptr)
	{
		return true;
	}

	FVAUtils::LogStuff("[FVAPlugin::RemoteStartVAServer()]: Try to remotely start the VAServer at address " + 
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
		FVAUtils::LogStuff("[FVAPlugin::RemoteStartVAServer()]: The Ip cannot be parsed!", true);
		return false;
	}

	if (VAServerLauncherSocket == nullptr || !VAServerLauncherSocket->Connect(*InternetAddress))
	{
		FVAUtils::LogStuff("[FVAPlugin::RemoteStartVAServer()]: Cannot connect to Launcher!", true);
		return false;
	}
	FVAUtils::LogStuff("[FVAPlugin::RemoteStartVAServer()]: Successfully connected to Launcher", false);

	//Send requested version
	TArray<uint8> RequestData;
	for (TCHAR Character : VersionName.GetCharArray())
	{
		const uint8 InByte = static_cast<uint8>(Character);
		if (InByte != 0)
		{
			RequestData.Add(static_cast<uint8>(Character));
		}
	}
	
	int BytesSend = 0;
	VAServerLauncherSocket->Send(RequestData.GetData(), RequestData.Num(), BytesSend);
	FVAUtils::LogStuff("[FVAPlugin::RemoteStartVAServer()]: Send " + FString::FromInt(BytesSend) + 
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
			FVAUtils::LogStuff("[FVAPlugin::RemoteStartVAServer()]: Received go from launcher, VAServer seems to be correctly started.", false);
			break;
		case 'n':
			FVAUtils::OpenMessageBox("[FVAPlugin::RemoteStartVAServer()]: VAServer cannot be launched, invalid VAServer binary file or cannot be found",
			                        true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'i':
			FVAUtils::OpenMessageBox("[FVAPlugin::RemoteStartVAServer()]: VAServer cannot be launched, invalid file entry in the config", true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'a':
			FVAUtils::OpenMessageBox("[FVAPlugin::RemoteStartVAServer()]: VAServer was aborted", true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'f':
			FVAUtils::OpenMessageBox("[FVAPlugin::RemoteStartVAServer()]: VAServer cannot be launched, requested version \"" + 
				VersionName + "\" is not available/specified", true);
			VAServerLauncherSocket = nullptr;
			return false;
		default:
			FVAUtils::OpenMessageBox("[FVAPlugin::RemoteStartVAServer()]: Unexpected response from VAServer Launcher: " + 
				FString(reinterpret_cast<char*>(&Response[0])), true);
			VAServerLauncherSocket = nullptr;
			return false;
		}
	}
	else
	{
		FVAUtils::LogStuff("[FVAPlugin::RemoteStartVAServer()]: Error while receiving response from VAServer Launcher", true);
		VAServerLauncherSocket = nullptr;
		return false;
	}
	FVAUtils::LogStuff("[FVAPlugin::RemoteStartVAServer()]: End of function", false);
	return true;
}


// ****************************************************************** // 
// ******* Sound Buffer ********************************************* //
// ****************************************************************** //

std::string FVAPlugin::CreateNewBuffer(const FString SoundFileName, const bool bLoop, float SoundOffset)
{
	if (!ShouldInteractWithServer())
	{
		return "-1";
	}

	if (SoundOffset < 0.0f)
	{
		SoundOffset = 0;
	}
	
	const std::string FileName(TCHAR_TO_UTF8(*SoundFileName));

	try
	{
		const std::string SignalSourceID = VAServer->CreateSignalSourceBufferFromFile(FileName);

		VAServer->SetSignalSourceBufferLooping(SignalSourceID, bLoop);
		VAServer->SetSignalSourceBufferPlaybackPosition(SignalSourceID, SoundOffset);
		VAServer->SetSignalSourceBufferPlaybackAction(SignalSourceID, 0);

		return SignalSourceID;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::CreateNewBuffer()", FString(e.ToString().c_str()));
		return "-1";
	}
}

bool FVAPlugin::SetSoundBufferAction(const std::string BufferID, const EPlayAction::Type Action)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	
	if (BufferID == "-1")
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundBufferAction()]: BufferID invalid (=-1)", true);
		return false;
	}

	try
	{
		VAServer->SetSignalSourceBufferPlaybackAction(BufferID, Action);

		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundSourceAction()", FString(e.ToString().c_str()));
		return false;
	}
}

int FVAPlugin::GetSoundBufferAction(const std::string BufferID)
{
	if (!ShouldInteractWithServer())
	{
		return -1;
	}

	if (BufferID == "-1")
	{
		FVAUtils::LogStuff("[FVAPlugin::GetSoundBufferAction()]: BufferID invalid (=-1)", true);
		return -1;
	}
	
	try
	{
		const int State = VAServer->GetSignalSourceBufferPlaybackState(BufferID);
		return State;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::GetSoundBufferAction()", FString(e.ToString().c_str()));
		return -1;
	}
}

bool FVAPlugin::SetSoundBufferTime(const std::string BufferID, const float Time)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (BufferID == "-1")
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundBufferTime()]: BufferID invalid (=-1)", true);
		return false;
	}

	try
	{
		VAServer->SetSignalSourceBufferPlaybackPosition(BufferID, Time);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundBufferTime()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSoundBufferLoop(const std::string BufferID, const bool bLoop)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (BufferID == "-1")
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundBufferLoop()]: BufferID invalid (=-1)", true);
		return false;
	}


	try
	{
		VAServer->SetSignalSourceBufferLooping(BufferID, bLoop);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundBufferLoop()", FString(e.ToString().c_str()));
		return false;
	}
}


// ****************************************************************** // 
// ******* Sound Sources ******************************************** //
// ****************************************************************** //

int FVAPlugin::CreateNewSoundSource(const std::string BufferID, const std::string Name,
                                    FVector Pos, FRotator Rot, const float Power)
{
	if (!ShouldInteractWithServer())
	{
		return -1;
	}

	if (BufferID == "-1")
	{
		FVAUtils::LogStuff("[FVAPlugin::CreateNewSoundSource()]: BufferID invalid (=-1)", true);
		return -1;
	}

	Pos = FVAUtils::ToVACoordinateSystem(Pos, WorldScale);
	FVAUtils::FVecToVAVec3(Pos, *TmpVAVec3);

	Rot = FVAUtils::ToVACoordinateSystem(Rot);
	FQuat QuatF = Rot.Quaternion();
	FVAUtils::FQuatToVAQuat(QuatF, *TmpVAQuat);


	try
	{
		const int SoundSourceID = VAServer->CreateSoundSource(Name);

		VAServer->SetSoundSourcePose(SoundSourceID, *TmpVAVec3, *TmpVAQuat);

		if (Power != -1.0f && Power >= 0.0f)
		{
			VAServer->SetSoundSourceSoundPower(SoundSourceID, Power);
		}
		
		VAServer->SetSoundSourceSignalSource(SoundSourceID, BufferID);
		return SoundSourceID;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::CreateNewSoundSource()", FString(e.ToString().c_str()));
		return -1;
	}
}

bool FVAPlugin::SetSoundSourcePosition(const int SoundSourceID, FVector Pos)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == -1)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourcePos()]: SoundSourceID invalid (= -1)", true);
		return false;
	}

	Pos = FVAUtils::ToVACoordinateSystem(Pos, WorldScale);
	FVAUtils::FVecToVAVec3(Pos, *TmpVAVec3);


	try
	{
		VAServer->SetSoundSourcePosition(SoundSourceID, *TmpVAVec3);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundSourcePos()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSoundSourceRotation(const int SoundSourceID, FRotator Rot)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == -1)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourceRot()]: SoundSourceID invalid (= -1)", true);
		return false;
	}
	
	Rot = FVAUtils::ToVACoordinateSystem(Rot);
	FQuat QuatF = Rot.Quaternion();
	FVAUtils::FQuatToVAQuat(QuatF, *TmpVAQuat);

	try
	{
		VAServer->SetSoundSourceOrientation(SoundSourceID, *TmpVAQuat);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundSourceRot()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetNewBufferForSoundSource(const int SoundSourceID, const std::string BufferID)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == -1)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetNewBufferForSoundSource()]: SoundSourceID invalid (= -1)", true);
		return false;
	}

	if (BufferID == "-1")
	{
		FVAUtils::LogStuff("[FVAPlugin::SetNewBufferForSoundSource()]: BufferID invalid (=-1)", true);
		return false;
	}
	
	try
	{
		VAServer->SetSoundSourceSignalSource(SoundSourceID, BufferID);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetNewBufferForSoundSource()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSoundSourceMuted(const int SoundSourceID, const bool bMuted)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == -1)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourceMuted()]: SoundSourceID invalid (= -1)", true);
		return false;
	}
	
	try
	{
		VAServer->SetSoundSourceMuted(SoundSourceID, bMuted);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundSourceMuted()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSoundSourcePower(const int SoundSourceID, const float Power)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == -1)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourcePower()]: SoundSourceID invalid (= -1)", true);
		return false;
	}

	try
	{
		VAServer->SetSoundSourceSoundPower(SoundSourceID, Power);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundSourcePower()", FString(e.ToString().c_str()));
		return false;
	}
}


// ****************************************************************** // 
// ******* Directivities ******************************************** //
// ****************************************************************** //

int FVAPlugin::CreateNewDirectivity(const FString FileName)
{
	if (!ShouldInteractWithServer())
	{
		return -1;
	}

	try
	{
		const std::string FileNameSTD(TCHAR_TO_UTF8(*FileName));
		return VAServer->CreateDirectivityFromFile(FileNameSTD);
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::CreateNewDirectivity()",
		                FString(e.ToString().c_str()) + " (" + FileName + ")");
		return -1;
	}
}

bool FVAPlugin::SetSoundSourceDirectivity(const int SoundSourceID, const int DirectivityID)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == -1)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourceDirectivity()]: SoundSourceID invalid (= -1)", true);
		return false;
	}

	if (DirectivityID == -1)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourceDirectivity()]: DirectivityID invalid (=-1)", true);
		return false;
	}


	try
	{
		VAServer->SetSoundSourceDirectivity(SoundSourceID, DirectivityID);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundSourceDirectivity()", FString(e.ToString().c_str()));
		return false;
	}
}

// ****************************************************************** // 
// ******* HRIR ***************************************************** //
// ****************************************************************** //

int FVAPlugin::CreateNewHRIR(const FString FileName)
{
	if (!ShouldInteractWithServer())
	{
		return -1;
	}

	try
	{
		const std::string DirHRIR(TCHAR_TO_UTF8(*FileName));
		return VAServer->CreateDirectivityFromFile(DirHRIR);
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::CreateNewHRIR()", FString(e.ToString().c_str()) + " (" + FileName + ")");
		return -1;
	}
}


bool FVAPlugin::SetSoundReceiverHRIR(const int SoundReceiverID, const int HRIRID)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (HRIRID == -1)
	{
		FVAUtils::LogStuff("[SetSoundReceiverHRIR()]: HRIR is not valid (id = -1)", true);
		return false;
	}

	try
	{
		VAServer->SetSoundReceiverDirectivity(SoundReceiverID, HRIRID);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::setSoundReceiverDirectivity()", FString(e.ToString().c_str()));
		return false;
	}
}

// ****************************************************************** // 
// ******* Sound Receiver ******************************************* //
// ****************************************************************** //

int FVAPlugin::CreateNewSoundReceiver(AVAReceiverActor* Actor)
{
	ReceiverActor = Actor;

	if (!ShouldInteractWithServer())
	{
		return -1;
	}

	WorldScale = ReceiverActor->GetScale();

	try
	{
		const int SoundReceiverID = VAServer->CreateSoundReceiver("VASoundReceiver");
		const int HRIR = FVAHRIRManager::GetDefaultHRIR()->GetID();

		if (HRIR != -1)
		{
			VAServer->SetSoundReceiverDirectivity(SoundReceiverID, HRIR);
			FVAUtils::LogStuff("[FVAPlugin::CreateNewSoundReceiver()]: Default Directivity is not initialized, using no Directivity", false);
		}
		
		return SoundReceiverID;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::createNewSoundReceiver()", FString(e.ToString().c_str()));
		return -1;
	}
}


bool FVAPlugin::SetSoundReceiverPosition(const int SoundReceiverID, FVector Pos)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundReceiverID == -1)
	{
		FVAUtils::LogStuff("[SetSoundReceiverPosition()]: SoundReceiverID is not valid (id = -1)", true);
		return false;
	}

	Pos = FVAUtils::ToVACoordinateSystem(Pos, WorldScale);
	FVAUtils::FVecToVAVec3(Pos, *TmpVAVec3);
	
	try
	{
		VAServer->SetSoundReceiverPosition(SoundReceiverID, *TmpVAVec3);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::setSoundReceiverPosition()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSoundReceiverRotation(const int SoundReceiverID, FRotator Rot)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundReceiverID == -1)
	{
		FVAUtils::LogStuff("[SetSoundReceiverRotation()]: SoundReceiverID is not valid (id = -1)", true);
		return false;
	}
	
	Rot = FVAUtils::ToVACoordinateSystem(Rot);
	FQuat Quat = Rot.Quaternion();
	FVAUtils::FQuatToVAQuat(Quat, *TmpVAQuat);

	try
	{
		VAServer->SetSoundReceiverOrientation(SoundReceiverID, *TmpVAQuat);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundReceiverRotation()", FString(e.ToString().c_str()));
		return false;
	}
}


// ****************************************************************** // 
// ******* Real World *********************************************** //
// ****************************************************************** //

bool FVAPlugin::SetSoundReceiverRealWorldPose(const int SoundReceiverID, FVector Pos, FRotator Rot)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	
	if (SoundReceiverID == -1)
	{
		FVAUtils::LogStuff("[SetSoundReceiverRealWorldPose()]: SoundReceiverID is not valid (id = -1)", true);
		return false;
	}

	Pos = FVAUtils::ToVACoordinateSystem(Pos, WorldScale);
	FVAUtils::FVecToVAVec3(Pos, *TmpVAVec3);

	Rot = FVAUtils::ToVACoordinateSystem(Rot);
	FQuat quat = Rot.Quaternion();
	FVAUtils::FQuatToVAQuat(quat, *TmpVAQuat);

	try
	{
		VAServer->SetSoundReceiverRealWorldPose(SoundReceiverID, *TmpVAVec3, *TmpVAQuat);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("SetSoundReceiverRealWorldPose()", FString(e.ToString().c_str()));
		return false;
	}
}


// ****************************************************************** // 
// ******* General Setter Functions ********************************* //
// ****************************************************************** //

void FVAPlugin::SetReceiverActor(AVAReceiverActor* Actor)
{
	ReceiverActor = Actor;
}

void FVAPlugin::SetScale(const float ScaleN)
{
	WorldScale = ScaleN;
}

void FVAPlugin::SetUseVA(const bool bUseVAN)
{
	// VA cannot be activated once it was deactivated
	if(bUseVA == false || bUseVA == bUseVAN)
	{
		return;
	}
	
	bUseVA = bUseVAN;
	if (bUseVAN)
	{
		ReceiverActor->RunOnAllNodes("useVA = true");
	}
	else
	{
		ReceiverActor->RunOnAllNodes("useVA = false");
		DisconnectServer();
	}
	
}

void FVAPlugin::SetDebugMode(const bool bDebugModeN)
{
	if (bDebugMode == bDebugModeN)
	{
		return;
	}
	bDebugMode = bDebugModeN;

	if (bDebugMode)
	{
		ReceiverActor->RunOnAllNodes("debugMode = true");
	}
	else
	{
		ReceiverActor->RunOnAllNodes("debugMode = false");
	}

	TArray<AActor*> ActorArray;
	UGameplayStatics::GetAllActorsOfClass(ReceiverActor->GetWorld(), AActor::StaticClass(), ActorArray);

	for (AActor* EntryActor : ActorArray)
	{
		TArray<UActorComponent*> VAComponents = EntryActor->GetComponentsByClass(UVASourceComponent::StaticClass());
		for (UActorComponent* EntryVAComponents : VAComponents)
		{
			(Cast<UVASourceComponent>(EntryVAComponents))->SetVisibility(bDebugMode);
		}
	}
}

// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //


bool FVAPlugin::GetIsInitialized()
{
	return bPluginInitialized;
}

bool FVAPlugin::GetUseVA()
{
	if (!bPluginInitialized)
	{
		AskForSettings();
	}

	return bUseVA;
}

bool FVAPlugin::GetDebugMode()
{
	if (!bPluginInitialized)
	{
		AskForSettings();
	}

	return bDebugMode;
}

bool FVAPlugin::GetIsMaster()
{
	if (!bPluginInitialized)
	{
		AskForSettings();
	}

	return bIsMaster;
}

bool FVAPlugin::ShouldInteractWithServer()
{
	if (!bPluginInitialized)
	{
		AskForSettings();
	}

	return (bIsMaster && bUseVA && IsConnected());
}

AVAReceiverActor* FVAPlugin::GetReceiverActor()
{
	return ReceiverActor;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVAPlugin, VAPlugin)
