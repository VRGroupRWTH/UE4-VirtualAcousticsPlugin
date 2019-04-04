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

#ifndef IW_VABASE_AUDIO_SIGNAL_SOURCE
#define IW_VABASE_AUDIO_SIGNAL_SOURCE

#include <VABaseDefinitions.h>
#include <VABase.h>
#include <VAStruct.h>

#include <string>

// Forward declarations
class IVAInterface;

//! Interface class for audio signal sources
/**
  * This purely abstract class defines an interface for
  * data source objects that provide audio-streams. It
  * is intended to acoustically represent sound emitting
  * objects in a virtual scene.
  *
  * A data source is always monaural (single channel)
  * and can only be connected to a single instance of a
  * #IVAInterface during runtime.
  */
class IVAAudioSignalSource
{
public:
	//! Type literals
	/**
	  * The type literals describe the type of the audio signal source.
	  */
	enum AudioSignalSourceType
	{
		VA_SS_AUDIOFILE = 0,	//!< Sound source is an audio file (from secondary storage)
		VA_SS_SEQUENCER,	//!< Sound source is a music sequencer (MIDI, DAWs, ...)
		VA_SS_DEVICE_INPUT, //!< Sound source is fed through audio device input channel(s)
		VA_SS_NETSTREAM,	//!< Sound source is a network stream receiver socket
		VA_SS_ENGINE,		//!< Sound source is an engine
		VA_SS_MACHINE,		//!< Sound source is a machine
		VA_SS_TEXT_TO_SPEECH, //!< Text-to-speech signal source
	};

	//! Destructor.
	inline virtual ~IVAAudioSignalSource() {};

	//! Type ID getter
	/**
	  * @return Returns the ID (one out of AudioSignalSourceType)
	  */
	virtual int GetType() const = 0;

	//! Type getter (human readable string)
	/**
	  * @return Returns the type (one out of AudioSignalSourceType as a human readable string)
	  */
	virtual std::string GetTypeString() const = 0;

	//! Description getter
	/**
	  * The description formulates a human readable string that describes the
	  * sound source, i.e. 'Network stream on port 2344'
	  *
	  * @return Returns a human readable sound source description
	  */
	virtual std::string GetDesc() const = 0;

	//! State getter
	/**
	  * The sound state is for example the percentage of progess
	  * in an audio file sound source, i.e. 'Playback (10%)'
	  *
	  * @return Returns a human readable status of the sound source
	  */
	virtual std::string GetStateString() const = 0;

	//! Associated core getter
	/**
	  * Returns the pointer to the associated core (#IVAInterface) that is
	  * registered with the sound source. Will return NULL if the source
	  * has not been associated to a core yet.
	  * This method is used by the core to assuure that the sound source
	  * object has not been associated to another core (not allowed).
	  *
	  * @return Pointer to the associated #IVAInterface, NULL if non present
	  */
	virtual IVAInterface* GetAssociatedCore() const = 0;

	//! Stream block getter
	/**
	  * Returns the data pointer to the next audio block of the sound source.
	  *
	  * @param[in] pStreamInfo Information on stream state
	  *
	  * @return Pointer to the audio data array containing data for next block (NULL if no data available)
	  *
	  * @note This method is called by the core to fetch data. If the core receives a NULL pointer it will be interpreted as silence.
	  */
	virtual const float* GetStreamBlock( const CVAAudiostreamState* pStreamInfo ) = 0;

	//! Parameter getter interaction hook
	/**
	  * @param[in] oIn Parameter request set
	  * @return Parameters
	  */
	virtual CVAStruct GetParameters( const CVAStruct& oIn ) const = 0;

	//! Parameter setter interaction hook
	/**
	  * @param[in] oIn Parameter set
	  */
	virtual void SetParameters( const CVAStruct& oIn ) = 0;


private:
	//! Register sound source at a core instance
	/**
	  * Calls the audio signal source manager to register this sound source a the given core.
	  *
	  * @param[in] pParentCore Pointer to the associated core.
	  *
	  * @sa CVAAudioSignalSourceManager
	  */
	virtual void HandleRegistration( IVAInterface* pParentCore ) = 0;

	//! Unregister sound source at a core instance
	/**
	  * Calls the audio signal source manager to unregister this sound source a the given core.
	  * Will inform the core about a new signal source.
	  *
	  * @param[in] pParentCore Pointer to the associated core.
	  *
	  * @sa CVAAudioSignalSourceManager
	  */
	virtual void HandleUnregistration( IVAInterface* pParentCore ) = 0;

	friend class CVAAudioSignalSourceManager;
};

#endif // IW_VABASE_AUDIO_SIGNAL_SOURCE
