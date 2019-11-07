// Class for graphical representation of every Sound Source (and Representation)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "VASoundSourceRepresentation.generated.h"

UCLASS()
class VAPLUGIN_API AVASoundSourceRepresentation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVASoundSourceRepresentation();

	UPROPERTY(VisibleDefaultsOnly)
		class USphereComponent* sphereComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* sphereMesh;

	bool setPos(FVector pos);

	bool setRot(FRotator rot);

	bool setVisibility(bool visibility);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
