// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VAPlugin.h"

#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "Cluster/IDisplayClusterClusterManager.h"
#include "Game/IDisplayClusterGameManager.h"
#include "Input/IDisplayClusterInputManager.h"
#include "IDisplayCluster.h"


// #include "IDisplayCluster.h"
// #include "IDisplayClusterClusterManager.h"


#include "VAReceiverActor.h"
#include "VASourceComponent.h"
//#include "VA.h"
//#include "VANet.h"
#include "VistaBase/VistaTimeUtils.h"




#define LOCTEXT_NAMESPACE "FVAPluginModule"

IVANetClient* FVAPluginModule::pVANet;
IVAInterface* FVAPluginModule::pVA;
AVAReceiverActor* FVAPluginModule::receiverActor;
TArray<UVASourceComponent*> FVAPluginModule::soundComponents;
TArray<UVASourceComponent*> FVAPluginModule::uninitializedSoundComponents;
TMap<int, std::string> FVAPluginModule::soundComponentsIDs;
TMap<int, TArray<int>> FVAPluginModule::soundComponentsReflectionIDs;
TMap<int, AVAReflectionWall*> FVAPluginModule::matchingReflectionWalls;
TMap<FString, int> FVAPluginModule::dirMap;
int FVAPluginModule::defaultDirID;
TArray<AVAReflectionWall*> FVAPluginModule::reflectionWalls;

bool FVAPluginModule::useVA;
bool FVAPluginModule::debugMode = true; // TODO: Change here


void* FVAPluginModule::LibraryHandleNet;
void* FVAPluginModule::LibraryHandleBase;
void* FVAPluginModule::LibraryHandleVistaAspects;
void* FVAPluginModule::LibraryHandleVistaBase;
void* FVAPluginModule::LibraryHandleVistaInterProcComm;

int FVAPluginModule::iSoundReceiverID;
int FVAPluginModule::iSoundSourceID;
int FVAPluginModule::iHRIR;
float FVAPluginModule::scale = 1.0f;
VAUtils::viewEnum FVAPluginModule::viewMode = VAUtils::viewEnum::unclear;
std::string FVAPluginModule::directivity;

VAQuat* FVAPluginModule::tmpQuat = new VAQuat();
VAVec3* FVAPluginModule::tmpVec = new VAVec3();

TArray<FString> FVAPluginModule::listOfPhonemes;
TArray<FString> FVAPluginModule::listOfPairs;
TArray<FString> FVAPluginModule::listOfDirectivities;
TMap<FString, FString> FVAPluginModule::dirMapping;
TMap<FString, int> FVAPluginModule::dirMappingToInt;


void FVAPluginModule::StartupModule()
{
	// http://www.virtualacoustics.org/overview.html // 
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	setViewMode();

	VAVec3 vec;

	// ++ Get Paths of DLL Handles ++ //
	//FString LibraryPath;
	FString BaseDir = IPluginManager::Get().FindPlugin("VAPlugin")->GetBaseDir();
	FString pathNet, pathBase, pathVistaAspects, pathVistaBase, pathVistaInterProcComm;

	useVA = true;

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
	if (!checkLibraryHandles(true))
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

	
	//runServerTest();
	
	//delete pVANet;

}

void FVAPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (!isMasterAndUsed()) {
		return;
	}
	
	if (pVANet != nullptr) {
		if (pVANet->IsConnected()) {
			pVANet->Disconnect();
		}
	}

#if PLATFORM_WINDOWS
	FPlatformProcess::FreeDllHandle(LibraryHandleNet);
	FPlatformProcess::FreeDllHandle(LibraryHandleBase);
    FPlatformProcess::FreeDllHandle(LibraryHandleVistaAspects);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaBase);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaInterProcComm);
#endif // PLATFORM_WINDOWS

}

bool FVAPluginModule::connectServer(FString hostF, int port)
{

	if (!isMasterAndUsed()) {
		return false;
	}

	if (isConnected()) {
		return true;
	}

	VAUtils::logStuff("Connecting to VAServer. Be sure to have it switched on");

	try {
		pVANet = IVANetClient::Create();

		std::string host(TCHAR_TO_UTF8(*hostF));
		pVANet->Initialize(host, port);
		if (!pVANet->IsConnected()) {
			VAUtils::openMessageBox("Could not connect to VA Server", true);
			return false;
		}

		pVA = pVANet->GetCoreInstance();
		pVA->Reset();
	}
	catch (CVAException& e) {
		processExeption("connectServer()", FString(e.ToString().c_str()));
		return false;
	}
	
	return true;
}

bool FVAPluginModule::resetServer()
{
	if (!isMasterAndUsed()) {
		return false;
	}

	if (!isConnected()) {
		return false;
	}

	pVA->Reset();

	return true;
}

bool FVAPluginModule::initializeServer(FString host, int port)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	EAppReturnType::Type ret = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString("Use VA Server? If yes, make sure to have it switched on."));

	EAppReturnType::Type retD = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString("Start in Debug mode?"));
	if (ret == EAppReturnType::Type::Yes) {
		debugMode = true;
	}
	else {
		debugMode = false;
	}

	if (ret == EAppReturnType::Type::No) {
		useVA = false;
		return false;
	}

	if (!connectServer(host, port)) {
		useVA = false;
		return false;
	}
	
	return true;
}

void FVAPluginModule::initializeWalls(TArray<AVAReflectionWall*> walls)
{

	reflectionWalls = walls;

	//GetAllActorsOfClass(AActor::GetWorld());
	//FindAllActors(AActor::GetWorld(), reflectionWalls);

	// TArray<AActor*> FoundActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), YourClass::StaticClass(), FoundActors);
}

bool FVAPluginModule::initializeReceiver(AVAReceiverActor* actor)
{
	receiverActor = actor;
	scale = receiverActor->getScale();
	
	
	if (!isMasterAndUsed()) {
		return false;
	}


	//receiverActor->getStartingPosition();

	// tmpQuat->Set(0.0f, 0.0f, 0.0f, 1.0f);
	// tmpVec->Set(0.0f, 1.7f, 0.0f);

	iSoundReceiverID = pVA->CreateSoundReceiver("VASoundReceiver");
	updateReceiverPos(FVector(0,1.7,0), FQuat(0,0,0,0));

	try {
		std::string dir = receiverActor->getDirectivity(); // DELETED HERE
		iHRIR = pVA->CreateDirectivityFromFile(dir); // DELETED HERE
	}
	catch (CVAException& e) {
		processExeption("initializeReceiver", FString(e.ToString().c_str()));
		return false;
	}
   
	

	pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);

	pVA->SetOutputGain(actor->getGainFactor());

	return true;
}

/*
bool FVAPluginModule::ini()
{
	connectServer();

	const std::string sSignalSourceID = pVA->CreateSignalSourceBufferFromFile("$(DemoSound)");
	pVA->SetSignalSourceBufferPlaybackAction(sSignalSourceID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
	pVA->SetSignalSourceBufferLooping(sSignalSourceID, true);

	iSoundSourceID = pVA->CreateSoundSource("C++ example sound source");
	tmpQuat = new VAQuat(0.0f, 0.0f, 0.0f, 1.0f);
	tmpVec = new VAVec3(-2.0f, 1.7f, -2.0f);
	pVA->SetSoundSourcePose(iSoundSourceID, *tmpVec, *tmpQuat);

	pVA->SetSoundSourceSignalSource(iSoundSourceID, sSignalSourceID);

	iHRIR = pVA->CreateDirectivityFromFile("$(DefaultHRIR)");

	iSoundReceiverID = pVA->CreateSoundReceiver("ReceiverActor");
	tmpQuat = new VAQuat(0.0f, 0.0f, 0.0f, 1.0f);
	tmpVec = new VAVec3(0.0f, 1.7f, 0.0f);
	pVA->SetSoundReceiverPose(iSoundReceiverID, *tmpVec, *tmpQuat);
	pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);


	//pVA->SetSoundReceiverPose(iSoundReceiverID, *RecVec3, *RecQuat);
	//pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);

	return true;
}
 */

/*
void FVAPluginModule::playTestSound(bool loop)
{
	VAUtils::openMessageBox("in playTestAudio", true);

	const std::string sSignalSourceID = pVA->CreateSignalSourceBufferFromFile("Audiofiles/Bauer.wav");
	pVA->SetSignalSourceBufferPlaybackAction(sSignalSourceID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
	pVA->SetSignalSourceBufferLooping(sSignalSourceID, loop);

	VAQuat* SourceQuat = new VAQuat(0.0f, 0.0f, 0.0f, 1.0f);
	VAVec3* SourceVec3 = new VAVec3(-2.0f, 1.7f, -2.0f);
	iSoundSourceID = pVA->CreateSoundSource("C++ example sound source");
	//pVA->SetSoundSourcePose(iSoundSourceID, VAVec3(-2.0f, 1.7f, -2.0f), VAQuat(0.0f, 0.0f, 0.0f, 1.0f));
	pVA->SetSoundSourcePose(iSoundSourceID, *SourceVec3, *SourceQuat);

	pVA->SetSoundSourceSignalSource(iSoundSourceID, sSignalSourceID);
}
 */

int FVAPluginModule::initializeSoundWithReflections(FString soundNameF, FVector soundPos, FRotator soundRot, float gainFactor, bool loop, float soundOffset, int action, UVASourceComponent* sourceComp)
{
	
	// first initialize real sound
	const int iSoundSourceID = initializeSound(soundNameF, soundPos, soundRot, gainFactor, loop, soundOffset, IVAInterface::VA_PLAYBACK_ACTION_STOP);
	
	TArray<int> reflectionArrayIDs;

	// now all reflections
	for (AVAReflectionWall* wall : reflectionWalls)
	{
		// get Reflection Factor R
		float R = wall->getR();
		if (R == 0.0f) {	// skip if there is no reflection
			continue;
		}
		
		//FTransform trans = computeReflectedTransform(wall, soundPos, soundRot);

		FVector pos_new = computeReflectedPos(wall, soundPos);
		FRotator rot_new = computeReflectedRot(wall, soundRot);

		// Set Name TODO:
		// FString soundNameF_new = soundNameF.Append("_ReflectedBy_").Append(wall->GetName());
		

		int id = initializeSound(soundNameF, pos_new, rot_new, gainFactor * R * R, loop, soundOffset, IVAInterface::VA_PLAYBACK_ACTION_STOP);

		matchingReflectionWalls.Add(id, wall);

		reflectionArrayIDs.Add(id);
		if(sourceComp != nullptr && isInDebugMode()){
			sourceComp->setReflectedSourceRepresentation(wall, pos_new, rot_new);
			// sourceComp->setReflectedSourceReprVisibility(wall, true);
		}
			// wall->spawnSphere(pos_new, rot_new); // TODO: delete

		FString text = "orig pos: ";
		text.Append(FString::FromInt(soundPos.X)).Append("/").Append(FString::FromInt(soundPos.Y)).Append("/").Append(FString::FromInt(soundPos.Z));
		text.Append(" // reflected pos: ");
		text.Append(FString::FromInt(pos_new.X)).Append("/").Append(FString::FromInt(pos_new.Y)).Append("/").Append(FString::FromInt(pos_new.Z));
		VAUtils::logStuff(text);

		text = "orig rot: ";
		text.Append(FString::FromInt(soundRot.Roll)).Append("/").Append(FString::FromInt(soundRot.Pitch)).Append("/").Append(FString::FromInt(soundRot.Yaw));
		text.Append(" // reflected pos: ");
		text.Append(FString::FromInt(rot_new.Roll)).Append("/").Append(FString::FromInt(rot_new.Pitch)).Append("/").Append(FString::FromInt(rot_new.Yaw));
		VAUtils::openMessageBox(text);
	}

	// Play all sounds together
	// if (action == IVAInterface::VA_PLAYBACK_ACTION_PLAY) {
	// 	setSoundAction(iSoundSourceID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
	// 	for (int i : reflectionArrayIDs) {
	// 		setSoundAction(i, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
	// 	}
	// }

	soundComponentsReflectionIDs.Add(iSoundSourceID, reflectionArrayIDs);

	return iSoundSourceID;
}

int  FVAPluginModule::initializeSound(FString soundNameF, FVector soundPos, FRotator soundRot, float gainFactor, bool loop, float soundOffset, int action, UVASourceComponent* sourceComp)
{
	soundPos = VAUtils::toVACoordinateSystem(soundPos);
	soundRot = VAUtils::toVACoordinateSystem(soundRot);

	FQuat fQuat = soundRot.Quaternion();
	VAUtils::fQuatToVAQuat(fQuat, *tmpQuat);
	
	VAUtils::fVecToVAVec3(soundPos, *tmpVec);
	VAUtils::scaleVAVec(*tmpVec, scale);

	std::string soundName = std::string(TCHAR_TO_UTF8(*soundNameF));

	if (!isMasterAndUsed()) {
		return -1;
	}

	int iSoundSourceID;

	std::string sSignalSourceID;
	try
	{
		sSignalSourceID = pVA->CreateSignalSourceBufferFromFile(soundName); // DELETED HERE // NAME HIER
		// const std::string sSignalSourceID = "hallo"; // = pVA->CreateSignalSourceBufferFromFile(soundName); // DELETED HERE
		pVA->SetSignalSourceBufferPlaybackAction(sSignalSourceID, action);
		pVA->SetSignalSourceBufferLooping(sSignalSourceID, loop);



		iSoundSourceID = pVA->CreateSoundSource(soundName + "_source");
		pVA->SetSoundSourcePose(iSoundSourceID, *tmpVec, *tmpQuat);

		//TODO Set Gain
		float power = pVA->GetSoundSourceSoundPower(iSoundSourceID) * gainFactor;
		pVA->SetSoundSourceSoundPower(iSoundSourceID, power);



		pVA->SetSoundSourceSignalSource(iSoundSourceID, sSignalSourceID);

		soundComponentsIDs.Add(iSoundSourceID, sSignalSourceID);
	}
	catch (CVAException& e)
	{
		processExeption("initializeSound", FString(e.ToString().c_str()));
		return -1;
	}

	

	return iSoundSourceID;
}

bool FVAPluginModule::enqueueSound(UVASourceComponent * soundComponent)
{
	uninitializedSoundComponents.Add(soundComponent);
	return true;
}

bool FVAPluginModule::processSoundQueue()
{
	if (!isMasterAndUsed()) {
		return false;
	}

	if (uninitializedSoundComponents.Num() != 0)
	{
		VAUtils::logStuff("processSoundQueue(): Sound Queue is not empty!");
	}

	for (auto iter = uninitializedSoundComponents.CreateIterator(); iter; iter++)
	{
		(*iter)->sendSoundData();
	}

	uninitializedSoundComponents.Empty();

	return true;
}

bool FVAPluginModule::setSoundAction(int iSoundID, int soundAction)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	std::string sSoundID = soundComponentsIDs[iSoundID];
	try	{
		pVA->SetSignalSourceBufferPlaybackAction(sSoundID, soundAction);
	}
	catch (CVAException& e) {
		processExeption("setSoundAction()", FString(e.ToString().c_str()));
		return false;
	}

	return true;
}

bool FVAPluginModule::setSoundActionWithReflections(int soundID, int soundAction)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	TArray<int> reflectionArrayIDs = *soundComponentsReflectionIDs.Find(soundID);

	// HERE DELETE
	setSoundAction(soundID, soundAction);

	for (int id : reflectionArrayIDs) {
		setSoundAction(id, soundAction);
	}

	FString text = "Counter started Reflections: ";
	text.Append(FString::FromInt(reflectionArrayIDs.Num()));
	VAUtils::logStuff(text);

	
	return true;
}

/*
void FVAPluginModule::updateReceiverPos(VAVec3* pos, VAQuat* quat)
{
	if (!pVANet->IsConnected())
		VAUtils::openMessageBox("NOT CONNECTED", true);
	
	VAUtils::scaleVAVec(*pos, scale);

	//SourcepVA->SetSoundReceiverOrientation(iSoundReceiverID, *quat);
	//pVA->SetSoundReceiverPosition(iSoundReceiverID, *pos);
	//pVA->SetSoundReceiverPose(iSoundReceiverID, *pos, *quat);
	
}
 */

/*
 void FVAPluginModule::updateReceiverPosRot(FVector& posF, FQuat& quatF)
{
	VAUtils::fVecToVAVec3Rot(posF, *tmpVec);
	VAUtils::fQuatToVAQuatRot(quatF, *tmpQuat);

	pVA->SetSoundReceiverPose(iSoundReceiverID, *tmpVec, *tmpQuat);
}
*/

/*
void FVAPluginModule::updateReceiverPosRot(FVector& posF, FRotator& rot)
{
	FQuat quatF = rot.Quaternion();
	
	updateReceiverPosRot(posF, quatF);
}
 */

bool FVAPluginModule::updateSourcePos(int iSourceID, FVector pos, FQuat quat)
{
	FRotator rot = quat.Rotator();
	return updateSourcePos(iSourceID, pos, rot);
}

bool FVAPluginModule::updateSourcePos(int iSourceID, FVector pos, FRotator rot)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	VAUtils::rotateFVec(pos);
	VAUtils::rotateFRotator(rot);

	FQuat quat = rot.Quaternion();

	VAUtils::fVecToVAVec3(pos, *tmpVec);
	VAUtils::fQuatToVAQuat(quat, *tmpQuat);

	VAUtils::scaleVAVec(*tmpVec, scale);
	try {
		pVA->SetSoundSourceOrientation(iSoundReceiverID, *tmpQuat);
		pVA->SetSoundSourcePosition(iSoundReceiverID, *tmpVec);
}
	catch (CVAException& e) {
		processExeption("updateSoundSourcePos", FString(e.ToString().c_str()));
		return false;
	}
    
	// pVA->SetSoundSourcePose(iSourceID, *tmpVec, *tmpQuat);
	return true;
}

bool FVAPluginModule::updateSourcePosWithReflections(int iSourceID, FVector pos, FQuat quat)
{
	FRotator rot = quat.Rotator();
	return updateSourcePosWithReflections(iSourceID, pos, rot);
}

bool FVAPluginModule::updateSourcePosWithReflections(int iSourceID, FVector pos, FRotator rot)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	updateSourcePos(iSourceID, pos, rot);

	TArray<int> reflectionArrayIDs = *soundComponentsReflectionIDs.Find(iSourceID);

	for (int id : reflectionArrayIDs) {
		// eval wich wall to reflect on 
		AVAReflectionWall* wall = *matchingReflectionWalls.Find(id);
		
		// get new pos
		// FTransform trans_new = computeReflectedTransform(wall, pos, rot);
        
        FVector pos_new = computeReflectedPos(wall, pos);
        FRotator rot_new = computeReflectedRot(wall, rot);
		
		// update to the new pos
		updateSourcePos(id, pos_new, rot_new);
	}

	return true;
}

FTransform FVAPluginModule::computeReflectedTransform(AVAReflectionWall* wall, FTransform trans)
{
	return computeReflectedTransform(wall, trans.GetLocation(), trans.GetRotation().Rotator());
}

FTransform FVAPluginModule::computeReflectedTransform(AVAReflectionWall* wall, FVector pos, FRotator rot)
{
	FVector pos_new = computeReflectedPos(wall, pos);
	FRotator rot_new = computeReflectedRot(wall, rot);

	// bring all together in new Transform
	FTransform trans_new = FTransform();
	trans_new.SetLocation(pos_new);
	trans_new.SetRotation(rot_new.Quaternion());

	return trans_new;
}


FVector FVAPluginModule::computeReflectedPos(AVAReflectionWall* wall, FVector pos) 
{
	FVector n = wall->getNormalVec();
	float d = wall->getD();
	float t = d - FVector::DotProduct(n, pos);

	return (pos + 2.0 * t * n);
}


FRotator FVAPluginModule::computeReflectedRot(AVAReflectionWall* wall, FRotator rot)
{
	// Transform Positions
	FVector n = wall->getNormalVec();

	// Transform orientation
	FQuat mirrorNormalQuat = FQuat(n.X, n.Y, n.Z, 0); // see https://answers.unrealengine.com/questions/758012/mirror-a-frotator-along-a-plane.html
	FQuat reflectedQuat = mirrorNormalQuat * rot.Quaternion() * mirrorNormalQuat;
	FRotator soundRot_new = reflectedQuat.Rotator();










	FRotator tmp = mirrorNormalQuat.Rotator();
	FString text = "Normal vec: ";
	text.Append(FString::FromInt(n.X)).Append("/").Append(FString::FromInt(n.Y)).Append("/").Append(FString::FromInt(n.Z));
	text.Append(" // normal rot: ");
	text.Append(FString::FromInt(tmp.Roll)).Append("/").Append(FString::FromInt(tmp.Pitch)).Append("/").Append(FString::FromInt(tmp.Yaw));
	// VAUtils::openMessageBox(text);
	
	return soundRot_new;
}


bool FVAPluginModule::updateReceiverPos(FVector pos, FQuat quat)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	FRotator rot = quat.Rotator();
	return updateReceiverPos(pos, rot);
}

bool FVAPluginModule::updateReceiverPos(FVector pos, FRotator rot)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	VAUtils::rotateFVec(pos);
	VAUtils::rotateFRotator(rot);

	FQuat quat = rot.Quaternion();

	VAUtils::fVecToVAVec3(pos, *tmpVec);
	VAUtils::fQuatToVAQuat(quat, *tmpQuat);
	
	VAUtils::scaleVAVec(*tmpVec, scale);
	try {
		pVA->SetSoundReceiverOrientation(iSoundReceiverID, *tmpQuat);
		pVA->SetSoundReceiverPosition(iSoundReceiverID, *tmpVec);
	}
	catch (CVAException& e) {
		processExeption("updateReceiverPos()", FString(e.ToString().c_str()));
		return false;
	}

	//pVA->SetSoundReceiverPose(iSoundReceiverID, *tmpVec, *tmpQuat);
	return true;
}

bool FVAPluginModule::updateReceiverRealWorldPos(FVector pos, FQuat quat)
{
    if (!isMasterAndUsed()) {
        return false;
    }
    
    FRotator rot = quat.Rotator();
    return updateReceiverRealWorldPos(pos, rot);
}

bool FVAPluginModule::updateReceiverRealWorldPos(FVector pos, FRotator rot)
{
    if (!isMasterAndUsed()) {
        return false;
    }

	VAUtils::rotateFVec(pos);
	VAUtils::rotateFRotator(rot);

	FQuat quat = rot.Quaternion();

	VAUtils::fVecToVAVec3(pos, *tmpVec);
	VAUtils::fQuatToVAQuat(quat, *tmpQuat);

	VAUtils::scaleVAVec(*tmpVec, scale);

	try {
		pVA->SetSoundReceiverRealWorldPose(iSoundReceiverID, *tmpVec, *tmpQuat);
	}
	catch (CVAException& e) {
		processExeption("updateReceiverRealWorldPos()", FString(e.ToString().c_str()));
		return false;
	}
	
	return false;
}

bool FVAPluginModule::setReceiverDirectivity(std::string pDirectivity)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	if (pDirectivity == directivity) {
		return true;
	}

	try {
		iHRIR = pVA->CreateDirectivityFromFile(directivity); // DELETED HERE
		pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);
	}
	catch (CVAException& e) {
		processExeption("setReceiverDirectivity", FString(e.ToString().c_str()));
		return false;
	}

	
	return false;
}

bool FVAPluginModule::setSourceDirectivity(int id, FString directivity)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	// Find dir in Map
	int* dirID = dirMap.Find(directivity);

	try {
		// if not available choose default
		if (dirID == nullptr) {
			pVA->SetSoundSourceDirectivity(id, defaultDirID);
			return false;
		}
		else {
			pVA->SetSoundSourceDirectivity(id, *dirID);
			return true;
		}
	}
	catch (CVAException& e) {
		processExeption("setSourceDirectivity()", FString(e.ToString().c_str()));
		return false;
	}
}

bool FVAPluginModule::setSourceDirectivityWithReflections(int soundID, FString directivity)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	TArray<int> reflectionArrayIDs = *soundComponentsReflectionIDs.Find(soundID);

	// HERE DELETE
	setSourceDirectivity(soundID, directivity);

	for (int id : reflectionArrayIDs) {
		setSourceDirectivity(id, directivity);
	}


	return false;
}

bool FVAPluginModule::setSourceDirectivity(int soundID, int dirID)
{
	if (!isMasterAndUsed()) {
		return false;
	}


	try {
		pVA->SetSoundSourceDirectivity(soundID, dirID);
	}
	catch (CVAException& e) {
		processExeption("setSourceDirectivity()", FString(e.ToString().c_str()));
		return false;
	}
	
	return true;
}

bool FVAPluginModule::setSourceDirectivityWithReflections(int soundID, int dirID)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	TArray<int> reflectionArrayIDs = *soundComponentsReflectionIDs.Find(soundID);

	// HERE DELETE
	setSourceDirectivity(soundID, dirID);

	for (int id : reflectionArrayIDs) {
		setSourceDirectivity(id, dirID);
	}

	return true;
}

bool FVAPluginModule::setSourceDirectivity_Phoneme(int soundID, FString phoneme)
{
	FString tmp = *dirMapping.Find(phoneme);
	int dirID = *dirMappingToInt.Find(tmp);

	return setSourceDirectivity(soundID, phoneme);

}

bool FVAPluginModule::setSourceDirectivityWithReflections_Phoneme(int soundID, FString phoneme)
{
	FString tmp = *dirMapping.Find(phoneme);
	int dirID = *dirMappingToInt.Find(tmp);

	return setSourceDirectivityWithReflections(soundID, phoneme);
}




/*
bool FVAPluginModule::runServerTest()
{
	VAUtils::openMessageBox("IN RUNSERVERTEST()", true);

	const std::string sSignalSourceID = pVA->CreateSignalSourceBufferFromFile("$(DemoSound)");
	pVA->SetSignalSourceBufferPlaybackAction(sSignalSourceID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
	pVA->SetSignalSourceBufferLooping(sSignalSourceID, true);

	VAQuat* SourceQuat = new VAQuat(0.0f, 0.0f, 0.0f, 1.0f);
	VAVec3* SourceVec3 = new VAVec3(-2.0f, 1.7f, -2.0f);
	const int iSoundSourceID = pVA->CreateSoundSource("C++ example sound source");
	//pVA->SetSoundSourcePose(iSoundSourceID, VAVec3(-2.0f, 1.7f, -2.0f), VAQuat(0.0f, 0.0f, 0.0f, 1.0f));
	pVA->SetSoundSourcePose(iSoundSourceID, *SourceVec3, *SourceQuat);

	pVA->SetSoundSourceSignalSource(iSoundSourceID, sSignalSourceID);

	const int iHRIR = pVA->CreateDirectivityFromFile("$(DefaultHRIR)");

	VAQuat* RecQuat = new VAQuat(0.0f, 0.0f, 0.0f, 1.0f);
	VAVec3* RecVec3 = new VAVec3(0.0f, 1.7f, 0.0f);
	VAUtils::openMessageBox("createRec (runServerTest)", true);
	const int iSoundReceiverID = pVA->CreateSoundReceiver("C++ example sound receiver");
	pVA->SetSoundReceiverPose(iSoundReceiverID, *RecVec3, *RecQuat);

	pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);
	// do something that suspends the program ...
 
	pVANet->Disconnect();
	return true;
}
*/

bool FVAPluginModule::checkLibraryHandles(bool einzeln)
{
	if (einzeln == true)
	{
		if (LibraryHandleNet) {
			VAUtils::logStuff("loaded Net");
		}
		else {
			VAUtils::logStuff("could not load Net");
		}

		if (LibraryHandleBase) {
			VAUtils::logStuff("loaded Base");
		}
		else {
			VAUtils::logStuff("could not load Base");
		}

		if (LibraryHandleVistaAspects) {
			VAUtils::logStuff("loaded Vista Aspects");
		}
		else {
			VAUtils::logStuff("could not load Vista Aspects");
		}

		if (LibraryHandleVistaBase) {
			VAUtils::logStuff("loaded Vista Base");
		}
		else {
			VAUtils::logStuff("could not load Vista Base");
		}

		if (LibraryHandleVistaInterProcComm) {
			VAUtils::logStuff("loaded Vista InterProcComm");
		}
		else {
			VAUtils::logStuff("could not load Vista InterProcComm");
		}
	}

	if (LibraryHandleNet && LibraryHandleBase &&
		LibraryHandleVistaAspects && LibraryHandleVistaBase && LibraryHandleVistaInterProcComm)
	{
		// all fine
		VAUtils::logStuff("all fine");
		return true;
	}
	else {
		VAUtils::logStuff("could not load all dll");
		return false;
	}

}

bool FVAPluginModule::setViewMode()
{
	/*UEditorEngine*    EditorEngine = Cast<UEditorEngine>(GEngine);
	if (EditorEngine)
	{
		return    EditorEngine->bUseVRPreviewForPlayWorld;
	}*/
	
	viewMode = VAUtils::viewEnum::ThirdPerson;

	return true;
}

VAUtils::viewEnum FVAPluginModule::getViewMode()
{
	return viewMode;
}

bool FVAPluginModule::isViewModeUnclear()
{
	return viewMode == VAUtils::viewEnum::unclear;
}

bool FVAPluginModule::isViewModeThirdPerson()
{
	return viewMode == VAUtils::viewEnum::ThirdPerson;
}

bool FVAPluginModule::isViewModeHMD()
{
	return viewMode == VAUtils::viewEnum::HMD;
}

bool FVAPluginModule::isViewModeCave()
{
	return viewMode == VAUtils::viewEnum::Cave;
}

bool FVAPluginModule::isMasterAndUsed()
{
	return (IDisplayCluster::Get().GetClusterMgr()!= nullptr && IDisplayCluster::Get().GetClusterMgr()->IsMaster() && useVA);
}

void FVAPluginModule::processExeption(FString location, CVAException e)
{
	useVA = false;
	FString exp = FString(e.ToString().c_str());
	UE_LOG(LogTemp, Error, TEXT("Error in [%s] with error: %s"), *location, *exp);
}

void FVAPluginModule::processExeption(FString location, FString exp)
{
	useVA = false;
	UE_LOG(LogTemp, Error, TEXT("Error in [%s] with error: %s"), *location, *exp);
}

bool FVAPluginModule::readDirFile(FString dirName)
{
	if (!isMasterAndUsed()) {
		return false;
	}

	FString BaseDir = IPluginManager::Get().FindPlugin("VAPlugin")->GetBaseDir();
	FString dir = FPaths::Combine(*BaseDir, TEXT("config/directivities/"));
	FString config_file_name = dir + dirName;

	GConfig->UnloadFile(config_file_name);
	GConfig->LoadFile(config_file_name);

	FConfigFile* config = GConfig->FindConfigFile(config_file_name);

	if (config == nullptr) {
		// UE_LOG(LogTemp, Error, TEXT("[Sc]Unable to load study config file: %s"), *config_file_name_);
		VAUtils::logStuff(FString("Unable to load directivity config file") + config_file_name);
		return false;
	}

	listOfPhonemes.Empty();
	listOfPairs.Empty();
	dirMapping.Empty();
	listOfDirectivities.Empty();
	FString divideSymbol;

	//config->GetArray(TEXT("DirectivityMapping"), TEXT("ListOfPhonemesArray"), listOfPhonemes);
	config->GetArray(TEXT("DirectivityMapping"), TEXT("pair"), listOfPairs);
	config->GetString(TEXT("DirectivityMapping"), TEXT("dividingSymbol"), divideSymbol);
	// FString listOfdir;
	// config->GetString(TEXT("DirectivityMapping"), TEXT("ListOfPhonemesString"), listOfDir);

	VAUtils::logStuff("The following inputs for dirs were registered: ");
	FString tmpS, tmpP, tmp; // tmpS = Name of directivity, tmpP = Name of input Phenome "name"
	int tmpPos;
	FString test = "Hallo";
	for (auto entry : listOfPairs) {
		//for (auto entry : listOfPhonemes) {
		// When using a=ad
		// b = bd
		// config->GetString(TEXT("DirectivityMapping"), *entry, tmpS);
		
		// When using Mapping=a,ad
		// Mapping = b,bd
		tmpPos = entry.Find(divideSymbol);
		if (!entry.Contains(divideSymbol)) {
			UE_LOG(LogTemp, Warning, TEXT("   Directivity %s has a wrong format!"), *entry);
			continue;
		}

		tmp = *entry;
		tmp.Split(divideSymbol, &tmpP, &tmpS);

		if (tmpS.IsEmpty() || tmpP.IsEmpty()) {
			UE_LOG(LogTemp, Warning, TEXT("   Directivity %s has a wrong format!"), *entry);
			continue;
		}
		// dirMapping.Add(entry, tmpS);
		// UE_LOG(LogTemp, Warning, TEXT("   %s --> %s"), *entry, *tmpS);

		dirMapping.Add(tmpP, tmpS);
		UE_LOG(LogTemp, Warning, TEXT("   %s --> %s"), *tmpP, *tmpS);
		


		// if(tmpS is not in Directivities yet)
		if (!listOfDirectivities.Contains(tmpS)) {
			listOfDirectivities.Add(tmpS);
		}
	}

	// Do default!
	config->GetString(TEXT("DirectivityMapping"), TEXT("default"), tmpS);
	dirMapping.Add("default", tmpS);
	UE_LOG(LogTemp, Warning, TEXT("   default --> %s"), *tmpS);

	VAUtils::logStuff("------------------"); 	  
	VAUtils::logStuff("Finished reading Dir File");


	FString folder;
	config->GetString(TEXT("DirectivityMapping"), TEXT("dirFolder"), folder);


	// First do default
	std::string dirD(TCHAR_TO_UTF8(*(folder + tmpS)));
	try {
		iHRIR = pVA->CreateDirectivityFromFile(dirD);
		dirMappingToInt.Add(tmpS, iHRIR);
	}
	catch (CVAException& e) {
		// VAUtils::openMessageBox("Hallo");
		// processExeption("readDirFile", FString(*e.ToString().c_str()));
		// VAUtils::logStuff(FString("Could not read Directivity from phenmoes config: " + *e.ToString().c_str()));
		if (&e == nullptr)
		{
		}
	}

	// ************ // 


	int iHRIR;
	for (auto entry : listOfDirectivities) {
		// tmp = folder;
		// tmp.Append(entry);
		std::string dir(TCHAR_TO_UTF8(*(folder + entry)));
		try {
			iHRIR = pVA->CreateDirectivityFromFile(dir);
			dirMappingToInt.Add(entry, iHRIR);
		}
		catch (CVAException& e) {
			// VAUtils::openMessageBox("Hallo");
			// processExeption("readDirFile", FString(*e.ToString().c_str()));
			// VAUtils::logStuff(FString("Could not read Directivity from phenmoes config: " + *e.ToString().c_str()));
			if (&e == nullptr)
			{
			}
			continue;
		}
	}

	// Do default

	return true;
}


bool FVAPluginModule::isConnected()
{
	if (!isMasterAndUsed()) {
		return false;
	}

	// TODO return false if nullptr
	if (pVANet == nullptr) {
		return false;
	}

	try {
		return pVANet->IsConnected();
	}
	catch (CVAException& e) {
		processExeption("isConnected", FString(e.ToString().c_str()));
		return false;
	}

}

bool FVAPluginModule::initializeSoundSourceDirectivities()
{
	// Read config file

	// set defaultDirID

	// fill dirMap


	return false;
}

bool FVAPluginModule::resetAll()
{
	if (!isMasterAndUsed()) {
		return false;
	}

	resetServer();

	receiverActor = nullptr; 
	uninitializedSoundComponents.Empty();
	
	soundComponents.Empty();
	soundComponentsIDs.Empty();
	soundComponentsReflectionIDs.Empty();

	matchingReflectionWalls.Empty();
	reflectionWalls.Empty();

	return false;
}

bool FVAPluginModule::isInDebugMode()
{
	return debugMode;
}


/*
void FVAPluginModule::openMessageBox(char* text, bool error)
{
	//char* text;
	if (error) {
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ANSI_TO_TCHAR(addExclamationMarkInChar(text))));
		//text = addExclamationMarkInChar(text);
	}
	else {
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ANSI_TO_TCHAR(text)));
		//text = text_p;
	}
	//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ANSI_TO_TCHAR(text)), &FText::FromString(ANSI_TO_TCHAR(title)));
	//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ANSI_TO_TCHAR(text)));
}

void FVAPluginModule::openMessageBoxV(char* text, bool error)
{
	if (M_VERBOSE)
		openMessageBox(text, error);
}

char* FVAPluginModule::addExclamationMarkInChar(char* text)
{
	size_t len = strlen(text);
	char* ret = new char[len + 21];

	for (int i = 0; i < 10; i++)
	{
		ret[i] = '!';
		ret[i + 10 + len] = '!';
	}

	for (int i = 0; i < len; i++)
	{
		ret[10 + i] = text[i];
	}

	ret[len + 20] = '\0';

	strcpy(ret, text);
	ret[len] = a;
	ret[len + 1] = '\0';

	return ret;
}

*/

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVAPluginModule, VAPlugin)
