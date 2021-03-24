// Fill out your copyright notice in the Description page of Project Settings.


#include "VAAbstractSignalSource.h"

std::string UVAAbstractSignalSource::GetPrototypeName()
{
	return "invalid";
}

std::string UVAAbstractSignalSource::GetID() const
{
	return sID;
}

bool UVAAbstractSignalSource::IsValid() const
{
	return !bool( FString(sID.c_str()).Equals(FString("-1")) );
}
