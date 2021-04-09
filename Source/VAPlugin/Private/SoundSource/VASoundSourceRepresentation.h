// Class for graphical representation of every Sound Source (and Representation)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"

#include "VASoundSourceRepresentation.generated.h"

UCLASS(ClassGroup = (VA))
class VAPLUGIN_API AVASoundSourceRepresentation : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVASoundSourceRepresentation();

	// Setter
	void SetPosition(FVector Pos);
	void SetRotation(FRotator Rot);
	void SetVisibility(bool bVisibility);

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	// Graphical Components
	USphereComponent* SphereComponent;
	UStaticMeshComponent* SphereMesh;

};
