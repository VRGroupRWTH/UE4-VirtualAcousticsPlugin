/*
 *  --------------------------------------------------------------------------------------------
 *
 *    VVV        VVV A           Virtual Acoustics (VA) | http://www.virtualacoustics.org
 *     VVV      VVV AAA          Licensed under the Apache License, Version 2.0
 *      VVV    VVV   AAA
 *       VVV  VVV     AAA        Copyright 2015-2018
 *        VVVVVV       AAA       Institute of Technical Acoustics (ITA)
 *         VVVV         AAA      RWTH Aachen University
 *
 *  --------------------------------------------------------------------------------------------
 */

#ifndef IW_VABASE_SAMPLES
#define IW_VABASE_SAMPLES

#include <VABaseDefinitions.h>
#include <vector>

//! Data base class for audio samples
/**
  * Not thread-safe. Uses 32-bit floating type.
  *
  * This class is the basis for any time data set.
  * VA provides network transmission, i.e. using
  * the VAStruct as transport container.
  *
  * In VACore, this class is derived for extended
  * functionality, i.e. for impulse responses.
  */
class VABASE_API CVASampleBuffer
{
public:
	//! Constructor with empty samples
	CVASampleBuffer();

	//! Constructor for given number of samples
	/**
	  * @param[in] iNumSamples Number of samples
	  * @param[in] bZeroInit Will init all samples to zero if true (default)
	  */
	CVASampleBuffer( const int iNumSamples, const bool bZeroInit = true );

	//! Copy constructor
	/**
	  * @param[in] oCopyFrom Copy data from this sample buffer
	  */
	CVASampleBuffer( const CVASampleBuffer& oCopyFrom );
	virtual ~CVASampleBuffer();

	//! Number of samples, zero if buffer is uninitialized
	/**
	  * @return Number of samples
	  */
	int GetNumSamples() const;

	//! Pointer to first sample of buffer in memory
	/**
	  * @return Data pointer as float
	  */
	float* GetData();

	//! Pointer to first sample of buffer in memory
	/**
	  * @return Read-only data pointer as float
	  */
	const float* GetDataReadOnly() const;

	//! Sets all samples to zero
	void SetZero();

	std::vector< float > vfSamples; //!< Vector of audio samples, 32-bit floating point precision

	//! Assignment operator
	/**
	  * @param[in] oOther Other sample buffer
	  * @return Sample buffer with assigned data from other sample buffer
	  */
	CVASampleBuffer& operator=( const CVASampleBuffer& oOther );
};

#endif // IW_VABASE_SAMPLES
