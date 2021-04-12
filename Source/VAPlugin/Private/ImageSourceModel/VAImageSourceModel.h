// Class for a Sound Source which can handle its reflections 

#pragma once

#include "CoreMinimal.h"

#include <string>
#include "Templates/SharedPointer.h"

#include "SoundSource/VASoundSource.h"

class AVAReflectionWall;
class UWorld;

// A simple, first-order image source (IS) model that creates additional VA sound sources based on an original source and a set of reflection walls
//   Important: This model only makes sense when using a free-field renderer in VA. Additionally, no audibility test is done for the image sources, they are always assumed to be audible.
class FVAImageSourceModel
{
public:

	// ---Initialization---

	FVAImageSourceModel(UWorld* World, FVASoundSource* ParentSource, const TArray<AVAReflectionWall*>& ReflectionWalls);

	// ---Setter---

	// Updates all IS positions based on current parent source position and the respective walls
	void UpdateISPositions();
	// Updates all IS rotations based on current parent source orientation and the respective walls
	void UpdateISRotations();
	// Sets the visibility for all image sources
	void SetISVisibility(bool bVisible);
	// (Un-)Mutes all image sources
	bool MuteIS(bool bMute = true);

	bool UpdateISSignalSource();

	// Updates all IS directivities to match the one of the parent source
	bool UpdateISDirectivities();
	// Removes the directivities of all image sources
	bool RemoveISDirectivities();

	// Updates the sound power of all IS based on parent source power and the respective reflection factors
	bool UpdateISPower();


	// ---Mirroring of position and rotation---

	// Computes an image source position based on a reflection wall and the position of the original source
	static FVector ComputeReflectedPos(const AVAReflectionWall* Wall, const FVector& Pos);
	// Computes an image source rotation based on a reflection wall and the rotation of the original source
	static FRotator ComputeReflectedRot(const AVAReflectionWall* Wall, const FRotator& Rot);


private:
	class FVAImageSource : public FVASoundSource
	{
	public:
		FVAImageSource(AVAReflectionWall* Wall, UWorld* World, const FVector& PosN, const FRotator& RotN, float PowerN, const std::string& NameN, TSharedPtr<FVADirectivity> Directivity = nullptr);
		AVAReflectionWall* GetWall();
	private:
		AVAReflectionWall* Wall;
	};

	FVASoundSource* ParentSource;
	TArray<TSharedRef<FVAImageSource>> ImageSources;

};
