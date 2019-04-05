// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VAPlugin.h"

#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "VAReceiverActor.h"
#include "VASourceComponent.h"
//#include "VA.h"
//#include "VANet.h"
#include "VistaBase/VistaTimeUtils.h"

#include "VADefines.h"



#define LOCTEXT_NAMESPACE "FVAPluginModule"

IVANetClient* FVAPluginModule::pVANet;
IVAInterface* FVAPluginModule::pVA;
AVAReceiverActor* FVAPluginModule::receiverActor;
TArray<UVASourceComponent*> FVAPluginModule::soundComponents;
TArray<UVASourceComponent*> FVAPluginModule::uninitializedSoundComponents;
TMap<int, std::string> FVAPluginModule::soundComponentsIDs;

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


void FVAPluginModule::StartupModule()
{
	// http://www.virtualacoustics.org/overview.html // 
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	setViewMode();

	// ++ Get Paths of DLL Handles ++ //
	//FString LibraryPath;
	FString BaseDir = IPluginManager::Get().FindPlugin("VAPlugin")->GetBaseDir();
	FString pathNet, pathBase, pathVistaAspects, pathVistaBase, pathVistaInterProcComm;

#if PLATFORM_WINDOWS
	//LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/VAPlugin/x64/Release/VABase.dll"));
	pathNet					=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VANet.dll"));
	pathBase				=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VABase.dll"));
	pathVistaAspects		=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaAspects.dll"));
	pathVistaBase			=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaBase.dll"));
	pathVistaInterProcComm	=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaInterProcComm.dll"));
#elif PLATFORM_LINUX
	//LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/VAPlugin/x64/Release/VABase.dll"));
	pathNet					=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VANet.so"));
	pathBase				=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VABase.so"));
	pathVistaAspects		=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaAspects.so"));
	pathVistaBase			=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaBase.so"));
	pathVistaInterProcComm	=	FPaths::Combine(*BaseDir, TEXT("Source/VALibrary/lib/VistaInterProcComm.so"));
#endif

	// ++ Load DLL Handles ++ // 
	//ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	LibraryHandleVistaBase			= !pathVistaBase.IsEmpty()			? FPlatformProcess::GetDllHandle(*pathVistaBase)			: nullptr;
	LibraryHandleVistaAspects		= !pathVistaAspects.IsEmpty()		? FPlatformProcess::GetDllHandle(*pathVistaAspects)			: nullptr;
	LibraryHandleVistaInterProcComm = !pathVistaInterProcComm.IsEmpty() ? FPlatformProcess::GetDllHandle(*pathVistaInterProcComm)	: nullptr;
	LibraryHandleBase				= !pathBase.IsEmpty()				? FPlatformProcess::GetDllHandle(*pathBase)					: nullptr;
	LibraryHandleNet				= !pathNet.IsEmpty()				? FPlatformProcess::GetDllHandle(*pathNet)					: nullptr;

	// ++ Check Handles ++ //
	if (!checkLibraryHandles(true))
	{
		VAUtils::openMessageBoxV("stop initialzing because of incomplete libraryHandles", true);
		return;
	}
	
	//runServerTest();
	
	//delete pVANet;

}

void FVAPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	if (pVANet != nullptr)
		if (pVANet->IsConnected())
			pVANet->Disconnect();


	FPlatformProcess::FreeDllHandle(LibraryHandleNet);
	FPlatformProcess::FreeDllHandle(LibraryHandleBase);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaAspects);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaBase);
	FPlatformProcess::FreeDllHandle(LibraryHandleVistaInterProcComm);

}

bool FVAPluginModule::connectServer(FString hostF, int port)
{
	VAUtils::openMessageBox("Connecting to VAServer. Be sure to have it switched on");

	pVANet = IVANetClient::Create();

	std::string host(TCHAR_TO_UTF8(*hostF));
	pVANet->Initialize(host, port);
	if (!pVANet->IsConnected()) {
		VAUtils::openMessageBox("Could not connect to VA Server", true);
		return false;
	}

	pVA = pVANet->GetCoreInstance();
	pVA->Reset();

	return true;
}

bool FVAPluginModule::initializeServer(FString host, int port)
{
	if (!connectServer(host, port)) {
		// Try again
		return connectServer(host, port);
	}
	return true;
}

bool FVAPluginModule::initializeReceiver(AVAReceiverActor* actor)
{
	receiverActor = actor;

	scale = receiverActor->getScale();

	//receiverActor->getStartingPosition();

	tmpQuat->Set(0.0f, 0.0f, 0.0f, 1.0f);
	tmpVec->Set(0.0f, 1.7f, 0.0f);

	iSoundReceiverID = pVA->CreateSoundReceiver("VASoundReceiver");
	updateReceiverPos(tmpVec, tmpQuat);

	std::string dir = receiverActor->getDirectivity();
	iHRIR = pVA->CreateDirectivityFromFile(dir);
	pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);

	pVA->SetOutputGain(actor->getGainFactor());

	return true;
}

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


	//updateReceiverPos(SourceVec3, SourceQuat);
	//updateReceiverPos(SourceVec3, SourceQuat);
	//updateReceiverPos(SourceVec3, SourceQuat);
	//updateReceiverPos(SourceVec3, SourceQuat);
	//updateReceiverPos(SourceVec3, SourceQuat);

	return true;
}

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

int  FVAPluginModule::initializeSound(FString soundNameF, FVector soundPos, FRotator soundRot, float gain, bool loop, float soundOffset, int action)
{
	soundPos = VAUtils::toVACoordinateSystem(soundPos);
	soundRot = VAUtils::toVACoordinateSystem(soundRot);

	FQuat fQuat = soundRot.Quaternion();
	VAUtils::fQuatToVAQuat(fQuat, *tmpQuat);
	
	VAUtils::fVecToVAVec3(soundPos, *tmpVec);
	VAUtils::scaleVAVec(*tmpVec, scale);

	std::string soundName = std::string(TCHAR_TO_UTF8(*soundNameF));

	const std::string sSignalSourceID = pVA->CreateSignalSourceBufferFromFile(soundName);
	pVA->SetSignalSourceBufferPlaybackAction(sSignalSourceID, action);
	pVA->SetSignalSourceBufferLooping(sSignalSourceID, loop);



	const int iSoundSourceID = pVA->CreateSoundSource(soundName + "_source");
	pVA->SetSoundSourcePose(iSoundSourceID, *tmpVec, *tmpQuat);

	pVA->SetSoundSourceSignalSource(iSoundSourceID, sSignalSourceID);

	soundComponentsIDs.Add(iSoundSourceID, sSignalSourceID);

	//TODO Set Gain

	return iSoundSourceID;
}

bool FVAPluginModule::enqueueSound(UVASourceComponent * soundComponent)
{
	uninitializedSoundComponents.Add(soundComponent);
	return true;
}

bool FVAPluginModule::processSoundQueue()
{
	for (auto iter = uninitializedSoundComponents.CreateIterator(); iter; iter++)
	{
		(*iter)->sendSoundData();
	}
	
	return true;
}

bool FVAPluginModule::setSoundAction(int iSoundID, int soundAction)
{
	std::string sSoundID = soundComponentsIDs[iSoundID];
	pVA->SetSignalSourceBufferPlaybackAction(sSoundID, soundAction);

	return true;
}

void FVAPluginModule::updateReceiverPos(VAVec3* pos, VAQuat* quat)
{
	if (!pVANet->IsConnected())
		VAUtils::openMessageBox("NOT CONNECTED", true);
	
	VAUtils::scaleVAVec(*pos, scale);

	//SourcepVA->SetSoundReceiverOrientation(iSoundReceiverID, *quat);
	//pVA->SetSoundReceiverPosition(iSoundReceiverID, *pos);
	//pVA->SetSoundReceiverPose(iSoundReceiverID, *pos, *quat);
	
}

void FVAPluginModule::updateReceiverPosRot(FVector& posF, FQuat& quatF)
{
	VAUtils::fVecToVAVec3Rot(posF, *tmpVec);
	VAUtils::fQuatToVAQuatRot(quatF, *tmpQuat);

	pVA->SetSoundReceiverPose(iSoundReceiverID, *tmpVec, *tmpQuat);
}

void FVAPluginModule::updateReceiverPosRot(FVector& posF, FRotator& rot)
{
	FQuat quatF = rot.Quaternion();
	
	updateReceiverPosRot(posF, quatF);
}

bool FVAPluginModule::updateSourcePos(int iSourceID, FVector pos, FQuat quat)
{
	FRotator rot = quat.Rotator();
	return updateSourcePos(iSourceID, pos, rot);
}

bool FVAPluginModule::updateSourcePos(int iSourceID, FVector pos, FRotator rot)
{
	VAUtils::rotateFVec(pos);
	VAUtils::rotateFRotator(rot);

	FQuat quat = rot.Quaternion();

	VAUtils::fVecToVAVec3(pos, *tmpVec);
	VAUtils::fQuatToVAQuat(quat, *tmpQuat);

	VAUtils::scaleVAVec(*tmpVec, scale);

	pVA->SetSoundSourcePose(iSourceID, *tmpVec, *tmpQuat);
	return true;
}

bool FVAPluginModule::updateReceiverPos(FVector pos, FQuat quat)
{
	FRotator rot = quat.Rotator();
	return updateReceiverPos(pos, rot);
}

bool FVAPluginModule::updateReceiverPos(FVector pos, FRotator rot)
{
	VAUtils::rotateFVec(pos);
	VAUtils::rotateFRotator(rot);

	FQuat quat = rot.Quaternion();

	VAUtils::fVecToVAVec3(pos, *tmpVec);
	VAUtils::fQuatToVAQuat(quat, *tmpQuat);
	
	VAUtils::scaleVAVec(*tmpVec, scale);

	pVA->SetSoundReceiverOrientation(iSoundReceiverID, *tmpQuat);
	pVA->SetSoundReceiverPosition(iSoundReceiverID, *tmpVec);
	//pVA->SetSoundReceiverPose(iSoundReceiverID, *tmpVec, *tmpQuat);
	return true;
}

bool FVAPluginModule::setReceiverDirectivity(std::string pDirectivity)
{
	if (pDirectivity == directivity)
		return true;

	iHRIR = pVA->CreateDirectivityFromFile(directivity);
	pVA->SetSoundReceiverDirectivity(iSoundReceiverID, iHRIR);
	
	return false;
}

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
	/*
	// do something that suspends the program ...
	*/
	pVANet->Disconnect();
	return true;
}

bool FVAPluginModule::checkLibraryHandles(bool einzeln)
{
	if (einzeln == true)
	{
		if (LibraryHandleNet) {
			VAUtils::openMessageBoxV("loaded Net");
		}
		else {
			VAUtils::openMessageBox("could not load Net", true);
		}

		if (LibraryHandleBase) {
			VAUtils::openMessageBoxV("loaded Base");
		}
		else {
			VAUtils::openMessageBox("could not load Base", true);
		}

		if (LibraryHandleVistaAspects) {
			VAUtils::openMessageBoxV("loaded Vista Aspects");
		}
		else {
			VAUtils::openMessageBox("could not load Vista Aspects", true);
		}

		if (LibraryHandleVistaBase) {
			VAUtils::openMessageBoxV("loaded Vista Base");
		}
		else {
			VAUtils::openMessageBox("could not load Vista Base", true);
		}

		if (LibraryHandleVistaInterProcComm) {
			VAUtils::openMessageBoxV("loaded Vista InterProcComm");
		}
		else {
			VAUtils::openMessageBox("could not load Vista InterProcComm", true);
		}
	}

	if (LibraryHandleNet && LibraryHandleBase &&
		LibraryHandleVistaAspects && LibraryHandleVistaBase && LibraryHandleVistaInterProcComm)
	{
		// all fine
		VAUtils::openMessageBoxV("all fine");
		return true;
	}
	else {
		VAUtils::openMessageBox("could not load all dll", true);
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

bool FVAPluginModule::isConnected()
{
	// TODO return false if nullptr
	if (pVANet == nullptr)
		return false;

	return pVANet->IsConnected();
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

	/*strcpy(ret, text);
	ret[len] = a;
	ret[len + 1] = '\0';

	return ret;
}

*/

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVAPluginModule, VAPlugin)
