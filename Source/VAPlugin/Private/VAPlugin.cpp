// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VAPlugin.h"


#include "VAReflectionWall.h"

#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "Cluster/IDisplayClusterClusterManager.h"
#include "Containers/UnrealString.h"
#include "Game/IDisplayClusterGameManager.h"
#include "Input/IDisplayClusterInputManager.h"
#include "IDisplayCluster.h"

#include "Networking.h"
#include "Engine.h"

#if WITH_EDITOR
#include "Editor.h"
#endif


// #include "IDisplayCluster.h"
// #include "IDisplayClusterClusterManager.h"

// #pragma warning(disable:5038) //disable initilization order warning
#include "VA.h"
#include "VANet.h"
// #pragma warning(default:5038)

#include "VAReceiverActor.h"
#include "VASourceComponent.h"
#include "VistaBase/VistaTimeUtils.h"



#define LOCTEXT_NAMESPACE "FVAPluginModule"

// Library Handles for dll loading of VA Classes
void* FVAPluginModule::LibraryHandleNet;
void* FVAPluginModule::LibraryHandleBase;
void* FVAPluginModule::LibraryHandleVistaAspects;
void* FVAPluginModule::LibraryHandleVistaBase;
void* FVAPluginModule::LibraryHandleVistaInterProcComm;

// Vars for setting of usage
bool FVAPluginModule::initialized	= false;
bool FVAPluginModule::useVA			= true;
bool FVAPluginModule::debugMode		= true;
bool FVAPluginModule::isMaster		= false;

// Interface Classes to Server 
IVANetClient* FVAPluginModule::pVANet;
IVAInterface* FVAPluginModule::pVA;

// Link to the current receiver actor 
AVAReceiverActor* FVAPluginModule::receiverActor;

// Scale of the UE4 world (how many units is 1m in "real life")
float FVAPluginModule::scale = 100.0f;

// tmp Var for easier usage
VAQuat* FVAPluginModule::tmpQuat = new VAQuat();
VAVec3* FVAPluginModule::tmpVec = new VAVec3();

FSocket* FVAPluginModule::VAServerLauncherSocket = nullptr;


// ****************************************************************** // 
// ******* REMAKE!!!!!! ********************************************* //
// ****************************************************************** //

void FVAPluginModule::processExeption(FString location, CVAException e)
{
	processExeption(location, FString(e.ToString().c_str()));
}

void FVAPluginModule::processExeption(FString location, FString exp)
{
	FString output = " in [";
	output.Append(location).Append("] with error: ").Append(exp);
	VAUtils::logStuff(output, true);
	VAUtils::openMessageBox(output, true);
}

bool FVAPluginModule::isInDebugMode()
{
	return debugMode;
}
 


// ****************************************************************** // 
// ******* Initialization Functions ********************************* //
// ****************************************************************** //

void FVAPluginModule::StartupModule()
{
	VAVec3 vec;

	// ++ Get Paths of DLL Handles ++ //
	FString BaseDir = IPluginManager::Get().FindPlugin("VAPlugin")->GetBaseDir();
	FString pathNet, pathBase, pathVistaAspects, pathVistaBase, pathVistaInterProcComm;

#if PLATFORM_WINDOWS
	//LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/VAPlugin/x64/Release/VABase.dll"));
	pathBase				=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VABase.dll"));
	pathVistaAspects		=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaAspects.dll"));
	pathVistaBase			=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaBase.dll"));
	pathVistaInterProcComm	=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaInterProcComm.dll"));
	pathNet					=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VANet.dll"));
	
	
	// ++ Load DLL Handles ++ // 
	//ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	LibraryHandleBase				= !pathBase.IsEmpty()				? FPlatformProcess::GetDllHandle(*pathBase)					: nullptr;
	LibraryHandleVistaBase			= !pathVistaBase.IsEmpty()			? FPlatformProcess::GetDllHandle(*pathVistaBase)			: nullptr;
	LibraryHandleVistaAspects		= !pathVistaAspects.IsEmpty()		? FPlatformProcess::GetDllHandle(*pathVistaAspects)			: nullptr;
	LibraryHandleVistaInterProcComm = !pathVistaInterProcComm.IsEmpty() ? FPlatformProcess::GetDllHandle(*pathVistaInterProcComm)	: nullptr;
	LibraryHandleNet				= !pathNet.IsEmpty()				? FPlatformProcess::GetDllHandle(*pathNet)					: nullptr;

	// ++ Check Handles ++ //
	if (!checkLibraryHandles())
	{
		VAUtils::openMessageBox("stop initialzing because of incomplete libraryHandles");
		return;
	}

#elif PLATFORM_LINUX
	//LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/VAPlugin/x64/Release/VABase.dll"));
	pathBase				=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VABase.so"));
	pathVistaAspects		=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaAspects.so"));
	pathVistaBase			=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaBase.so"));
	pathVistaInterProcComm	=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaInterProcComm.so"));
	pathNet					=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VANet.so"));
#endif // PLATFORM_LINUX
	
	initialized = false;
	useVA = true;
	debugMode = true;
	isMaster = false;
#if WITH_EDITOR
	FEditorDelegates::BeginPIE.AddRaw(this, &FVAPluginModule::BeginSession);
	FEditorDelegates::EndPIE.AddRaw(this, &FVAPluginModule::EndSession);
#endif
}

void FVAPluginModule::BeginSession(const bool something)
{
	initialized = false;
	useVA = true;
	debugMode = true;
	isMaster = false;
}

void FVAPluginModule::EndSession(const bool something)
{
	if (VAServerLauncherSocket != nullptr) {
		VAServerLauncherSocket->Close();
		VAServerLauncherSocket = nullptr;
	}
}

void FVAPluginModule::ShutdownModule()
{
	disconnectServer();

#if PLATFORM_WINDOWS
	FPlatformProcess::FreeDllHandle(LibraryHandleNet);
	FPlatformProcess::FreeDllHandle(LibraryHandleBase);
    FPlatformProcess::FreeDllHandle(LibraryHandleVistaAspects);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaBase);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaInterProcComm);
#endif // PLATFORM_WINDOWS

	if (VAServerLauncherSocket != nullptr) {
		VAServerLauncherSocket->Close();
		VAServerLauncherSocket = nullptr;
	}

}

void FVAPluginModule::askForSettings(FString host, int port, bool askForDebugMode, bool askForUseVA)
{
	if (initialized == true) {
		return;
	}

	isMaster = IDisplayCluster::Get().GetClusterMgr() != nullptr && IDisplayCluster::Get().GetClusterMgr()->IsMaster();

	if (!isMaster) {
		return;
	}

	if (askForUseVA) {
		EAppReturnType::Type ret = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString("Use VA Server (" + host + ":" + FString::FromInt(port) + ")? If yes, make sure to have it switched on."));
		if (ret == EAppReturnType::Type::Yes) {
			useVA = true;
		}
		else {
			useVA = false;
			debugMode = false;
			initialized = true;
			return;
		}
	}
	

	if (askForDebugMode) {
		EAppReturnType::Type retD = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString("Start in Debug mode?"));
		if (retD == EAppReturnType::Type::Yes) {
			debugMode = true;
		}
		else {
			debugMode = false;
		}
	}
	else {
		debugMode = false;
	}


	initialized = true;
}

bool FVAPluginModule::checkLibraryHandles()
{
	if (LibraryHandleNet && LibraryHandleBase &&
		LibraryHandleVistaAspects && LibraryHandleVistaBase && LibraryHandleVistaInterProcComm) {
		return true;
	}
	else {
		if (!LibraryHandleNet) {
			VAUtils::logStuff("could not load Net", true);
		}

		if (!LibraryHandleBase) {
			VAUtils::logStuff("could not load Base", true);
		}

		if (!LibraryHandleVistaAspects) {
			VAUtils::logStuff("could not load Vista Aspects", true);
		}

		if (!LibraryHandleVistaBase) {
			VAUtils::logStuff("could not load Vista Base", true);
		}

		if (!LibraryHandleVistaInterProcComm) {
			VAUtils::logStuff("could not load Vista InterProcComm", true);
		}
		return false;
	}

}


// ****************************************************************** // 
// ******* General Server Functions ********************************* //
// ****************************************************************** //

bool FVAPluginModule::connectServer(FString hostF, int port)
{

	if (!isMaster || !useVA) {
		return false;
	}

	if (isConnected()) {
		resetServer();
		return true;
	}

	VAUtils::logStuff("Connecting to VAServer. Be sure to have it switched on");

	try {
		pVANet = IVANetClient::Create();

		std::string host(TCHAR_TO_UTF8(*hostF));
		pVANet->Initialize(host, port);
		if (!pVANet->IsConnected()) {
			VAUtils::openMessageBox("Could not connect to VA Server", true);
			useVA = false;
			return false;
		}

		pVA = pVANet->GetCoreInstance();
		pVA->Reset();
	}
	catch (CVAException& e) {
		useVA = false;
		processExeption("FVAPluginModule::connectServer()", FString(e.ToString().c_str()));

		return false;
	}
	
	return true;
}

bool FVAPluginModule::resetServer()
{
	if (!getIsMaster() || !isConnected() || !getUseVA()) {
		return false;
	}

	try {
		VAUtils::logStuff("Resetting Server now...");
		pVA->Reset();
	}
	catch (CVAException& e) {
		useVA = false;
		processExeption("FVAPluginModule::resetServer()", FString(UTF8_TO_TCHAR(e.ToString().c_str())));

		return false;
	}

	return true;
}

bool FVAPluginModule::isConnected()
{
	if (!isMaster || !useVA) {
		return false;
	}

	if (pVANet == nullptr || pVA == nullptr) {
		return false;
	}

	try {
		return pVANet->IsConnected();
	}
	catch (CVAException& e) {
		useVA = false;
		processExeption("FVAPluginModule::isConnected() ", FString(e.ToString().c_str()));
		return false;
	}

}

bool FVAPluginModule::disconnectServer()
{
	if (!isConnected()) {
		return true;
	}

	if (VAServerLauncherSocket != nullptr) {
		VAServerLauncherSocket->Close();
		VAServerLauncherSocket = nullptr;
	}
	
	return true;
	VAUtils::logStuff("[FVAPluginModule::disconnectServer()] Disconnecting now");
	pVA->Finalize();

	if (pVANet != nullptr) {
		if (pVANet->IsConnected()) {
			pVANet->Disconnect();
		}
	}
	return true;
}

bool FVAPluginModule::remoteStartVAServer(const FString& Host, const int Port, const FString& VersionName)
{
	isMaster = IDisplayCluster::Get().GetClusterMgr() != nullptr && IDisplayCluster::Get().GetClusterMgr()->IsMaster();

	if (!isMaster)
		return false;

	if(VAServerLauncherSocket!=nullptr)
	{
		return true;
	}
	
	VAUtils::logStuff("Try to remotely start the VAServer at address "+Host+":"+FString::FromInt(Port)+" for version: "+VersionName);


	//Connect
	const FString SocketName(TEXT("VAServerStarterConnection"));
	VAServerLauncherSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, SocketName, false);

	TSharedPtr<FInternetAddr> InternetAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool ValidIP;
	InternetAddress->SetIp(*Host, ValidIP);
	InternetAddress->SetPort(Port);
	if (!ValidIP) {
		VAUtils::logStuff("Error: The Ip cannot be parsed!");
		return false;
	}

	if(VAServerLauncherSocket == nullptr || !VAServerLauncherSocket->Connect(*InternetAddress))
	{
		VAUtils::logStuff("Error: Cannot connect to Launcher!");
		return false;
	}
	VAUtils::logStuff("Successfully connected to Launcher");

	//Send requested version
	TArray<uint8> RequestData;
	for (TCHAR character : VersionName.GetCharArray())
	{
		uint8 byte = static_cast<uint8>(character);
		if (byte != 0) {
			RequestData.Add(static_cast<uint8>(character));
		}
	}
	int BytesSend = 0;
	VAServerLauncherSocket->Send(RequestData.GetData(), RequestData.Num(), BytesSend);
	VAUtils::logStuff("Send "+FString::FromInt(BytesSend)+" bytes to the VAServer Launcher, with version name: "+ VersionName +" Waiting for answer....");

	//Receive response
	const int32 BufferSize = 16;
	int32 BytesRead = 0;
	uint8 Response[16];
	if (VAServerLauncherSocket->Recv(Response, BufferSize, BytesRead) && BytesRead==1)
	{
		switch (Response[0]) {
		case 'g':
			VAUtils::logStuff("Received go from launcher, VAServer seems to be correctly started.");
			break;
		case 'n':
			VAUtils::openMessageBox("VAServer cannot be launched, invalid VAServer binary file or cannot be found", true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'i':
			VAUtils::openMessageBox("VAServer cannot be launched, invalid file entry in the config", true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'a':
			VAUtils::openMessageBox("VAServer was aborted", true);
			VAServerLauncherSocket = nullptr;
			return false;
		case 'f':
			VAUtils::openMessageBox("VAServer cannot be launched, requested version \""+VersionName+"\" is not available/specified", true);
			VAServerLauncherSocket = nullptr;
			return false;
		default:
			VAUtils::openMessageBox("Unexpected response from VAServer Launcher: " + FString(reinterpret_cast<char*>(&Response[0])), true);
			VAServerLauncherSocket = nullptr;
			return false;
		}
	}
	else
	{
		VAUtils::logStuff("Error while receiving response from VAServer Launcher");
		VAServerLauncherSocket = nullptr;
		return false;
	}

	return true;
}


// ****************************************************************** // 
// ******* Sound Buffer ********************************************* //
// ****************************************************************** //

std::string FVAPluginModule::createNewBuffer(FString soundFileName, bool loop, float soundOffset)
{
	if (!isConnected()) {
		return "";
	}


	std::string sSignalSourceID;

	std::string fileName(TCHAR_TO_UTF8(*soundFileName));

	try
	{
		sSignalSourceID = pVA->CreateSignalSourceBufferFromFile(fileName);
		// const std::string sSignalSourceID = "hallo"; // = pVA->CreateSignalSourceBufferFromFile(soundFile); // DELETED HERE
		pVA->SetSignalSourceBufferLooping(sSignalSourceID, loop);
		pVA->SetSignalSourceBufferPlaybackPosition(sSignalSourceID, soundOffset);
		pVA->SetSignalSourceBufferPlaybackAction(sSignalSourceID, 0);

		return sSignalSourceID;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::createNewBuffer()", FString(e.ToString().c_str()));
		return "";
	}
}

bool FVAPluginModule::setSoundBufferAction(std::string sBufferID, EPlayAction action)
{
	if (!isConnected()) {
		return false;
	}

	try
	{
		pVA->SetSignalSourceBufferPlaybackAction(sBufferID, VAUtils::EPlayActionToVAAction(action));

		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setSoundSourceAction()", FString(e.ToString().c_str()));
		return false;
	}
}

int FVAPluginModule::getSoundBufferAction(std::string sBufferID) 
{
	if (!isConnected()) {
		return -2;
	}

	try
	{
		int state = pVA->GetSignalSourceBufferPlaybackState(sBufferID);	
		return state;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::getSoundBufferAction()", FString(e.ToString().c_str()));
		return -2;
	}
}

bool FVAPluginModule::setSoundBufferTime(std::string sBufferID, float time)
{
	if (!isConnected()) {
		return false;
	}

	try
	{
		pVA->SetSignalSourceBufferPlaybackPosition(sBufferID, time);

		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setSoundBufferTime()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPluginModule::setSoundBufferLoop(std::string sBufferID, bool loop)
{
	if (!isConnected()) {
		return false;
	}

	try
	{
		pVA->SetSignalSourceBufferLooping(sBufferID, loop);

		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setSoundBufferLoop()", FString(e.ToString().c_str()));
		return false;
	}
}


// ****************************************************************** // 
// ******* Sound Sources ******************************************** //
// ****************************************************************** //

int FVAPluginModule::createNewSoundSource(std::string bufferID, std::string name,
	FVector soundPos, FRotator soundRot, float gainFactor)
{
	if (!isConnected()) {
		return -1;
	}

	soundPos = VAUtils::toVACoordinateSystem(soundPos, scale);
	VAUtils::fVecToVAVec3(soundPos, *tmpVec);

	soundRot = VAUtils::toVACoordinateSystem(soundRot);
	FQuat fQuat = soundRot.Quaternion();
	VAUtils::fQuatToVAQuat(fQuat, *tmpQuat);


	try
	{
		int soundSourceID = pVA->CreateSoundSource(name);
		pVA->SetSoundSourcePose(soundSourceID, *tmpVec, *tmpQuat);

		float power = pVA->GetSoundSourceSoundPower(soundSourceID) * gainFactor;
		pVA->SetSoundSourceSoundPower(soundSourceID, power);

		pVA->SetSoundSourceSignalSource(soundSourceID, bufferID);

		// pVA->SetSoundSourceDirectivity(soundSourceID, VADirectivityManager::getDefaultDirectivity()->getID());

		return soundSourceID;

	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::createNewSoundSource()", FString(e.ToString().c_str()));
		return -1;
	}

}

bool FVAPluginModule::setSoundSourcePos(int soundSourceID, FVector pos)
{
	if (!isConnected()) {
		return false;
	}
	
	VAUtils::logStuff(FString("SoundSou: X: " + FString::SanitizeFloat(pos.X) + " - Y: " + FString::SanitizeFloat(pos.Y) + " - Z: " + FString::SanitizeFloat(pos.Z)));

	pos = VAUtils::toVACoordinateSystem(pos, scale);
	VAUtils::fVecToVAVec3(pos, *tmpVec);

	
	
	try
	{
		pVA->SetSoundSourcePosition(soundSourceID, *tmpVec);

		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setSoundSourcePos()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPluginModule::setSoundSourceRot(int soundSourceID, FRotator rot)
{
	if (!isConnected()) {
		return false;
	}
	
	VAUtils::logStuff(FString("Sou Angl: R: " + FString::SanitizeFloat(rot.Roll) + " - P: " + FString::SanitizeFloat(rot.Pitch) + " - Y: " + FString::SanitizeFloat(rot.Yaw)));

	rot = VAUtils::toVACoordinateSystem(rot);
	FQuat fQuat = rot.Quaternion();
	VAUtils::fQuatToVAQuat(fQuat, *tmpQuat);

	try
	{
		pVA->SetSoundSourceOrientation(soundSourceID, *tmpQuat);
		// pVA->SetSoundSourcePose(iSoundSourceID, *tmpVec, *tmpQuat);
		
		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setSoundSourceRot()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPluginModule::setNewBufferForSoundSource(int soundSourceID, std::string bufferID)
{
	if (!isConnected()) {
		return false;
	}

	try
	{
		pVA->SetSoundSourceSignalSource(soundSourceID, bufferID);
		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setNewBufferForSoundSource()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPluginModule::setSoundSourceMuted(int soundSourceID, bool muted)
{
	if (!isConnected()) {
		return false;
	}

	try
	{
		pVA->SetSoundSourceMuted(soundSourceID, muted);
		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setSoundSourceMuted()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPluginModule::setSoundSourcePower(int soundSourceID, float power)
{
	if (!isConnected()) {
		return false;
	}

	try
	{
		pVA->SetSoundSourceSoundPower(soundSourceID, power);
		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setSoundSourcePower()", FString(e.ToString().c_str()));
		return false;
	}
}



// ****************************************************************** // 
// ******* Directivities ******************************************** //
// ****************************************************************** //

int FVAPluginModule::createNewDirectivity(FString fileName)
{
	if (!isConnected()) {
		return -1;
	}

	try
	{
		std::string dir(TCHAR_TO_UTF8(*fileName));
		return pVA->CreateDirectivityFromFile(dir);
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::createNewDirectivity()", FString(e.ToString().c_str()) + " (" + fileName + ")");
		return -1;
	}
}

bool FVAPluginModule::setSoundSourceDirectivity(int soundSourceID, int dirID)
{
	if (!isConnected()) {
		return false;
	}

	try
	{
		pVA->SetSoundSourceDirectivity(soundSourceID, dirID);
		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::setSoundSourceDirectivity()", FString(e.ToString().c_str()));

		return false;
	}
}

// ****************************************************************** // 
// ******* HRIR ***************************************************** //
// ****************************************************************** //

int FVAPluginModule::createNewHRIR(FString fileName)
{
	if (!isConnected()) {
		return -1;
	}

	try
	{
		std::string dir(TCHAR_TO_UTF8(*fileName));
		return pVA->CreateDirectivityFromFile(dir);
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::createNewHRIR()", FString(e.ToString().c_str()) + " (" + fileName + ")");
		return -1;
	}
}


bool FVAPluginModule::setSoundReceiverHRIR(int soundReceiverID, int hrirID)
{
	if (!isConnected()) {
		return false;
	}

	try {
		pVA->SetSoundReceiverDirectivity(soundReceiverID, hrirID);
		return true;
	}
	catch (CVAException& e) {
		processExeption("FVAPluginModule::setSoundReceiverDirectivity()", FString(e.ToString().c_str()));
		return false;
	}
}

// ****************************************************************** // 
// ******* Sound Receiver ******************************************* //
// ****************************************************************** //

int FVAPluginModule::createNewSoundReceiver(AVAReceiverActor* actor)
{
	receiverActor = actor;

	if (!isConnected() || !isMaster) {
		return -1;
	}

	scale = receiverActor->getScale();

	try {

		int iSoundReceiverID = pVA->CreateSoundReceiver("VASoundReceiver");
		int iHRIR = VAHRIRManager::getDefaultHRIR()->getID();
		pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);
		return iSoundReceiverID;
	}
	catch (CVAException& e) {
		processExeption("FVAPluginModule::createNewSoundReceiver()", FString(e.ToString().c_str()));
		return -1;
	}
}


bool FVAPluginModule::setSoundReceiverPosition(int soundReceiverID, FVector pos)
{
	if (!isConnected()) {
		return false;
	}
	
	VAUtils::logStuff(FString("Receiver: X: " + FString::SanitizeFloat(pos.X) + " - Y: " + FString::SanitizeFloat(pos.Y) + " - Z: " + FString::SanitizeFloat(pos.Z)));

	pos = VAUtils::toVACoordinateSystem(pos, scale);
	VAUtils::fVecToVAVec3(pos, *tmpVec);

	

	try {
		pVA->SetSoundReceiverPosition(soundReceiverID, *tmpVec);
		return true;
	}
	catch (CVAException& e) {
		processExeption("FVAPluginModule::setSoundReceiverPosition()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPluginModule::setSoundReceiverRotation(int soundReceiverID, FRotator rot)
{
	if (!isConnected()) {
		return false;
	}

	VAUtils::logStuff(FString("Rec Angl: R: " + FString::SanitizeFloat(rot.Roll) + " - P: " + FString::SanitizeFloat(rot.Pitch) + " - Y: " + FString::SanitizeFloat(rot.Yaw)));

	rot = VAUtils::toVACoordinateSystem(rot);
	FQuat quat = rot.Quaternion();
	VAUtils::fQuatToVAQuat(quat, *tmpQuat);

	try {
		pVA->SetSoundReceiverOrientation(soundReceiverID, *tmpQuat);
		return true;
	}
	catch (CVAException& e) {
		processExeption("FVAPluginModule::setSoundReceiverRotation()", FString(e.ToString().c_str()));
		return false;
	}
}


// ****************************************************************** // 
// ******* Real World *********************************************** //
// ****************************************************************** //

bool FVAPluginModule::setSoundReceiverRealWorldPose(int soundReceiverID, FVector pos, FRotator rot)
{
	if (!isConnected()) {
		return false;
	}
	
	VAUtils::logStuff(FString("RL World: X: " + FString::SanitizeFloat(pos.X) + " - Y: " + FString::SanitizeFloat(pos.Y) + " - Z: " + FString::SanitizeFloat(pos.Z)));

	pos = VAUtils::toVACoordinateSystem(pos, scale);
	VAUtils::fVecToVAVec3(pos, *tmpVec);
	
	rot = VAUtils::toVACoordinateSystem(rot);
	FQuat quat = rot.Quaternion();
	VAUtils::fQuatToVAQuat(quat, *tmpQuat);


	
	try {
		pVA->SetSoundReceiverRealWorldPose(soundReceiverID, *tmpVec, *tmpQuat);
		return true;
	}
	catch (CVAException& e) {
		processExeption("setSoundReceiverRealWorldPose()", FString(e.ToString().c_str()));
		return false;
	}
}


// ****************************************************************** // 
// ******* General Setter Functions ********************************* //
// ****************************************************************** //

void FVAPluginModule::setReceiverActor(AVAReceiverActor* actor) {
	receiverActor = actor;
}

void FVAPluginModule::setScale(float scale_) {
	scale = scale_;
}

void FVAPluginModule::setUseVA(bool useVA_)
{
	useVA = useVA_;
}

void FVAPluginModule::setDebugMode(bool debugMode_)
{
	debugMode = debugMode_;

	TArray<AActor*> actorsA;
	UGameplayStatics::GetAllActorsOfClass(receiverActor->GetWorld(), AActor::StaticClass(), actorsA);

	for (AActor* actor : actorsA) {
		TArray<UActorComponent*> VA_components = actor->GetComponentsByClass(UVASourceComponent::StaticClass());
		for (UActorComponent* VA_component : VA_components) {
			Cast<UVASourceComponent>(VA_component)->setSoundSourceVisibility(debugMode);
		}
	}
}

// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //


bool FVAPluginModule::getIsInitialized()
{
	return initialized;
}

bool FVAPluginModule::getUseVA()
{
	if (!initialized) {
		askForSettings();
	}

	return useVA;
}

bool FVAPluginModule::getDebugMode()
{
	if (!initialized) {
		askForSettings();
	}

	return debugMode;
}

bool FVAPluginModule::getIsMaster()
{
	if (!initialized) {
		askForSettings();
	}

	return isMaster;
}

AVAReceiverActor* FVAPluginModule::getReceiverActor() {
	return receiverActor;
}

TArray<AVAReflectionWall*> FVAPluginModule::getReflectionWalls() {
	return receiverActor->getReflectionWalls();
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVAPluginModule, VAPlugin)
