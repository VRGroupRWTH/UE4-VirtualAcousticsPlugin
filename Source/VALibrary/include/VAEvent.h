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

#ifndef IW_VABASE_EVENT
#define IW_VABASE_EVENT

#include <VABaseDefinitions.h>
#include <VABase.h>

#include <string>
#include <vector>

// Forwards
class IVAInterface;

// Helper macro for 64-bit bitmasks
#define VA_BIT64( i ) ( ( ( const uint64_t ) 1 ) << i )

//! Data class encapsulating events of a VA
/**
  * Events are emitted by the core and propagated to all atached event handlers.
  * It is up to the event handlers to filter the corresponding types and react upon
  * the data provided by the event. Therefore, the event data is not fixed but is
  * related to the event type. Many of the provided slots are unused for simple
  * events (like a gain change of the core).
  *
  * Use this events to synchronize your application if you are not controlling the
  * core and can not intercept method calls. This way, you still receive information
  * on updates e.g. from real world sound receiver head rotation and so on.
  *
  */
class VABASE_API CVAEvent
{
public:

	static const uint64_t NOTHING = 0;

	static const uint64_t INITIALIZED = VA_BIT64( 1 );
	static const uint64_t FINALIZED = VA_BIT64( 2 );
	static const uint64_t PROGRESS_UPDATE = VA_BIT64( 3 );

	static const uint64_t DIRECTIVITY_LOADED = VA_BIT64( 4 );
	static const uint64_t DIRECTIVITY_DELETED = VA_BIT64( 5 );

	static const uint64_t SIGNALSOURCE_CREATED = VA_BIT64( 6 );
	static const uint64_t SIGNALSOURCE_DELETED = VA_BIT64( 7 );
	static const uint64_t SIGNALSOURCE_REGISTERED = VA_BIT64( 8 );
	static const uint64_t SIGNALSOURCE_UNREGISTERED = VA_BIT64( 9 );
	static const uint64_t SIGNALSOURCE_STATE_CHANGED = VA_BIT64( 10 );


	static const uint64_t SAMPLE_LOADED = VA_BIT64( 11 );
	static const uint64_t SAMPLE_FREED = VA_BIT64( 12 );

	static const uint64_t SOUND_SOURCE_CREATED = VA_BIT64( 13 );
	static const uint64_t SOUND_SOURCE_DELETED = VA_BIT64( 14 );
	static const uint64_t SOUND_SOURCE_CHANGED_NAME = VA_BIT64( 15 );
	static const uint64_t SOUND_SOURCE_CHANGED_SIGNALSOURCE = VA_BIT64( 16 );
	static const uint64_t SOUND_SOURCE_CHANGED_AURALIZATIONMODE = VA_BIT64( 17 );
	static const uint64_t SOUND_SOURCE_CHANGED_SOUND_POWER = VA_BIT64( 18 );
	static const uint64_t SOUND_SOURCE_CHANGED_MUTING = VA_BIT64( 19 );
	static const uint64_t SOUND_SOURCE_CHANGED_POSE = VA_BIT64( 20 );

	static const uint64_t SOUND_RECEIVER_CREATED = VA_BIT64( 21 );
	static const uint64_t SOUND_RECEIVER_DELETED = VA_BIT64( 22 );
	static const uint64_t SOUND_RECEIVER_CHANGED_NAME = VA_BIT64( 23 );
	static const uint64_t SOUND_RECEIVER_CHANGED_AURALIZATIONMODE = VA_BIT64( 24 );
	static const uint64_t SOUND_RECEIVER_CHANGED_POSE = VA_BIT64( 25 );

	static const uint64_t SOUND_PORTAL_CHANGED_NAME = VA_BIT64( 26 );
	static const uint64_t SOUND_PORTAL_CHANGED_POSE = VA_BIT64( 27 );
	static const uint64_t SOUND_PORTAL_CHANGED_PARAMETER = VA_BIT64( 41 );

	static const uint64_t ACTIVE_SOUND_RECEIVER_CHANGED = VA_BIT64( 28 );
	static const uint64_t INPUT_GAIN_CHANGED = VA_BIT64( 29 );
	static const uint64_t INPUT_MUTING_CHANGED = VA_BIT64( 30 );
	static const uint64_t OUTPUT_GAIN_CHANGED = VA_BIT64( 31 );
	static const uint64_t OUTPUT_MUTING_CHANGED = VA_BIT64( 32 );
	static const uint64_t GLOBAL_AURALIZATION_MODE_CHANGED = VA_BIT64( 33 );

	static const uint64_t UNSPECIFIED_34 = VA_BIT64( 34 );
	static const uint64_t UNSPECIFIED_35 = VA_BIT64( 35 );
	static const uint64_t SOUND_RECEIVER_CHANGED_DIRECTIVITY = VA_BIT64( 36 );
	static const uint64_t MEASURES_UPDATE = VA_BIT64( 37 );
	static const uint64_t RESET = VA_BIT64( 38 );
	static const uint64_t SOUND_SOURCE_CHANGED_DIRECTIVITY = VA_BIT64( 39 );

	static const uint64_t DESTROY = VA_BIT64( 40 );

	static const uint64_t SOUND_SOURCE_REAL_WORLD_POSE_CHANGED = VA_BIT64( 41 );
	static const uint64_t SOUND_RECEIVER_REAL_WORLD_POSE_CHANGED = VA_BIT64( 42 );

	// Next free slot: 43

	//!< Selector mask for signal source related events
	static const uint64_t SIGNALSOURCE_EVENT = SIGNALSOURCE_CREATED |
		SIGNALSOURCE_DELETED |
		SIGNALSOURCE_REGISTERED |
		SIGNALSOURCE_UNREGISTERED |
		SIGNALSOURCE_STATE_CHANGED |
		SOUND_SOURCE_CHANGED_SIGNALSOURCE;

	//!< Selector mask for sound source related events
	static const uint64_t SOUNDSOURCE_EVENT = SOUND_SOURCE_CREATED |
		SOUND_SOURCE_DELETED |
		SOUND_SOURCE_CHANGED_NAME |
		SOUND_SOURCE_CHANGED_SIGNALSOURCE |
		SOUND_SOURCE_CHANGED_AURALIZATIONMODE |
		SOUND_SOURCE_CHANGED_SOUND_POWER |
		SOUND_SOURCE_CHANGED_MUTING |
		SOUND_SOURCE_CHANGED_POSE |
		SOUND_SOURCE_CHANGED_DIRECTIVITY;

	//!< Selector mask for sound receiver related events
	static const uint64_t SOUND_RECEIVER_EVENT = SOUND_RECEIVER_CREATED |
		SOUND_RECEIVER_DELETED |
		SOUND_RECEIVER_CHANGED_NAME |
		SOUND_RECEIVER_CHANGED_AURALIZATIONMODE |
		SOUND_RECEIVER_CHANGED_DIRECTIVITY |
		SOUND_RECEIVER_CHANGED_POSE;

	//!< Selector mask for portal related events
	static const uint64_t PORTAL_EVENT = SOUND_PORTAL_CHANGED_NAME |
		SOUND_PORTAL_CHANGED_PARAMETER;


	//!< Selector mask for sound source modification event
	static const uint64_t SOUNDSOURCE_MODIFICATION = SOUND_SOURCE_CHANGED_NAME |
		SOUND_SOURCE_CHANGED_SIGNALSOURCE |
		SOUND_SOURCE_CHANGED_AURALIZATIONMODE |
		SOUND_SOURCE_CHANGED_SOUND_POWER |
		SOUND_SOURCE_CHANGED_MUTING |
		SOUND_SOURCE_CHANGED_POSE |
		SOUND_SOURCE_CHANGED_DIRECTIVITY;

	//!< Selector mask for sound receiver modification event
	static const uint64_t SOUND_RECEIVER_MODIFICATION = SOUND_RECEIVER_CHANGED_NAME |
		SOUND_RECEIVER_CHANGED_AURALIZATIONMODE |
		SOUND_RECEIVER_CHANGED_DIRECTIVITY |
		SOUND_RECEIVER_CHANGED_POSE;


	IVAInterface* pSender;			//!< Sender interface
	int iEventID; //!< Event identifier
	uint64_t iEventType; //!< Event type (bit vector)
	int iObjectID;			//!< Integer-ID (Standard)
	std::string sObjectID;	//!< String-ID (z.B. für Signalquellen)
	int iParamID; //!< Parameter identifier
	std::string sParam; //!< Parameter string
	int iIndex; //!< Index value
	int iAuralizationMode; //!< Auralization mode
	double dVolume; //!< Volume, gain, sound power
	double dState; //!< Floating point state
	bool bMuted; //!< Mute flag
	std::string sName; //!< Name
	std::string sFilenPath; //!< File path
	VAVec3 vPos; //!< Position vector
	VAVec3 vView; //!< View vector
	VAVec3 vUp; //!< Up vector
	VAQuat qHATO; //!< Head-above-torso quaternion orientation
	VAQuat oOrientation; //!< Quaternion orientation
	std::vector< float > vfInputPeaks;	//!< Peak values of the input channels [factor]
	std::vector< float > vfInputRMSs;	//!< RMS values of the input channels [factor]
	std::vector< float > vfOutputPeaks;	//!< Peak values of the output channels [factor]
	std::vector< float > vfOutputRMSs;	//!< RMS values of the output channels [factor]
	float fSysLoad, fDSPLoad;			//!< System and DSP load [percent]
	double dCoreClock;					//!< Current value of the core clock [s]

	//!< Progress data
	CVAProgress oProgress;

	CVAStruct oPrototypeParams;

	//! Create empty event
	inline CVAEvent()
		: pSender( NULL )
		, iEventID( 0 )
		, iEventType( 0 )
		, iObjectID( -1 )
		, iParamID( -1 )
		, iIndex( -1 )
		, iAuralizationMode( 0 )
		, dVolume( 0 )
		, dState( 0 )
		, bMuted( false )
	{};

	//! Destroy event
	inline virtual ~CVAEvent() {};

	//! Convert to string
	/**
	  * @return String formatted event (for debugging output)
	  */
	virtual std::string ToString() const;
};


//! Event handler interface class
/**
 * This purely abstract class defines an interface to receive
 * core events, e.g. for a graphical user intface. Derive your
 * class and implement the handler. Attach your instance to the VA
 * class (also works with network connections, event will be transmitted).
 */
class VABASE_API IVAEventHandler
{
public:
	//! Destructor
	inline virtual ~IVAEventHandler() {};

	//! Event handler hook
	/**
	  * @param[in] pEvent Event pointer
	  */
	virtual void HandleVAEvent( const CVAEvent* pEvent ) = 0;
};

#endif // IW_VABASE_EVENT
