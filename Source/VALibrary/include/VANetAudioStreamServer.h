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

#ifndef IW_VANET_AUDIO_STREAM_SERVER
#define IW_VANET_AUDIO_STREAM_SERVER

#include <VANetDefinitions.h>

#include <VABase.h>
#include <VASamples.h>

#include <string>

class CVANetAudioStreamServerImpl;

//! Stream server framework for providing samples for a network stream
/**
  * This class helps to set up a network audio streaming server
  * to be connected with a VA network stream. Process() method
  * needs to be implemented by user and is called by the network
  * server implementation occasionally if more samples are requested.
  * Timing can be observed with the stream status, i.e. a time stamp and
  * sample count. The timing is most probably not as acurate as an 
  * audio stream that is driven by a sound card, because network
  * transmission deviates stream process call timings.
  *
  */
class VANET_API CVANetAudioStreamServer
{
public:
	//! Create a network audio stream
	/**
	  * @param[in] dSampleRate Sampling rate
	  * @param[in] iBlockLength Block length
	  */
	CVANetAudioStreamServer( const double dSampleRate, const int iBlockLength );

	//! Destructor
	virtual ~CVANetAudioStreamServer();

	//! Initialize server and start listening on network port
	/**
	  * @param[in] sBindAddress Bind address of socket
	  * @param[in] iBindPort Bind port of socket
	  *
	  * @return True, if initialization was successful
	  */
	bool Initialize( const std::string& sBindAddress, const int iBindPort );

	//! Provide a frame of samples on request (overload this method)
	/**
	  * Networked stream process callback function, please overload and implement.
	  * @param[out] oFrame Target frame buffer with sampels to be generated
	  * @param[in] oState Stream state of audio processing
	  */
	virtual void Process( CVASampleBuffer& oFrame, const CVAAudiostreamState& oState ) = 0;

private:
	CVANetAudioStreamServerImpl* m_pImpl; //!< Implementation instance pointer
};

#endif // IW_VANET_AUDIO_STREAM_SERVER
