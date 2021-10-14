#include "VAPlugin.h"
#include "VADefines.h"

#if PLATFORM_WINDOWS
#pragma warning(disable:5038) //disable initilization order warning in MSVC
#elif PLATFORM_LINUX
#pragma GCC diagnostic ignored "-Wreorder-ctor" //also for gcc
#endif

#include "VA.h"
#include "VANet.h"

#if PLATFORM_WINDOWS
#pragma warning(default:5038)
#elif PLATFORM_LINUX
#pragma GCC diagnostic pop
#endif




#include "VAUtils.h"
#include "VAStruct.h"

#include "VAReceiverActor.h"
#include "SoundSource/VAAbstractSourceComponent.h"

#include "Engine.h"
#include "Core.h"

#include "Interfaces/IPluginManager.h"
#include "Containers/UnrealString.h"
#include "Utility/VirtualRealityUtilities.h"

#include "IDisplayCluster.h"

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
bool FVAPlugin::bWasStarted = false;
bool FVAPlugin::bUseVA = true;
bool FVAPlugin::bDebugMode = true;
bool FVAPlugin::bIsMaster = false;


// Interface Classes to Server 
IVANetClient* FVAPlugin::VANetClient;
IVAInterface* FVAPlugin::VAServer;

// Link to the current receiver actor 
AVAReceiverActor* FVAPlugin::ReceiverActor = nullptr;

// Scale of the UE4 world (how many units is 1m in "real life")
float FVAPlugin::WorldScale = 100.0f;

// tmp Var for easier usage
TSharedPtr<VAQuat> FVAPlugin::TmpVAQuatSharedPtr;
TSharedPtr<VAVec3> FVAPlugin::TmpVAVec3SharedPtr;

VAQuat* FVAPlugin::TmpVAQuat;
VAVec3* FVAPlugin::TmpVAVec3;

FVAServerLauncher FVAPlugin::VAServerLauncher;


// ****************************************************************** // 
// ******* REMAKE!!!!!! ********************************************* //
// ****************************************************************** //

void FVAPlugin::ProcessException(const FString Location, const CVAException Exception)
{
	ProcessException(Location, FString(Exception.ToString().c_str()));
}

void FVAPlugin::ProcessException(const FString Location, const FString ExceptionString)
{
	FString output = "Exception from [";
	output.Append(Location).Append("] with error message:\n").Append(ExceptionString);
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
	PathBase				= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VABase.dll"));
	PathVistaAspects		= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaAspects.dll"));
	PathVistaBase			= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaBase.dll"));
	PathVistaInterProcComm	= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaInterProcComm.dll"));
	PathNet					= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VANet.dll"));


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
	PathBase				= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VABase.so"));
	PathVistaAspects		= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaAspects.so"));
	PathVistaBase			= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaBase.so"));
	PathVistaInterProcComm	= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaInterProcComm.so"));
	PathNet					= FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VANet.so"));
#endif 

	
#if WITH_EDITOR
	FEditorDelegates::BeginPIE.AddRaw(this, &FVAPlugin::BeginSession);
	FEditorDelegates::EndPIE.AddRaw(this, &FVAPlugin::EndSession);
#endif

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
	bIsMaster = UVirtualRealityUtilities::IsMaster();
}

void FVAPlugin::EndSession(const bool bSomething)
{
	if(bWasStarted){
		DisconnectServer();
	}
	VAServerLauncher.ReleaseVAServerLauncherConnection();

	ReceiverActor = nullptr;
}

void FVAPlugin::ShutdownModule()
{
	if(bWasStarted){
		DisconnectServer();
	}

#if PLATFORM_WINDOWS
	FPlatformProcess::FreeDllHandle(LibraryHandleNet);
	FPlatformProcess::FreeDllHandle(LibraryHandleBase);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaAspects);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaBase);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaInterProcComm);
#endif // PLATFORM_WINDOWS

	VAServerLauncher.ReleaseVAServerLauncherConnection();
}

void FVAPlugin::AskForSettings(const FString Host, const int Port, const bool bAskForDebugMode, const bool bAskForUseVA)
{
	if (bPluginInitialized == true)
	{
		return;
	}

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
	bWasStarted=true;
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
	if(!bWasStarted)
	{
		return true;
	}
	
	if (!ShouldInteractWithServer() || !IsConnected())
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


	VAServer->Finalize();

	if (VANetClient != nullptr)
	{
		if (VANetClient->IsConnected())
		{
			VANetClient->Disconnect();
		}
	}
	VAServer = nullptr;
	VANetClient = nullptr;

	if (VAServerLauncher.IsVAServerLauncherConnected())
	{
		VAServerLauncher.ReleaseVAServerLauncherConnection();
	}

	return true;
}

void FVAPlugin::AddVAServerSearchPath(const std::string& SearchPath)
{
	CVAStruct Searchpaths = VAServer->GetSearchPaths();
  	bool bAlreayAdded=false;
	for(CVAStruct::const_iterator It = Searchpaths.Begin(); It != Searchpaths.End(); ++It) {
		std::string Path = It->second;
		if(Path == SearchPath){
			bAlreayAdded=true;
		}
	}
	if(!bAlreayAdded){
		VAServer->AddSearchPath(SearchPath);
	}
}

// ****************************************************************** // 
// ******* Signal Sources ******************************************* //
// ****************************************************************** //

std::string FVAPlugin::CreateSignalSourceBuffer(const FString& SoundFileName, const bool bLoop, float SoundOffset)
{
	if (!ShouldInteractWithServer())
	{
		return VA_INVALID_ID_STRING;
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
		ProcessException("FVAPluginModule::CreateSignalSourceBuffer()", FString(e.ToString().c_str()));
		return VA_INVALID_ID_STRING;
	}
}

bool FVAPlugin::SetSignalSourceBufferPlayAction(const std::string& SignalSourceID, const EPlayAction::Type Action)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	
	if (!VA::IsValidID(SignalSourceID))
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSignalSourceBufferPlayAction()]: SignalSourceID invalid (=-1)", true);
		return false;
	}

	try
	{
		VAServer->SetSignalSourceBufferPlaybackAction(SignalSourceID, Action);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundSourceAction()", FString(e.ToString().c_str()));
		return false;
	}
}

int FVAPlugin::GetSignalSourceBufferPlayAction(const std::string& SignalSourceID)
{
	if (!ShouldInteractWithServer())
	{
		return -1;
	}

	if (!VA::IsValidID(SignalSourceID))
	{
		FVAUtils::LogStuff("[FVAPlugin::GetSignalSourceBufferPlayAction()]: SignalSourceID invalid (=-1)", true);
		return -1;
	}
	
	try
	{
		const int State = VAServer->GetSignalSourceBufferPlaybackState(SignalSourceID);
		return State;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::GetSignalSourceBufferPlayAction()", FString(e.ToString().c_str()));
		return -1;
	}
}

bool FVAPlugin::SetSignalSourceBufferPlaybackPosition(const std::string& SignalSourceID, const float Time)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (!VA::IsValidID(SignalSourceID))
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSignalSourceBufferPlaybackPosition()]: SignalSourceID invalid (=-1)", true);
		return false;
	}

	try
	{
		VAServer->SetSignalSourceBufferPlaybackPosition(SignalSourceID, Time);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSignalSourceBufferPlaybackPosition()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSignalSourceBufferLooping(const std::string& SignalSourceID, const bool bLoop)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (!VA::IsValidID(SignalSourceID))
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSignalSourceBufferLooping()]: SignalSourceID invalid (=-1)", true);
		return false;
	}


	try
	{
		VAServer->SetSignalSourceBufferLooping(SignalSourceID, bLoop);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSignalSourceBufferLooping()", FString(e.ToString().c_str()));
		return false;
	}
}

std::string FVAPlugin::CreateSignalSourcePrototype(UVAAbstractSignalSource* SignalSource)
{
	if (!ShouldInteractWithServer())
		return VA_INVALID_ID_STRING;

	if(SignalSource->GetPrototypeName() == "invalid")
	{
		ProcessException("FVAPluginModule::CreateSignalSourcePrototype()", FString("Trying to create unknown signal source class") );
		return VA_INVALID_ID_STRING;
	}

	try
	{
		CVAStruct SignalSourceStruct;
		SignalSourceStruct["class"] = SignalSource->GetPrototypeName();
		return VAServer->CreateSignalSourcePrototypeFromParameters( SignalSourceStruct );
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::CreateSignalSourcePrototype()", FString(e.ToString().c_str()));
		return VA_INVALID_ID_STRING;
	}
}

bool FVAPlugin::DeleteSignalSource(const std::string& SignalSourceID)
{
	if (!ShouldInteractWithServer() || SignalSourceID == VA_INVALID_ID_STRING)
	{
		return false;
	}
	try
	{
		return VAServer->DeleteSignalSource(SignalSourceID) == 0;
	}
	catch (CVAException & e)
	{
		ProcessException("FVAPlugin::DeleteSignalSource()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSignalSourceParameter(const std::string& SignalSourceID, std::string ParamName, float ParamValue)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	if (!VA::IsValidID(SignalSourceID))
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSignalSourceParameter()]: SignalSourceID invalid (=-1)", true);
		return false;
	}


	try
	{
		CVAStruct VAParams;
		VAParams[ParamName] = ParamValue;
		VAServer->SetSignalSourceParameters(SignalSourceID, VAParams);
		return true;
	}
	catch (CVAException & e)
	{
		ProcessException("FVAPlugin::SetSignalSourceParameter()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetJetEngineRMP(const std::string& SignalSourceID, float fRPM)
{
	return SetSignalSourceParameter(SignalSourceID, "rpm", fRPM);
}


// ****************************************************************** // 
// ******* Sound Sources ******************************************** //
// ****************************************************************** //

int FVAPlugin::CreateNewSoundSource(const std::string& Name, FVector Pos, FRotator Rot, const float Power, const std::string& SignalSourceID)
{
	if (!ShouldInteractWithServer())
	{
		return VA_INVALID_ID;
	}

	bool bApplySignalSource = !SignalSourceID.empty();

	if (bApplySignalSource && !VA::IsValidID(SignalSourceID))
	{
		FVAUtils::LogStuff("[FVAPlugin::CreateNewSoundSource()]: SignalSourceID invalid (=-1)", true);
		return VA_INVALID_ID;
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
		
		if (bApplySignalSource)
		{
			VAServer->SetSoundSourceSignalSource(SoundSourceID, SignalSourceID);
		}
		return SoundSourceID;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::CreateNewSoundSource()", FString(e.ToString().c_str()));
		return VA_INVALID_ID;
	}
}

bool FVAPlugin::DeleteSoundSource(const int SoundSourceID)
{
	if (!ShouldInteractWithServer() || SoundSourceID == VA_INVALID_ID)
	{
		return false;
	}
	try
	{
		const bool bSuccess = VAServer->DeleteSoundSource(SoundSourceID) == 0;
		return bSuccess;
	}
	catch (CVAException & e)
	{
		ProcessException("FVAPlugin::DeleteSoundSource()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSoundSourcePosition(const int SoundSourceID, FVector Pos)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == VA_INVALID_ID)
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

	if (SoundSourceID == VA_INVALID_ID)
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

bool FVAPlugin::SetSoundSourceSignalSource(const int SoundSourceID, const std::string& SignalSourceID)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == VA_INVALID_ID)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourceSignalSource()]: SoundSourceID invalid (= -1)", true);
		return false;
	}

	if (!VA::IsValidID(SignalSourceID))
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourceSignalSource()]: SignalSourceID invalid (=-1)", true);
		return false;
	}
	
	try
	{
		VAServer->SetSoundSourceSignalSource(SoundSourceID, SignalSourceID);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::SetSoundSourceSignalSource()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPlugin::SetSoundSourceMuted(const int SoundSourceID, const bool bMuted)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == VA_INVALID_ID)
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

	if (SoundSourceID == VA_INVALID_ID)
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
		return VA_INVALID_ID;
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
		return VA_INVALID_ID;
	}
}

bool FVAPlugin::SetSoundSourceDirectivity(const int SoundSourceID, const int DirectivityID)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == VA_INVALID_ID)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourceDirectivity()]: SoundSourceID invalid (= -1)", true);
		return false;
	}

	if (DirectivityID == VA_INVALID_ID)
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


bool FVAPlugin::RemoveSoundSourceDirectivity(const int SoundSourceID)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundSourceID == VA_INVALID_ID)
	{
		FVAUtils::LogStuff("[FVAPlugin::SetSoundSourceDirectivity()]: SoundSourceID invalid (= -1)", true);
		return false;
	}

	try
	{
		VAServer->SetSoundSourceDirectivity(SoundSourceID, VA_INVALID_ID);
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
		return VA_INVALID_ID;
	}

	try
	{
		const std::string DirHRIR(TCHAR_TO_UTF8(*FileName));
		return VAServer->CreateDirectivityFromFile(DirHRIR);
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::CreateNewHRIR()", FString(e.ToString().c_str()) + " (" + FileName + ")");
		return VA_INVALID_ID;
	}
}


bool FVAPlugin::SetSoundReceiverHRIR(const int SoundReceiverID, const int HRIRID)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (HRIRID == VA_INVALID_ID)
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
		ProcessException("FVAPluginModule::SetSoundReceiverDirectivity()", FString(e.ToString().c_str()));
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
		return VA_INVALID_ID;
	}

	WorldScale = ReceiverActor->GetScale();

	try
	{
		const int SoundReceiverID = VAServer->CreateSoundReceiver("VASoundReceiver");
		const int HRIR = FVAHRIRManager::GetDefaultHRIR()->GetID();

		if (HRIR != VA_INVALID_ID)
		{
			VAServer->SetSoundReceiverDirectivity(SoundReceiverID, HRIR);
			FVAUtils::LogStuff("[FVAPlugin::CreateNewSoundReceiver()]: Default Directivity is not initialized, using no Directivity", false);
		}
		
		return SoundReceiverID;
	}
	catch (CVAException& e)
	{
		ProcessException("FVAPluginModule::createNewSoundReceiver()", FString(e.ToString().c_str()));
		return VA_INVALID_ID;
	}
}


bool FVAPlugin::SetSoundReceiverPosition(const int SoundReceiverID, FVector Pos)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}

	if (SoundReceiverID == VA_INVALID_ID)
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

	if (SoundReceiverID == VA_INVALID_ID)
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
	
	if (SoundReceiverID == VA_INVALID_ID)
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
// ******* Renderer RElated ***************************************** //
// ****************************************************************** //

bool FVAPlugin::RendererEnabled(const std::string& RendererID)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	try
	{
		//TODO: This is a dirty work around !!!
		//		Add new Get Method in VA and exchange this
		const double Gain = VAServer->GetRenderingModuleGain(RendererID);
		return Gain != 0.0;
	}
	catch (CVAException& e)
	{
		ProcessException("RendererExists()", FString(e.ToString().c_str()));
	}
	return false;
}

int FVAPlugin::GetRendererMuted(const std::string& RendererID)
{
	if (!ShouldInteractWithServer())
	{
		return -1;
	}
	try
	{
		return (int)VAServer->GetRenderingModuleMuted(RendererID);
	}
	catch (CVAException& e)
	{
		ProcessException("GetRendererMuted()", FString(e.ToString().c_str()));
	}
	return -1;
}


bool FVAPlugin::SetRendererMute(const std::string& RendererID, const bool bMute)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	try
	{
		VAServer->SetRenderingModuleMuted(RendererID, bMute);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("SetRendererMute()", FString(e.ToString().c_str()));
	}

	return false;
}

bool FVAPlugin::SetRendererParameters(const std::string& RendererID, const CVAStruct& RendererParameterStruct)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	try
	{
		VAServer->SetRenderingModuleParameters(RendererID, RendererParameterStruct);
		return true;
	}
	catch (CVAException& e)
	{
		ProcessException("SetRendererParameters()", FString(e.ToString().c_str()));
	}
	return false;
}

bool FVAPlugin::SetGlobalAuralizationMode(const int AuraModeBitvector)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	try
	{
		VAServer->SetGlobalAuralizationMode(AuraModeBitvector);
		return true;
	}
	catch (CVAException & e)
	{
		ProcessException("SetGlobalAuralizationMode()", FString(e.ToString().c_str()));
	}
	return false;
}

bool FVAPlugin::SetRenderingModuleAuralizationMode(const std::string& RendererID, int AuraModeBitvector)
{
	if (!ShouldInteractWithServer())
	{
		return false;
	}
	try
	{
		VAServer->SetRenderingModuleAuralizationMode(RendererID, AuraModeBitvector);
		return true;
	}
	catch (CVAException & e)
	{
		ProcessException("SetRenderingModuleAuralizationMode()", FString(e.ToString().c_str()));
	}
	return false;
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
  if(ReceiverActor && ReceiverActor->GetWorld())
  {
    UGameplayStatics::GetAllActorsOfClass(ReceiverActor->GetWorld(), AActor::StaticClass(), ActorArray);
  }

	for (AActor* Actor : ActorArray)
	{
		TArray<UVAAbstractSourceComponent*> VAComponents;
		Actor->GetComponents(VAComponents);
		for (UVAAbstractSourceComponent* VAComponent : VAComponents)
		{
			VAComponent->SetVisibility(bDebugMode);
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

bool FVAPlugin::GetWasStarted()
{
	return bWasStarted;
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
	bIsMaster = UVirtualRealityUtilities::IsMaster();

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









