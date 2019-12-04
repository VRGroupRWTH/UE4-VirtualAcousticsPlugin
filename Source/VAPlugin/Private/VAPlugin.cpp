// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VAPlugin.h"

#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "Cluster/IDisplayClusterClusterManager.h"
#include "Game/IDisplayClusterGameManager.h"
#include "Input/IDisplayClusterInputManager.h"
#include "IDisplayCluster.h"

#include "Engine.h"



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

bool FVAPluginModule::isMasterAndUsed()
{
	return (IDisplayCluster::Get().GetClusterMgr()!= nullptr && IDisplayCluster::Get().GetClusterMgr()->IsMaster() && useVA);
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
	// http://www.virtualacoustics.org/overview.html // 
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	// setViewMode();

	VAVec3 vec;

	// ++ Get Paths of DLL Handles ++ //
	//FString LibraryPath;
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
#endif



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

}

void FVAPluginModule::askForSettings(FString host, int port, bool askForDebugMode)
{
	if (initialized == true) {
		return;
	}

	isMaster = IDisplayCluster::Get().GetClusterMgr() != nullptr && IDisplayCluster::Get().GetClusterMgr()->IsMaster();

	if (!isMaster) {
		return;
	}

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

	if (!isMasterAndUsed()) {
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
	if (!isMasterAndUsed()) {
		return false;
	}

	if (pVANet == nullptr) {
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


// ****************************************************************** // 
// ******* Sound Buffer ********************************************* //
// ****************************************************************** //

std::string FVAPluginModule::createNewBuffer(std::string soundFileName, bool loop, float soundOffset)
{
	std::string sSignalSourceID;

	try
	{
		sSignalSourceID = pVA->CreateSignalSourceBufferFromFile(soundFileName);
		// const std::string sSignalSourceID = "hallo"; // = pVA->CreateSignalSourceBufferFromFile(soundName); // DELETED HERE
		// pVA->SetSignalSourceBufferPlaybackAction(sSignalSourceID, VAUtils::EPlayActionToVAAction(action));
		pVA->SetSignalSourceBufferLooping(sSignalSourceID, loop);
		pVA->SetSignalSourceBufferPlaybackPosition(sSignalSourceID, soundOffset);

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

bool FVAPluginModule::setSoundBufferTime(std::string sBufferID, float time)
{
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


// ****************************************************************** // 
// ******* Sound Sources ******************************************** //
// ****************************************************************** //

int FVAPluginModule::createNewSoundSource(std::string bufferID, std::string name,
	FVector soundPos, FRotator soundRot, float gainFactor)
{
	soundPos = VAUtils::toVACoordinateSystem(soundPos);
	soundRot = VAUtils::toVACoordinateSystem(soundRot);

	FQuat fQuat = soundRot.Quaternion();
	VAUtils::fQuatToVAQuat(fQuat, *tmpQuat);

	VAUtils::fVecToVAVec3(soundPos, *tmpVec);
	VAUtils::scaleVAVec(*tmpVec, scale);

	try
	{
		int iSoundSourceID = pVA->CreateSoundSource(name);
		pVA->SetSoundSourcePose(iSoundSourceID, *tmpVec, *tmpQuat);

		float power = pVA->GetSoundSourceSoundPower(iSoundSourceID) * gainFactor;
		pVA->SetSoundSourceSoundPower(iSoundSourceID, power);

		pVA->SetSoundSourceSignalSource(iSoundSourceID, bufferID);

		return iSoundSourceID;

	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::createNewSoundSource()", FString(e.ToString().c_str()));
		return -1;
	}

}

bool FVAPluginModule::setSoundSourcePos(int soundSourceID, FVector pos)
{
	pos = VAUtils::toVACoordinateSystem(pos);
	
	VAUtils::fVecToVAVec3(pos, *tmpVec);
	VAUtils::scaleVAVec(*tmpVec, scale);

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


// ****************************************************************** // 
// ******* Directivities ******************************************** //
// ****************************************************************** //

int FVAPluginModule::createNewDirectivity(FString fileName)
{
	try
	{
		std::string dir(TCHAR_TO_UTF8(*fileName));
		return pVA->CreateDirectivityFromFile(dir);
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::createNewDirectivity()", FString(e.ToString().c_str()) + " (" + fileName + ")");
		// if (&e == nullptr)
		// {
		// }

		//processExeption("FVAPluginModule::createNewDirectivity()", FString(e.ToString().c_str()));
		return -1;
	}
}

bool FVAPluginModule::setSoundSourceDirectivity(int soundSourceID, int dirID)
{
	try
	{
		pVA->SetSoundSourceDirectivity(soundSourceID, dirID);
		return true;
	}
	catch (CVAException& e)
	{
		processExeption("FVAPluginModule::createNewDirectivity()", FString(e.ToString().c_str()));
		// if (&e == nullptr)
		// {
		// }

		return false;
	}
}


// ****************************************************************** // 
// ******* Sound Receiver ******************************************* //
// ****************************************************************** //

int FVAPluginModule::createNewSoundReceiver(AVAReceiverActor* actor)
{
	receiverActor = actor;

	if (!isMaster) {
		return -1;
	}

	if (pVA == nullptr) {
		return -1;
	}

	scale = receiverActor->getScale();

	try {

		int iSoundReceiverID = pVA->CreateSoundReceiver("VASoundReceiver");
		int iHRIR = VADirectivityManager::getDefaultReceiverDirectivity()->getID();
		pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);
		return iSoundReceiverID;
	}
	catch (CVAException& e) {
		processExeption("FVAPluginModule::createNewSoundReceiver()", FString(e.ToString().c_str()));
		return -1;
	}
}

bool FVAPluginModule::setSoundReceiverDirectivity(int soundReceiverID, int dirID)
{
	if (pVA == nullptr) {
		return false;
	}

	try {
		pVA->SetSoundReceiverDirectivity(soundReceiverID, dirID);
		return true;
	}
	catch (CVAException& e) {
		processExeption("FVAPluginModule::setSoundReceiverDirectivity()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPluginModule::setSoundReceiverPosition(int soundReceiverID, FVector pos)
{
	if (pVA == nullptr) {
		return false;
	}

	VAUtils::rotateFVec(pos);

	VAUtils::fVecToVAVec3(pos, *tmpVec);
	VAUtils::scaleVAVec(*tmpVec, scale);

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
	if (pVA == nullptr) {
		return false;
	}

	VAUtils::rotateFRotator(rot);

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
	if (pVA == nullptr) {
		return false;
	}

	VAUtils::rotateFVec(pos);
	VAUtils::rotateFRotator(rot);

	FQuat quat = rot.Quaternion();

	VAUtils::fVecToVAVec3(pos, *tmpVec);
	VAUtils::fQuatToVAQuat(quat, *tmpQuat);

	VAUtils::scaleVAVec(*tmpVec, scale);

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
	UVASourceComponent* tmp;
	for (AActor* actor : actorsA) {
		tmp = dynamic_cast<UVASourceComponent*> (actor->GetComponentByClass(UVASourceComponent::StaticClass()));
		if (tmp != nullptr) {
			tmp->setSoundSourceVisibility(debugMode);
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
