// Fill out your copyright notice in the Description page of Project Settings.


#include "SignalSources/VAAbstractSignalSource.h"

#include "VADefines.h"

UVAAbstractSignalSource::UVAAbstractSignalSource() : ID(VA_INVALID_ID_STRING)
{
}

std::string UVAAbstractSignalSource::GetPrototypeName()
{
	return "invalid";
}

std::string UVAAbstractSignalSource::GetID() const
{
	return ID;
}

bool UVAAbstractSignalSource::IsValid() const
{
	return VA::IsValidID(ID);
}

bool UVAAbstractSignalSource::IsValidID(const std::string& ID)
{
	return VA::IsValidID(ID);
}
