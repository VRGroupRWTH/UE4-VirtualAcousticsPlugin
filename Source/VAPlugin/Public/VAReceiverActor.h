// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAPlugin.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VAReceiverActor.generated.h"

// struct used to input Directivity via Variable in UE4 Editor
UENUM()
enum EDir { 
	DefaultHRIR
}; 

// struct used to input IP-Adress of VA server via Variable in UE4 Editor
USTRUCT()
struct FAdress {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, meta = (DisplayName = "xxx._._._"))
		uint8 ip1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "_.xxx._._"))
		uint8 ip2;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "_._.xxx._"))
		uint8 ip3;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "_._._.xxx"))
		uint8 ip4;

	FAdress(uint8 ip1_ = 127, uint8 ip2_ = 0, uint8 ip3_ = 0, uint8 ip4_ = 1) : ip1(ip1_), ip2(ip2_), ip3(ip3_), ip4(ip4_)
	{ }
};


UCLASS()
class VAPLUGIN_API AVAReceiverActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVAReceiverActor();

	// Factor for global output Gain
	UPROPERTY(EditAnywhere, meta=(DisplayName = "Gain Factor [0,1]", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float vGainFactor = 1.0f;
	
	// Choose Directivity File for Receiver
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity"))
		TEnumAsByte<EDir> vDirectivity = EDir::DefaultHRIR;

	// How many units in UE equal 1m in World
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Scale"))
		float vScale = 100.0f;

	// IP Adress of VA Server. 127.0.0.1 = localhost
	UPROPERTY(EditAnywhere, meta = (DisplayName = "IP Adress"))
		FAdress vIPAdress = FAdress(127,0,0,1);


	
	// return directivity x in form: $(x)
	std::string getDirectivity();
	
	// return Offset
	float getGainFactor();

	// return Scale
	float getScale();

	// return IP Adress as FString TODO wie sonst?
	FString getIPAdress();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Updates the Virtual World Position of the Player
	bool updateVirtualWorldPosition();

	// Updates the Real World Position of the Player
	bool updateRealWorldPosition();

	// Link to the PlayerController
	APlayerController* controller;

	// Link to the Player Instance
	AActor* player;

	// var used to measure time since last update
	float timeSinceUpdate;

	// Tmp Var
	FVector		tmpPosF;
	FRotator	tmpRotF;
	FQuat		tmpQuatF;
	VAVec3*		tmpPos;
	VAQuat*		tmpQuat;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
