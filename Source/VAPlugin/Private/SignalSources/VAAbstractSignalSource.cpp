// Fill out your copyright notice in the Description page of Project Settings.


#include "VAAbstractSignalSource.h"

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
	return IsValidID(ID);
}

bool UVAAbstractSignalSource::IsValidID(const std::string& ID)
{
	return !ID.empty() && ID != "-1";
}
