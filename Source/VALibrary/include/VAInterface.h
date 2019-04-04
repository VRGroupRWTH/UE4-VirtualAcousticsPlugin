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

#ifndef IW_VABASE_INTERFACE
#define IW_VABASE_INTERFACE

// VA includes
#include <VABaseDefinitions.h>
#include <VABase.h>
#include <VASamples.h>

// STL includes
#include <string>
#include <vector>

// Forward declarations
class VABASE_API CVAVersionInfo;
class VABASE_API CVAProgress;
class VABASE_API CVAStruct;
class VABASE_API IVAEventHandler;
class VABASE_API IVAAudioSignalSource;

//! Interface of Virtual Acoustics (VA)
/**
  * This mostly abstract interface to VA describes all relevant functions and methods
  * that have to be implemented to comply with a VA controller instance or core instance.
  *
  * The VACore will implement all these methods. The VANet library will wrap and transmit
  * methods and data classes to make the VA interface completely transparent through a network
  * connection using TCP/IP. All binding classes provide similar named methods in the script
  * language fashion and can use the networked connection or an internal core.
  *
  * There are a lot of prototyping methods using CVAStruct, an associative container for any
  * kind of value.
  *
  * Events can be propagated to event handlers (including network clients) and help to detect
  * core changes and updates.
  *
  */
class VABASE_API IVAInterface
{
public:
	//! Core states
	enum CoreState
	{
		VA_CORESTATE_CREATED = 0,	//!< Core was created, but is not yet initialized
		VA_CORESTATE_READY = 1,	//!< Core is successfully initialized and ready for use
		VA_CORESTATE_FAIL = -1,	//!< Core is corrupted and can not be recovered (trash state)
	};

	//! Core error levels
	enum ErrorLevel
	{
		VA_LOG_LEVEL_QUIET = 0, //!< Quiet log level (no outputs)
		VA_LOG_LEVEL_ERROR = 1, //!< Show errors ...
		VA_LOG_LEVEL_WARNING = 2, //!< ... and warnings ...
		VA_LOG_LEVEL_INFO = 3, //!< ... and infos ...
		VA_LOG_LEVEL_VERBOSE = 4, //!< ... and verbose messages ...
		VA_LOG_LEVEL_TRACE = 5, //!< ... and tracing outputs.
	};

	//! Playback states of audiofile signal sources
	enum PlaybackState
	{
		VA_PLAYBACK_STATE_INVALID = -1,	//!< Playback is stopped
		VA_PLAYBACK_STATE_STOPPED = 0,	//!< Playback is stopped
		VA_PLAYBACK_STATE_PAUSED = 1,	//!< Playback is paused
		VA_PLAYBACK_STATE_PLAYING = 2,	//!< Playback active
	};

	//! Playback actions (transitions) of audiofile signal sources
	enum PlaybackAction
	{
		VA_PLAYBACK_ACTION_NONE = -1,	//!< No action
		VA_PLAYBACK_ACTION_STOP = 0,	//!< Stop playback
		VA_PLAYBACK_ACTION_PAUSE = 1,	//!< Pause playback
		VA_PLAYBACK_ACTION_PLAY = 2,	//!< Start/resume playback	
	};

	//! Auralization modes (bit-vector flags)
	static const int VA_AURAMODE_NOTHING = 0;		//!< No auralization
	static const int VA_AURAMODE_DIRECT_SOUND = ( 1 << 0 );	//!< Direct sound
	static const int VA_AURAMODE_EARLY_REFLECTIONS = ( 1 << 1 );	//!< Early reflections
	static const int VA_AURAMODE_DIFFUSE_DECAY = ( 1 << 2 );	//!< Diffuse decay
	static const int VA_AURAMODE_SOURCE_DIRECTIVITY = ( 1 << 3 );	//!< Source directivity
	static const int VA_AURAMODE_MEDIUM_ABSORPTION = ( 1 << 4 );	//!< Absorption in air
	static const int VA_AURAMODE_TEMP_VAR = ( 1 << 5 );	//!< Atmospheric temporal variations
	static const int VA_AURAMODE_SCATTERING = ( 1 << 6 );	//!< Scattering 
	static const int VA_AURAMODE_DIFFRACTION = ( 1 << 7 );	//!< Diffraction
	static const int VA_AURAMODE_NEARFIELD = ( 1 << 8 );	//!< Near-field effects
	static const int VA_AURAMODE_DOPPLER = ( 1 << 9 );	//!< Doppler effects
	static const int VA_AURAMODE_SPREADING_LOSS = ( 1 << 10 );	//!< Spherical spreading loss
	static const int VA_AURAMODE_TRANSMISSION = ( 1 << 11 );	//!< Sound transmission
	static const int VA_AURAMODE_ABSORPTION = ( 1 << 12 );	//!< Material absorption
	static const int VA_AURAMODE_LAST = ( 1 << 13 ); //! For validation checking, increase for more modes!

	//! Default auralization mode (alias)
	static const int VA_AURAMODE_DEFAULT = VA_AURAMODE_DIRECT_SOUND |
		VA_AURAMODE_SOURCE_DIRECTIVITY |
		VA_AURAMODE_MEDIUM_ABSORPTION |
		VA_AURAMODE_DOPPLER |
		VA_AURAMODE_SPREADING_LOSS |
		VA_AURAMODE_TRANSMISSION |
		VA_AURAMODE_ABSORPTION;

	//! All auralization modes (mask)
	static const int VA_AURAMODE_ALL = VA_AURAMODE_DIRECT_SOUND |
		VA_AURAMODE_EARLY_REFLECTIONS |
		VA_AURAMODE_DIFFUSE_DECAY |
		VA_AURAMODE_SOURCE_DIRECTIVITY |
		VA_AURAMODE_MEDIUM_ABSORPTION |
		VA_AURAMODE_TEMP_VAR |
		VA_AURAMODE_SCATTERING |
		VA_AURAMODE_DIFFRACTION |
		VA_AURAMODE_NEARFIELD |
		VA_AURAMODE_DOPPLER |
		VA_AURAMODE_SPREADING_LOSS |
		VA_AURAMODE_TRANSMISSION |
		VA_AURAMODE_ABSORPTION;


	//! Destructor
	/**
	 * @note If the core was not finialized until the time of its destruction,
	 *       the destructor will attempt to implicitly finalize it here.
	 *       Remember that you should always finialize a core, so that you can
	 *       track errors.
	 */
	virtual ~IVAInterface();


	//! Returns the core version
	/**
	  * @param [out] pVersionInfo Core version information
	  */
	virtual void GetVersionInfo( CVAVersionInfo* pVersionInfo ) const = 0;

	//! Set the output stream for debug messages
	/**
	  * @param [in] posDebug Debug output stream
	  */
	virtual void SetOutputStream( std::ostream* posDebug ) = 0;


	//! Returns the state of the core
	/**
	  * @return Core state
	  */
	virtual int GetState() const = 0;

	//! Initializes the core
	/**
	 * This method initializes the core, making it ready for use.
	 *
	 * If the method succeeds, the core is ready to use and within
	 * the state VA_CORESTATE_READY. Otherwise a CVAException is thrown,
	 * describing the cause for that the initialization failed. In this
	 * case the core remains in the state VA_CORESTATE_CREATED.
	 * If the initialized failed, the instance remains tidy.
	 * It is not necessary to clean it up by calling Finalize.
	 *
	 * Since the initialization can take a longer time, involving many operations,
	 * progress events are send to all attached event handlers, informing about
	 * the current status. This happens during the call to the method.
	 *
	 * @note Initialization does not detach event handlers
	 */
	virtual void Initialize() = 0;

	//! Finalizes the core and frees allocated resources
	/**
	 * This method finalizes an initialized core.
	 * Thereby all its allocated resources are freed.
	 * Afterwards the core is in the state VA_CORESTATE_CREATED.
	 * It can then be reinitialized again.
	 *
	 * Finalization should not fail in general.
	 * If for some reason it does, the core cannot be
	 * used anymore and is within the VA_CORESTATE_FAIL.
	 * It shall be destroyed in this case.
	 *
	 * @note Finalization does not detach event handlers
	 */
	virtual void Finalize() = 0;

	//! Full reset of the core. Clears all scene objects, resources, etc.
	/**
	 * Clear all scenes, removes sound entities, resets states and medium values
	 */
	virtual void Reset() = 0;


	//! Attaches a handler for core events to the core instance
	/**
	* @param[in] pEventHandler Event handler pointer
	 * @note Attaching event handlers it always possible,
	 *            regardless of the state of the core.
	 *            This method can therefore be called anytime,
	 *            also before initialization and after finalization.
	 */
	virtual void AttachEventHandler( IVAEventHandler* pEventHandler ) = 0;

	//! Detaches a handler for core events from the core instance
	/**
	 * @param[in] pEventHandler Event handler pointer
	 * @note Detaching event handlers it always possible,
	 *            regardless of the state of the core.
	 *            This method can therefore be called anytime,
	 *            also before initialization and after finalization.
	 *
	 * @note When the method returns, it is guaranteed that the handler will not
	 *       receive any further core events and may safely be destroyed.
	 */
	virtual void DetachEventHandler( IVAEventHandler* pEventHandler ) = 0;


	//! Get all registered modules of the core
	/**
	  * @param[out] voModuleInfos Vector with module infos
	  */
	virtual void GetModules( std::vector< CVAModuleInfo >& voModuleInfos ) const = 0;

	//! Calls a module and returns the answer
	/**
	  * This method is useful for prototyping. There is no general documentation possible. Special calls have to be extracted from C++ code
	  * in core. It is recommended to implement at least a help text that will be returned if the 'help' or 'info' key is set to any value.
	  * Also, a true-valued 'verbose' key flag should activate verbose return or output on debug stream.
	  *
	  * @param[in] sModuleName Full qualifier of module identifier (may include ':' separator)
	  * @param[in] oArgs Magic struct that is interpreted by module
	  *
	  * @return Magic return struct
	  */
	virtual CVAStruct CallModule( const std::string& sModuleName, const CVAStruct& oArgs ) = 0;


	//! Adds a search path to the core instance
	/**
	  * @param[in]	sPath	Local relative or absolute path
	  * @return	True, if path is valid at core side
	  */
	virtual bool AddSearchPath( const std::string& sPath );

	//! Returns a struct with entries for each search paths
	/**
	  * @return	Encapsulated search paths
	  */
	virtual CVAStruct GetSearchPaths() const = 0;

	//! Returns files that are accessible through search paths
	/**
	  * @param[in]	bRecursive	Search recursively in paths
	  * @param[in]	sFileSuffixFilter	File suffix / type filtering, i.e. "wav" or "daff". "*" or empty string will list all.
	  *
	  * @return	Encapsulated file paths
	  *
	  * @warning This call may take a while and can result in a lot of return data.
	  */
	virtual CVAStruct GetFileList( const bool bRecursive = true, const std::string& sFileSuffixFilter = "*" ) const = 0;

	//! Returns a struct with the core configuration
	/**
	* @param[in] bFilterEnabled If true, only include enabled sections.
	*
	* @return	Encapsulated core configuration (as used to load/store and work in core)
	*/
	virtual CVAStruct GetCoreConfiguration( const bool bFilterEnabled = true ) const = 0;
	
	//! Returns a struct with the hardware configuration
	/**
	  * @return	Encapsulated hardware configuration
	  */
	virtual CVAStruct GetHardwareConfiguration() const = 0;

	//! Create a directivity from a set of parameters	
	/**
	  * @param[in] oParams Parameters
	  * @param[in] sName Versatile name
	  *
	  * @return	Directivity identifier
	  */
	virtual int CreateDirectivityFromParameters( const CVAStruct& oParams, const std::string& sName = "" ) = 0;

	//! Create a directivity from a file path
	/**
	  * @param[in] sFilePath File path (relative or absolute or macro)
	  * @param[in] sName Versatile name
	  *
	  * @return	Directivity identifier
	  */
	inline int CreateDirectivityFromFile( const std::string& sFilePath, const std::string& sName = "" )
	{
		CVAStruct oParams;
		oParams[ "filepath" ] = sFilePath;
		return CreateDirectivityFromParameters( oParams, sName );
	};

	//! Delete a directivity
	/**
	  * @param[in] iID  Identifier
	  *
	  * @return True, if directivity was found and could be released (was not in use)
	  */
	virtual bool DeleteDirectivity( const int iID ) = 0;

	//! Directivity info getter
	/**
	  * @param[in] iID  Identifier
  	  *
	  * @return Directivity information
	  */
	virtual CVADirectivityInfo GetDirectivityInfo( const int iID ) const = 0;
	
	//! Directivity info getter
	/**
	  * @param[out] voDest  Directivity information vector
	  */
	virtual void GetDirectivityInfos( std::vector< CVADirectivityInfo >& voDest ) const = 0;

	//! Directivity name setter
	/**
	  * @param[in] iID  Identifier
	  * @param[in] sName  Versatile name
	  */
	virtual void SetDirectivityName( const int iID, const std::string& sName ) = 0;
	
	//! Directivity name getter
	/**
	  * @param[in] iID  Identifier
	  * @return Versatile name
	  */
	virtual std::string GetDirectivityName( const int iID ) const = 0;
	

	//! Directivity parameter setter
	/**
	  * @param[in] iID  Identifier
	  * @param[in] oParams  Parameters
	  */
	virtual void SetDirectivityParameters( const int iID, const CVAStruct& oParams ) = 0;
	
	//! Directivity parameter getter
	/**
	  * @param[in] iID  Identifier
	  * @param[in] oParams  Parameters
	  * @return Parameters
	  */
	virtual CVAStruct GetDirectivityParameters( const int iID, const CVAStruct& oParams ) const = 0;

	
	//! Create acoustic material by passing material data
	/**
	  * @param[in] oMaterial  Material data
	  * @param[in] sName  Verbatim name
	  * @return Identifier
	  */
	virtual int CreateAcousticMaterial( const CVAAcousticMaterial& oMaterial, const std::string& sName = "" ) = 0;
	
	//! Create acoustic material based on magic parameter set (for prototyping)
	/**
	  * @param[in] oParams  Material parameters
	  * @param[in] sName  Verbatim name
	  * @return Identifier
	  */
	virtual int CreateAcousticMaterialFromParameters( const CVAStruct& oParams, const std::string& sName = "" ) = 0;
	
	//! Create acoustic material from file path (e.g. 'mat' file)
	/**
	  * @param[in] sFilePath  Material file path
	  * @param[in] sName  Verbatim name
	  * @return Identifier
	  */
	inline int CreateAcousticMaterialFromFile( const std::string& sFilePath, const std::string& sName = "" )
	{
		CVAStruct oParams;
		oParams[ "filepath" ] = sFilePath;
		return CreateAcousticMaterialFromParameters( oParams, sName );
	};
	
	//! Create acoustic material based on magic parameter set (for prototyping)
	/**
	  * @param[in] iID  Material identifier
	  * @return True, if material could be removed
	  */
	virtual bool DeleteAcousticMaterial( const int iID ) = 0;	
	
	//! Acoustic material info getter
	/**
	  * @param[in] iID  Material identifier
	  * @return Info on material
	  */
	virtual CVAAcousticMaterial GetAcousticMaterialInfo( const int iID ) const = 0;
		
	//! Acoustic material info getter for entire database
	/**
	  * @param[out] voDest  Vector with material infos
	  */
	virtual void GetAcousticMaterialInfos( std::vector< CVAAcousticMaterial >& voDest ) const = 0;	

	//! Acoustic material name getter
	/**
	  * @param[in] iID  Material identifier
	  * @return  Verbatim name
	  */
	virtual std::string GetAcousticMaterialName( const int iID ) const = 0;

	//! Acoustic material name setter
	/**
	  * @param[in] iID  Material identifier
	  * @param[in] sName  Verbatim name
	  */
	virtual void SetAcousticMaterialName( const int iID, const std::string& sName ) = 0;
	
	//! Acoustic material name getter
	/**
	  * @param[in] iID  Material identifier
	  * @param[in] oParams Material parameter request arguments
	  * @return  Material parameters
	  */
	virtual CVAStruct GetAcousticMaterialParameters( const int iID, const CVAStruct& oParams ) const = 0;
	
	//! Acoustic material parameter setter
	/**
	  * @param[in] iID  Material identifier
	  * @param[in] oParams Material parameters
	  */
	virtual void SetAcousticMaterialParameters( const int iID, const CVAStruct& oParams ) = 0;


	//! Create a geometry mesh with mesh infos
	/**
	  * @param[in] oMesh Geometry mesh information
	  * @param[in] sName Versatile name
	  *
	  * @return Geometry mesh identifier
	  */
	virtual int CreateGeometryMesh( const CVAGeometryMesh& oMesh, const std::string& sName = "" ) = 0;
	
	//! Create a geometry mesh from parameters
	/**
	  * @param[in] oParams Geometry mesh parameters
	  * @param[in] sName Versatile name
	  *
	  * @return Geometry mesh identifier
	  */
	virtual int CreateGeometryMeshFromParameters( const CVAStruct& oParams, const std::string& sName = "" ) = 0;
	
	//! Create a geometry mesh from file
	/**
	  * @param[in] sFilePath Geometry mesh file path (relative or absolute or macro)
	  * @param[in] sName Versatile name
	  *
	  * @return Geometry mesh identifier
	  */
	inline int CreateGeometryMeshFromFile( const std::string& sFilePath, const std::string& sName = "" )
	{
		CVAStruct oParams;
		oParams[ "filepath" ] = sFilePath;
		return CreateGeometryMeshFromParameters( oParams, sName );
	};
	
	//! Delete a geometry mesh
	/**
	  * @param[in] iID Geometry mesh identifier
	  *
	  * @return True if mesh could be removed
	  */
	virtual bool DeleteGeometryMesh( const int iID ) = 0;
	
	//! Get a geometry mesh from identifeir
	/**
	  * @param[in] iID Geometry mesh identifier
	  *
	  * @return Geometry mesh
	  */
	virtual CVAGeometryMesh GetGeometryMesh( const int iID ) const = 0;
	
	//! Get geometry mesh ids
	/**
	  * @param[out] viIDs All available geometry mesh identifiers
	  *
	  */
	virtual void GetGeometryMeshIDs( std::vector< int >& viIDs ) const = 0;
	
	//! Delete a geometry mesh
	/**
	  * @param[in] iID Geometry mesh identifier
	  * @param[in] sName Geometry mesh verbatim name
	  *
	  * @return True if mesh could be removed
	  */
	virtual void SetGeometryMeshName( const int iID, const std::string& sName ) = 0;
	
	//! Geometry mesh name getter
	/**
	  * @param[in] iID Geometry mesh identifier
	  *
	  * @return Verbatim name
	  */
	virtual std::string GetGeometryMeshName( const int iID ) const = 0;
	
	//! Geometry mesh parameter setter
	/**
	  * @param[in] iID Geometry mesh identifier
	  * @param[in] oParams Geometry mesh magic parameters
	  *
	  * @return True if mesh could be removed
	  */
	virtual void SetGeometryMeshParameters( const int iID, const CVAStruct& oParams ) = 0;
	
	//! Geometry mesh parameter getter
	/**
	  * @param[in] iID Geometry mesh identifier
	  * @param[in] oParams Geometry mesh magic parameter request
	  *
	  * @return Magic parameters
	  */
	virtual CVAStruct GetGeometryMeshParameters( const int iID, const CVAStruct& oParams ) const = 0;
	
	//! Geometry mesh enabled setter
	/**
	  * @param[in] iID Geometry mesh identifier
	  * @param[in] bEnabled If true, sets enabled, if false geo mesh is disabled
	  */
	virtual void SetGeometryMeshEnabled( const int iID, const bool bEnabled = true ) = 0;
	
	//! Geometry mesh enabled getter
	/**
	  * @param[in] iID Geometry mesh identifier
	  *
	  * @return True if mesh is enabled, false otherwise
	  */
	virtual bool GetGeometryMeshEnabled( const int iID ) const = 0;


	//! Creates a buffer signal source from an audio file
	/**
	  * Creates a signal source which streams the samples of an audiofile.
  	  *
	  * @param[in] sFilePath Filename, file path, macro, ...
	  * @param[in] sName		Name (optional, e.g. "Trumpet")
  	  *
	  * @return Signal source ID
	  */
	inline std::string CreateSignalSourceBufferFromFile( const std::string& sFilePath, const std::string& sName = "" )
	{
		CVAStruct oParams;
		oParams[ "filepath" ] = sFilePath;
		return CreateSignalSourceBufferFromParameters( oParams, sName );
	};

	//! Creates a mono buffer signal source from sample buffer
	/**
	  * Creates a signal source which streams the samples of a buffer.
	  *
	  * @param[in] oSamples Buffer samples
	  * @param[in] sName		Name (optional, e.g. "Trumpet")
	  *
	  * @return Signal source ID
	  */
	virtual std::string CreateSignalSourceBufferFromSamples( const CVASampleBuffer& oSamples, const std::string& sName = "" )
	{
		CVAStruct oParams;
		oParams[ "samples" ][ "ch1" ] = oSamples;
		return CreateSignalSourceBufferFromParameters( oParams, sName );
	};

	//! Creates a multi-channel buffer signal source from sample buffer
	/**
	  * Creates a signal source which streams the samples of a buffer with more than one channel.
	  * Useful for prototyping, i.e. for switching channels during playback or mixing.
	  *
	  * @param[in] voSamples Buffer sample vector
	  * @param[in] sName		Name (optional, e.g. "Trumpet")
  	  *
	  * @return Signal source ID
	  */
	inline std::string CreateSignalSourceBufferMultichannelFromSamples( const std::vector< CVASampleBuffer >& voSamples, const std::string& sName = "" )
	{
		CVAStruct oParams;
		for( size_t i = 0; i < voSamples.size(); i++ )
			oParams[ "samples" ][ "ch" + std::to_string( long( i + 1 ) ) ] = voSamples[ i ];
		return CreateSignalSourceBufferFromParameters( oParams, sName );
	};

	//! Creates a buffer signal source from parameters
	/**
	  * Creates a signal source which streams the samples of a buffer that is created based on magic parameters.
	  * Useful for prototyping, i.e. if a special signal source type is required and no interface change should be performed.
	  *
	  * @param[in] oParams Buffer signal source magic parameters
	  * @param[in] sName		Name (optional, e.g. "Prototype Trumpet")
  	  *
	  * @return Signal source ID
	  */
	virtual std::string CreateSignalSourceBufferFromParameters( const CVAStruct& oParams, const std::string& sName = "" ) = 0;

	//! Creates a text-to-speech (TTS) signal source
	/**
	  * Creates a signal source which streams generated samples from a text input.
	  *
	  * @param[in] sName		Name (optional, e.g. "Penny")
	  *
	  * @return Signal source ID
	  */
	virtual std::string CreateSignalSourceTextToSpeech( const std::string& sName = "" ) = 0;

	//! Creates a sequencer signal source
	/**
	 * Creates a sequencer signal source.
	 *
	 * @param[in] sName	Name (optional, e.g. "Sequencer 1")
	 *
	 * @return Signal source ID
	 */
	virtual std::string CreateSignalSourceSequencer( const std::string& sName = "" ) = 0;

	//! Creates a network-based signal source
	/**
	  * Creates a signal source which receives audio samples via network.
	  * Therefore an IP socket is set up with the given parameters.
	  *
	  * @param[in] sInterface  IP address of the socket
	  * @param[in] iPort       Destination port on which samples are received
	  * @param[in] sName	Optional name
	  *
	  * @note Usual port range is 12480 to 12500
	  *
	  * @return ID of signal source
	  */
	virtual std::string CreateSignalSourceNetworkStream( const std::string& sInterface, const int iPort, const std::string& sName = "" ) = 0;

	//! Creates a rotating engine source
	/**
	  * @param[in] oParams  Parameters
	  * @param[in] sName Name of the engine
	  * @return ID of signal source
	  */
	virtual std::string CreateSignalSourceEngine( const CVAStruct& oParams, const std::string& sName = "" ) = 0;

	//! Creates a machine signal source
	/**
	  * @param[in] oParams  Parameters
	  * @param[in] sName Name of the machine
	  *@return Signal source identifier
	  */
	virtual std::string CreateSignalSourceMachine( const CVAStruct& oParams, const std::string& sName = "" ) = 0;

	//! Deletes a signal source
	/**
	  * Deletes a signal source. This is only possible if it is not in use.
	  *
	  * @return True, if the signal source has been deleted, false otherwise
	  */
	virtual bool DeleteSignalSource( const std::string& sID ) = 0;

	//! Registers an (external) signal source with the core
	/**
	  * This method registers a signal source which is not created and managed
	  * by the core. The core assignes it an ID, which makes it usable for
	  * auralization.
	  *
	  * @param[in] pSignalSource	Signal source instance
	  * @param[in] sName		Name (optional, z.B. "My tone generator")
	  *
	  * @return Signal source ID
	  *
	  * @note This function is only locally available, but not for remote connections
	  */
	virtual std::string RegisterSignalSource( IVAAudioSignalSource* pSignalSource, const std::string& sName = "" ) = 0;

	//! Unregisters an (external) signal source from the core
	/**
	  * Unregisters an (external) signal source from the core.
	  * This is only possible, if the signal source is not in use.
	  *
	  * @param[in] pSignalSource	Signal source instance
	  *
	  * @return true, if the signal source has been unregistered, false otherwise
	  *
	  * @note This function is only locally available, but not remote
	  */
	virtual bool UnregisterSignalSource( IVAAudioSignalSource* pSignalSource ) = 0;

	//! Retrieves information of a signal source
	/**
	  * @param[in] sSignalSourceID Signal source identifier
	  * @return Signal source information
	  */
	virtual CVASignalSourceInfo GetSignalSourceInfo( const std::string& sSignalSourceID ) const = 0;

	//! Retrieves information of all signal sources
	/**
	  * @param[in] voInfos Signal source info vector
	  */
	virtual void GetSignalSourceInfos( std::vector< CVASignalSourceInfo >& voInfos ) const = 0;

	//! Returns the playback state flag (bit-vector) of an audiofile signal source
	/**
	  * @param[in] sSignalSourceID Signal source identifier
	  * @return Playback state integer
	  */
	virtual int GetSignalSourceBufferPlaybackState( const std::string& sSignalSourceID ) const = 0;

	//! Set the playback state of an audiofile signal source
	/**
	  * Start, stop or pause the playback of an audiofile using the playback action flags (bit-vector).
	  *
	  * @param[in] sSignalSourceID Signal source identifier
	  * @param[in] iPlaybackAction Playback action integer
	  */
	virtual void SetSignalSourceBufferPlaybackAction( const std::string& sSignalSourceID, const int iPlaybackAction ) = 0;

	//! Set the playback position of an audiofile signal source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @param[in] dPlaybackPosition		Playback position [s]
	  */
	virtual void SetSignalSourceBufferPlaybackPosition( const std::string& sSignalSourceID, const double dPlaybackPosition ) = 0;

	//! Set playback looping mode (true = looping)
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @param[in] bLooping				Playback is looping, if set true
	  */
	virtual void SetSignalSourceBufferLooping( const std::string& sSignalSourceID, const bool bLooping = true ) = 0;

	//! Get playback looping mode (true = looping)
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @return True, if looping is enabled
	  */
	virtual bool GetSignalSourceBufferLooping( const std::string& sSignalSourceID ) const = 0;

	//! Starts the signal of a machine source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  */
	inline void SetSignalSourceMachineStartMachine( const std::string& sSignalSourceID )
	{
		CVAStruct oParams;
		oParams[ "set" ] = "action";
		oParams[ "value" ] = "start";
		SetSignalSourceParameters( sSignalSourceID, oParams );
	};

	//! Halts the signal of a machine source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  */
	inline void SetSignalSourceMachineHaltMachine( const std::string& sSignalSourceID )
	{
		CVAStruct oParams;
		oParams[ "set" ] = "action";
		oParams[ "value" ] = "stop";
		SetSignalSourceParameters( sSignalSourceID, oParams );
	};

	//! Returns the state of a machine signal source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @return						Machine state as string
	  */
	inline std::string GetSignalSourceMachineStateStr( const std::string& sSignalSourceID ) const
	{
		CVAStruct oParams, oRet;
		oParams[ "get" ] = "state";
		oRet = GetSignalSourceParameters( sSignalSourceID, oParams );
		if( oRet.HasKey( "state" ) )
			if( oRet[ "state" ].IsString() )
				return oRet[ "state" ].ToString();
			else
				return "error";
		else
			return "unkown";
	};

	//! Sets the speed of a machine signal source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @param[in] dSpeed				Machine speed (0 .. not-too-large)
	  */
	inline void SetSignalSourceMachineSpeed( const std::string& sSignalSourceID, double dSpeed )
	{
		CVAStruct oParams;
		oParams[ "set" ] = "S";
		oParams[ "value" ] = dSpeed;
		SetSignalSourceParameters( sSignalSourceID, oParams );
	};

	//! Sets the speed of a machine signal source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @return						Machine speed
	  */
	inline double GetSignalSourceMachineSpeed( const std::string& sSignalSourceID ) const
	{
		CVAStruct oParams, oRet;
		oParams[ "get" ] = "speed";
		oRet = GetSignalSourceParameters( sSignalSourceID, oParams );
		if( oRet.HasKey( "speed" ) )
			if( oRet[ "speed" ].IsNumeric() )
				return double( oRet[ "speed" ] );

		return 1.0f;
	};

	//! Set start file sample of machine signal source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @param[in] sFilePath				Path to audio file
	  */
	inline void SetSignalSourceMachineStartFile( const std::string& sSignalSourceID, const std::string& sFilePath )
	{
		CVAStruct oParams;
		oParams[ "set" ] = "StartSoundFilePath";
		oParams[ "value" ] = sFilePath;
		SetSignalSourceParameters( sSignalSourceID, oParams );
	};

	//! Set idle file sample of machine signal source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @param[in] sFilePath				Path to audio file
	  */
	inline void SetSignalSourceMachineIdleFile( const std::string& sSignalSourceID, const std::string& sFilePath )
	{
		CVAStruct oParams;
		oParams[ "set" ] = "IdleSoundFilePath";
		oParams[ "value" ] = sFilePath;
		SetSignalSourceParameters( sSignalSourceID, oParams );
	};

	//! Set stop file sample of machine signal source
	/**
	  * @param[in] sSignalSourceID		Signal source ID
	  * @param[in] sFilePath				Path to audio file
	  */
	inline void SetSignalSourceMachineStopFile( const std::string& sSignalSourceID, const std::string& sFilePath )
	{
		CVAStruct oParams;
		oParams[ "set" ] = "StopSoundFilePath";
		oParams[ "value" ] = sFilePath;
		SetSignalSourceParameters( sSignalSourceID, oParams );
	};
	
	//! Set parameters for a signal source
	/**
	  * This method sets parameters of a signal source. Behavior depends on type and
	  * implementation of the referred instance.
	  *
	  * @param[in] sSignalSourceID	ID of signal source
	  * @param[in] oParams	Parameter structure
	  */
	virtual void SetSignalSourceParameters( const std::string& sSignalSourceID, const CVAStruct& oParams ) = 0;

	//! Get parameters from a signal source
	/**
	  * This method returns parameters of a signal source. Behavior depends on type and
	  * implementation of the referred instance.
	  *
	  * @param[in] sSignalSourceID	ID of signal source
	  * @param[in] oParams	Parameter request structure
	  * @return	Parameter structure
	  */
	virtual CVAStruct GetSignalSourceParameters( const std::string& sSignalSourceID, const CVAStruct& oParams ) const = 0;

	//! Add sequencer sample from file path
	/**
	  * @param[in] sSignalSourceID Signal source identifier
	  * @param[in] sFilePath Sample audio file path
	  * @return Sample identifier
	  */
	inline int AddSignalSourceSequencerSample( const std::string& sSignalSourceID, const std::string sFilePath )
	{
		CVAStruct oArgs;
		oArgs[ "filepath" ] = sFilePath;
		return AddSignalSourceSequencerSample( sSignalSourceID, oArgs );
	};

	//! Add sequencer sample
	/**
	  * @param[in] sSignalSourceID Signal source identifier
	  * @param[in] oArgs Sample audio file parameters
	  * @return Sample identifier
	  */
	virtual int AddSignalSourceSequencerSample( const std::string& sSignalSourceID, const CVAStruct& oArgs ) = 0;

	//! Add sequencer sound playback
	/**
	  * @param[in] sSignalSourceID Signal source identifier
	  * @param[in] iSoundID Sound sampler identifier
	  * @param[in] iFlags Sampler playback start flags
	  * @param[in] dTimeCode Time code
	  * @return Sample playback identifier
	  */
	virtual int AddSignalSourceSequencerPlayback( const std::string& sSignalSourceID, const int iSoundID, const int iFlags, const double dTimeCode ) = 0;

	//! Remove sound sample (a sample added for playback can not be reverted)
	/**
	  * @param[in] sSignalSourceID Signal source identifier
	  * @param[in] iSoundID Sound sample identifier
	  */
	virtual void RemoveSignalSourceSequencerSample( const std::string& sSignalSourceID, const int iSoundID ) = 0;


	//! Returns wheather a synchronized scene modification is in progress
	/**
	  * @return True, if scene update is locked
	  */
	virtual bool GetUpdateLocked() const = 0;

	//! Begins a synchronized scene modification
	/**
	  * Receives for the calling thread the exclusive scene modification tocken.
	  * Stays in possesion until UnlockUpdate is called.
	  */
	virtual void LockUpdate() = 0;

	//! Ends a synchronized scene modification and applies all changes
	/**
	  * Releases update token. Triggers scene lock and creates identifier.
	  *
	  * @return State identifier of current scene on success, -1 otherwiese
	  */
	virtual int UnlockUpdate() = 0;


	//! Create a sound source
	/**
	  * This method creates a new sound source and returns its ID.
	  * A signal source is not created and assigned to the sound source.
	  *
	  * @param[in] sName					Name (optional)
	  *
	  * @return ID of the new sound source, if the method succeeded, -1, otherwise
	  */
	virtual int CreateSoundSource( const std::string& sName = "" ) = 0;

	//! Return all sound source IDs
	/**
	  * @param[out] viSoundSourceIDs Sound source identifiers
	  */
	virtual void GetSoundSourceIDs( std::vector< int >& viSoundSourceIDs ) = 0;
	
	//! Return sound source info
	/**
	  * @param[in] iID Sound source identifier
	  * @return Sound source information
	  */
	virtual CVASoundSourceInfo GetSoundSourceInfo( const int iID ) const = 0;

	//! Create a sound source explicitly for a certain renderer
	/**
	  * This method creates a new sound source and returns its ID.
	  * This sound source will be skipped for rendering from any other
	  * but the given renderer. See GetRenderingModules() to get a list
	  * of available renderers and their name identifier.
	  *
	  * @param[in]	sRendererID		Identifier of target renderer
	  * @param[in]	sName			Name
	  *
	  * @return	Numerical identifier of the new sound source on success, -1 otherwise
	  *
	  * @note	Throws VA exception
	  */
	virtual int CreateSoundSourceExplicitRenderer( const std::string& sRendererID, const std::string& sName = "" ) = 0;

	//! Removes a sound source
	/**
	  * @param[in] iID Sound source identifier
	  * @return 0 on success, -1 otherwise
	  */
	virtual int DeleteSoundSource( const int iID ) = 0;

	//! Enables or disables a sound source (removes the sound source from processing within renderers)
	/**
	  * @param[in] iID	Sound source identifier
	  * @param[in] bEnabled			Enable (true) or disable (false) sound source
	  *
	  * @sa GetSoundSourceEnabled()
	  */
	virtual void SetSoundSourceEnabled( const int iID, const bool bEnabled = true ) = 0;

	//! Returns sound source enabled/disabled status
	/**
	  * @param[in] iID	Sound source identifier
	  * @return					Enabled (true) or disabled (false)
	  *
	  * @sa SetSoundSourceEnabled()
	  */
	virtual bool GetSoundSourceEnabled( const int iID ) const = 0;

	//! Returns name of a sound source
	/**
	  * @param[in] iID	Sound source identifier
	  * @return Verbatim name
 	  *
	  */
	virtual std::string GetSoundSourceName( const int iID ) const = 0;

	//! Sound source name setter
	/**
	  * @param[in] iID	Sound source identifier
	  * @param[in] sName Verbatim name
	  *
	  */
	virtual void SetSoundSourceName( const int iID, const std::string& sName ) = 0;
	
	//! Returns signal source identifier of a sound source
	/**
	  * @param[in] iID	Sound source identifier
	  * @return Signal source identifier, or -1 if not set
 	  *
	  */
	virtual std::string GetSoundSourceSignalSource( const int iID ) const = 0;
	
	//! Sound source geo mesh identifier getter
	/**
	  * @param[in] iID	Sound source identifier
	  * @return Geo mesh identifier
	  *
	  */
	virtual int GetSoundSourceGeometryMesh( const int iID ) const = 0;
	
	//! Sound source geo mesh identifier getter
	/**
	  * Sound sources can have geometrical meshes assigned, i.e. to identify which geometry is part
	  * of a sound source that should not be considered for direct path occlusion - or if the
	  * moving source geometry is relevant for occlusion of other source-receivere-paths, like
	  * cars, trains, etc.
	  *
	  * @param[in] iSoundReceiverID	Sound source identifier
	  * @param[in] iGeometryMeshID Geo mesh identifier
	  *
	  */
	virtual void SetSoundSourceGeometryMesh( const int iSoundReceiverID, const int iGeometryMeshID ) = 0;

	//! Attach a signal source to sound source
	/**
	  * Let a sound source play back samples from the given signal source.
	  * An empty string removes the signal source from sound source (silence).
	  *
	  * @sa RemoveSoundSourceSignalSource
	  *
	  * @param[in] iID	Sound source identifier
	  * @param[in] sSignalSourceID	Signal source identifier
	  *
	  */
	virtual void SetSoundSourceSignalSource( const int iID, const std::string& sSignalSourceID ) = 0;

	//! Detach signal source from a sound sourve
	/**
	  * Detaches a connected signal source from a sound source. Signal source will not be removed.
	  *
	  * @param[in] iID	Sound source identifier
	  */
	inline void RemoveSoundSourceSignalSource( const int iID )
	{
		SetSoundSourceSignalSource( iID, "" );
	};
	
	//! Sound source auralization mode getter
	/**
	  * Set an individual auralization mode for a given sound source.
	  *
	  * @param[in] iID Sound source identifier
	  * @return Auralizaion mode bit-vector
	  */
	virtual int GetSoundSourceAuralizationMode( const int iID ) const = 0;

	//! Sound source auralization mode setter
	/**
	  * Set an individual auralization mode for a given sound source.
	  *
	  * @param[in] iID Sound source identifier
	  * @param[in] iAuralizationMode Auralizaion mode bit-vector
	  */
	virtual void SetSoundSourceAuralizationMode( const int iID, const int iAuralizationMode ) = 0;

	//! Set sound source parameters
	/**
	  * This general parameter setter can be used for quick solutions changing
	  * sound source parameters without introducing new methods to the main interface.
	  *
	  * @param[in] iID Sound source identifier
	  * @param[in] oParams Magic struct with the parameters
	  */
	virtual void SetSoundSourceParameters( const int iID, const CVAStruct& oParams ) = 0;

	//! Get sound source parameters
	/**
	  * This general parameter getter can be used for quick solutions retrieving
	  * sound receiver parameters without introducing new methods to the main interface.
	  *
	  * @param[in] iID Sound source identifier
	  * @param[in] oParams Magic struct with the parameters that are requested
	  * @return Magic struct with the parameter values requested
	  */
	virtual CVAStruct GetSoundSourceParameters( const int iID, const CVAStruct& oParams ) const = 0;

	//! Returns the directivity of a sound source
	/**
	  * If the sound source is not assigned a directivity, the methods returns -1.
	  * @param[in] iID Sound source identifier
	  * @return Directivity identifier
	  */
	virtual int GetSoundSourceDirectivity( const int iID ) const = 0;

	//! Sets the directivity of a sound source
	/**
	  * In order to remove an assigned directivity, you can pass -1 to the method.
	  * @param[in] iSoundReceiverID Sound source identifier
	  * @param[in] iDirectivityID Directivity identifier
	  */
	virtual void SetSoundSourceDirectivity( const int iSoundReceiverID, const int iDirectivityID ) = 0;

	//! Sound receiver sound power getter
	/**
	  * @param[in] iID Sound source identifier
	  * @return Sound power in Watts [W]
	  */
	virtual double GetSoundSourceSoundPower( const int iID ) const = 0;

	//! Sound receiver sound power setter
	/**
	 * @param[in] iID Sound source identifier
	 * @param[in] dSoundPower Sound power in Watts [W]
	 */
	virtual void SetSoundSourceSoundPower( const int iID, const double dSoundPower ) = 0;

	//! Sound source enabled getter
	/**
	  * @param[in] iID Sound source identifier
	  * @return True, if muted
	  */
	virtual bool GetSoundSourceMuted( const int iID ) const = 0;

	//! Sound source enabled setter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[in] bMuted Mutes sound source, if true
	  */
	virtual void SetSoundSourceMuted( const int iID, const bool bMuted = true ) = 0;
	
	//! Sound source pose getter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[out] v3Pos Position vector
	  * @param[out] qOrient Orientation quaternion
	  */
	virtual void GetSoundSourcePose( const int iID, VAVec3& v3Pos, VAQuat& qOrient ) const = 0;
	
	//! Sound source pose setter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[in] v3Pos Position vector
	  * @param[in] qOrient Orientation quaternion
	  */
	virtual void SetSoundSourcePose( const int iID, const VAVec3& v3Pos, const VAQuat& qOrient ) = 0;
	
	//! Sound source position getter
	/**
	  * @param[in] iID Sound source identifier
	  * @return Position vector
	  */
	virtual VAVec3 GetSoundSourcePosition( const int iID ) const = 0;
	
	//! Sound source position setter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[in] v3Pos Position vector
	  */
	virtual void SetSoundSourcePosition( const int iID, const VAVec3& v3Pos ) = 0;
	
	//! Sound source orientation getter
	/**
	  * @param[in] iID Sound source identifier
	  * @return Orientation quaternion
	  */
	virtual VAQuat GetSoundSourceOrientation( const int iID ) const = 0;
	
	//! Sound source orientation setter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[in] qOrient Orientation quaternion
	  */
	virtual void SetSoundSourceOrientation( const int iID, const VAQuat& qOrient ) = 0;
	
	//! Sound source orientation getter for view and up vectors
	/**
	  * Uses OpenGL view and up convention.
	  *
	  * @param[in] iID Sound source identifier
	  * @param[out] v3View View vector
	  * @param[out] v3Up Up vector
	  */
	virtual void GetSoundSourceOrientationVU( const int iID, VAVec3& v3View, VAVec3& v3Up ) const = 0;
	
	//! Sound source orientation setter using view and up vector
	/**
	  * Uses OpenGL view and up convention.
	  *
	  * @param[in] iID Sound source identifier
	  * @param[in] v3View View vector
	  * @param[in] v3Up Up vector
	  */
	virtual void SetSoundSourceOrientationVU( const int iID, const VAVec3& v3View, const VAVec3& v3Up ) = 0;


	//! Returns all sound receiver IDs
	/**
	  * @param[out] viIDs Sound receiver identifier vector
	  */
	virtual void GetSoundReceiverIDs( std::vector< int >& viIDs ) const = 0;

	//! Creates a sound receiver
	/**
	  * This method creates a new sound receiver (sound receiver).
	  *
	  * @param[in] sName		Name (optional)
	  *
	  * @return ID of the new sound receiver, if the method succeeded, -1, otherwise
	  */
	virtual int CreateSoundReceiver( const std::string& sName = "" ) = 0;

	//! Creates a sound receiver explicitly for a given renderer
	/**
	  * This method creates a new sound receiver for a given renderer, only.
	  * the sound receiver will be skipped by all other renderers.
	  *
	  * @param[in]	sRendererID		Renderer identifier
	  * @param[in]	sName			Name
	  *
	  * @return ID of the new sound receiver, if the method succeeded and -1, otherwise
	  */
	virtual int CreateSoundReceiverExplicitRenderer( const std::string& sRendererID, const std::string& sName = "" ) = 0;

	//! Removes a sound receiver
	/**
	  * Will fail if sound receiver is an active listener.
	  *
	  * @return 0 on success, -1 otherwise
	  */
	virtual int DeleteSoundReceiver( const int iID ) = 0;

	//! Sound receiver info getter
	/**
	  * @param[in] iID Sound receiver identifier
	  * @return Sound receiver info
	  */
 	virtual CVASoundReceiverInfo GetSoundReceiverInfo( const int iID ) const = 0;

	//! Enables or disables a virtual sound receiver (removes the sound receiver from audio processing)
	/**
	  * @param[in] iID	Sound receiver identifier
	  * @param[in] bEnabled		Enable (true) or disable (false) sound source
	  *
	  * @sa GetSoundReceiverEnabled()
	  */
	virtual void SetSoundReceiverEnabled( const int iID, const bool bEnabled = true ) = 0;

	//! Returns sound receiver enabled/disabled status
	/**
	  * @param[in] iID	SoundReceiver identifier
	  * @return				Enabled (true) or disabled (false)
	  *
	  * @sa SetSoundReceiverEnabled()
	  */
	virtual bool GetSoundReceiverEnabled( const int iID ) const = 0;

	//! Sound receiver name getter
	/**
	  * @param[in] iID Sound receiver identifier
	  * @return Verbatim name
	  */
	virtual std::string GetSoundReceiverName( const int iID ) const = 0;
	
	//! Sound receiver name setter
	/**
	  * @param[in] iID Sound receiver identifier
	  * @param[in] sName Verbatim name
	  */
	virtual void SetSoundReceiverName( const int iID, const std::string& sName ) = 0;
	
	//! Sound receiver auralization mode getter
	/**
	  * @param[in] iID Sound receiver identifier
	  * @return Auralization mode bit-vector
	  */
	virtual int GetSoundReceiverAuralizationMode( const int iID ) const = 0;
	
	//! Sound receiver auralization mode setter
	/**
	  * @param[in] iSoundReceiverID Sound receiver identifier
	  * @param[in] iAuralizationMode Auralization mode bit-vector
	  */
	virtual void SetSoundReceiverAuralizationMode( const int iSoundReceiverID, const int iAuralizationMode ) = 0;

	//! Set sound receiver parameters
	/**
	  * This general parameter setter can be used for quick solutions changing
	  * sound receiver parameters without introducing new methods to the main interface.
	  *
	  * @param[in] iID SoundReceiver identifier
	  * @param[in] oParams Magic struct with the parameters
	  */
	virtual void SetSoundReceiverParameters( const int iID, const CVAStruct& oParams ) = 0;

	//! Get sound receiver parameters
	/**
	  * This general parameter getter can be used for quick solutions retrieving
	  * sound receiver parameters without introducing new methods to the main interface.
	  *
	  * @param[in] iID SoundReceiver identifier
	  * @param[in] oArgs Magic struct with the parameters that are requested
	  * @return Magic struct with the parameter values requested
	  */
	virtual CVAStruct GetSoundReceiverParameters( const int iID, const CVAStruct& oArgs ) const = 0;
	
	//! Returns the directivity of a sound source
	/**
	  * If the sound receiver is not assigned a directivity, the methods returns -1.
	  * @param[in] iID Sound receiver identifier
	  * @return Directivity identifier
	  */
	virtual int GetSoundReceiverDirectivity( const int iID ) const = 0;
	
	//! Sets the directivity of a sound receiver
	/**
	  * In order to remove an assigned directivity, you can pass -1 to the method.
	  * @param[in] iSoundReceiverID Sound receiver identifier
	  * @param[in] iDirectivityID Directivity identifier
	  */
	virtual void SetSoundReceiverDirectivity( const int iSoundReceiverID, const int iDirectivityID ) = 0;
	
	//! Sound receiver geo mesh identifier getter
	/**
	  * @param[in] iID	Sound receiver identifier
	  * @return Geo mesh identifier
	  *
	  */
	virtual int GetSoundReceiverGeometryMesh( const int iID ) const = 0;
	
	//! Sound receiver geo mesh identifier getter
	/**
	  * Sound receivers can have geometrical meshes assigned, i.e. to identify which geometry is part
	  * of a sound receiver that should not be considered for direct path occlusion - or if the
	  * moving receiver geometry is relevant for occlusion of other source-receivere-paths, like
	  * cars, trains, etc.
	  *
	  * @param[in] iSoundReceiverID	Sound source identifier
	  * @param[in] iGeometryMeshID Geo mesh identifier
	  *
	  */
	virtual void SetSoundReceiverGeometryMesh( const int iSoundReceiverID, const int iGeometryMeshID ) = 0;
	
	//! Sound source pose getter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[out] v3Pos Position vector
	  * @param[out] qOrient Orientation quaternion
	  */
	virtual void GetSoundReceiverPose( const int iID, VAVec3& v3Pos, VAQuat& qOrient ) const = 0;
	
	//! Sound source pose setter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[in] v3Pos Position vector
	  * @param[in] qOrient Orientation quaternion
	  */
	virtual void SetSoundReceiverPose( const int iID, const VAVec3& v3Pos, const VAQuat& qOrient ) = 0;
	
	//! Sound source position getter
	/**
	  * @param[in] iID Sound source identifier
	  * @return Position vector
	  */
	virtual VAVec3 GetSoundReceiverPosition( const int iID ) const = 0;
	
	//! Sound source position setter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[in] v3Pos Position vector
	  */
	virtual void SetSoundReceiverPosition( const int iID, const VAVec3& v3Pos ) = 0;
	
	//! Sound source orientation getter for view and up vectors
	/**
	  * Uses OpenGL view and up convention.
	  *
	  * @param[in] iID Sound source identifier
	  * @param[out] v3View View vector
	  * @param[out] v3Up Up vector
	  */
	virtual void GetSoundReceiverOrientationVU( const int iID, VAVec3& v3View, VAVec3& v3Up ) const = 0;
	
	//! Sound source orientation setter using view and up vector
	/**
	  * Uses OpenGL view and up convention.
	  *
	  * @param[in] iID Sound source identifier
	  * @param[in] v3View View vector
	  * @param[in] v3Up Up vector
	  */
	virtual void SetSoundReceiverOrientationVU( const int iID, const VAVec3& v3View, const VAVec3& v3Up ) = 0;
	
	//! Sound source orientation getter
	/**
	  * @param[in] iID Sound source identifier
	  * @return Orientation quaternion
	  */
	virtual VAQuat GetSoundReceiverOrientation( const int iID ) const = 0;
	
	//! Sound source orientation setter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[in] qOrient Orientation quaternion
	  */
	virtual void SetSoundReceiverOrientation( const int iID, const VAQuat& qOrient ) = 0;

	//! Sound receiver head-above-torso orientation getter
	/**
	  * @param[in] iID Sound source identifier
	  * @return Orientation of head-above-torso as quaternion
	  */
	virtual VAQuat GetSoundReceiverHeadAboveTorsoOrientation( const int iID ) const = 0;
	
	//! Sound receiver head-above-torso orientation setter
	/**
	  * @param[in] iID Sound source identifier
	  * @param[in] qOrient Orientation quaternion of head-above-torso
	  */
	virtual void SetSoundReceiverHeadAboveTorsoOrientation( const int iID, const VAQuat& qOrient ) = 0;

	//! Returns the position and orientation of the real-world sound receiver's head
	/**
	  * @note Coordinates refer the to center of the head on the axis which goes through both ears.
	  *
	  * @param[in] iID Sound receiver identifier
	  * @param[out] v3Pos Real world position of sound receiver
	  * @param[out] v3View Real world view vector of sound receiver
	  * @param[out] v3Up Real world up vector of sound receiver
	  */
	virtual void GetSoundReceiverRealWorldPositionOrientationVU( const int iID, VAVec3& v3Pos, VAVec3& v3View, VAVec3& v3Up ) const = 0;

	//! Updates the position and orientation of the real-world sound receiver's head
	/**
	  * This function is used to provide the crosstalk-cancellation module
	  * with the current position of the sound receivers head in the real-world.
  	  *
	  * @note Coordinates refer the to center of the head on the axis which goes through both ears.
	  *
	  * @param[in] iID Sound receiver identifier
	  * @param[in] v3Pos Real world position of sound receiver
	  * @param[in] v3View Real world view vector of sound receiver
	  * @param[in] v3Up Real world up vector of sound receiver
	  *
	  */
	virtual void SetSoundReceiverRealWorldPositionOrientationVU( const int iID, const VAVec3& v3Pos, const VAVec3& v3View, const VAVec3& v3Up ) = 0;
	
	//! Returns the pose of the real-world sound receiver's head
	/**
	  * @note Coordinates refer the to center of the head on the axis which goes through both ears.
	  *
	  * @param[in] iID Sound receiver identifier
	  * @param[out] v3Pos Real world position of sound receiver
	  * @param[out] qOrient Real world orientation as quaternion of sound receiver
	  *
	  */
	virtual void GetSoundReceiverRealWorldPose( const int iID, VAVec3& v3Pos, VAQuat& qOrient ) const = 0;
	
	//! Updates the pose of the real-world sound receiver's head
	/**
	  * This function is used to provide the crosstalk-cancellation module
	  * with the current position of the sound receivers head in the real-world.
  	  *
	  * @note Coordinates refer the to center of the head on the axis which goes through both ears.
	  *
	  * @param[in] iID Sound receiver identifier
	  * @param[in] v3Pos Real world position of sound receiver
	  * @param[in] qOrient Real world orientation as quaternion of sound receiver
	  *
	  */
	virtual void SetSoundReceiverRealWorldPose( const int iID, const VAVec3& v3Pos, const VAQuat& qOrient ) = 0;
	
	//! Sound receiver real-world head-above-torso (HATO) orientation getter
	/**
	  * A neutral HATO orientation will use the neutral head position looking to the front. A diverging
	  * orientation will describe how the head is oriented with respect to the torso (from the viewpoint of torso).
	  *
	  * @param[in] iID Sound source identifier
	  * @return Orientation of head-above-torso as quaternion
	  */
	virtual VAQuat GetSoundReceiverRealWorldHeadAboveTorsoOrientation( const int iID ) const = 0;
	
	//! Sound receiver real-world head-above-torso orientation setter
	/**
	  * A neutral HATO orientation will use the neutral head position looking to the front. A diverging
	  * orientation will describe how the head is oriented with respect to the torso (from the viewpoint of torso).
	  *
	  * @param[in] iID Sound source identifier
	  * @param[in] qOrient Orientation quaternion of head-above-torso
	  */
	virtual void SetSoundReceiverRealWorldHeadAboveTorsoOrientation( const int iID, const VAQuat& qOrient ) = 0;


	//! Set homogeneous medium sound speed in m/s
	/**
	  * Sets the speed of sound of the homogeneous medium. Significant changes to this parameter
	  * might cause audible artifacts due to its unphysical background.
	  *
	  * @param[in] dSoundSpeed Speed of sound [m/s]
	  */
	virtual void SetHomogeneousMediumSoundSpeed( const double dSoundSpeed ) = 0;

	//! Get homogeneous medium sound speed in m/s
	/**
	  * @return Speed of sound [m/s]
	  */
	virtual double GetHomogeneousMediumSoundSpeed() const = 0;

	//! Set homogeneous medium temperature in degree Celsius (0 is freezing point of water)
	/**
	  * Sets the temperature of the homogeneous medium. Significant changes to this parameter
	  * might cause audible artifacts due to its unphysical background.
	  *
	  * @param[in] dDegreesCentigrade Degrees Centigrade [°]
	  */
	virtual void SetHomogeneousMediumTemperature( const double dDegreesCentigrade ) = 0;

	//! Get homogeneous medium temperature in degree Celsius (0 is freezing point of water)
	/**
	  * @return Temperature in Degrees Centigrade [°]
	  */
	virtual double GetHomogeneousMediumTemperature() const = 0;

	//! Set homogeneous medium static pressure in Pascal
	/**
	  * Sets the static pressure of the homogeneous medium. Significant changes to this parameter
	  * might cause audible artifacts due to its unphysical background.
	  *
	  * @param[in] dPressurePascal Static pressure [Pa]
	  */
	virtual void SetHomogeneousMediumStaticPressure( const double dPressurePascal ) = 0;

	//! Get homogeneous medium static pressure in Pascal
	/**
	  * @return Static pressure [Pa]
	  */
	virtual double GetHomogeneousMediumStaticPressure() const = 0;

	//! Set homogeneous medium relative humidity in percent (0.0 = no vapor, 50.0 = 50%, ... )
	/**
	  * Sets the relative humidity of the homogeneous medium. Significant changes to this parameter
	  * might cause audible artifacts due to its unphysical background.
	  *
	  * @param[in] dRelativeHumidityPercent Relative humidity [%] ( 0.0f .. 50.0f ... 100.0f ... )
	  */
	virtual void SetHomogeneousMediumRelativeHumidity( const double dRelativeHumidityPercent ) = 0;

	//! Get homogeneous medium relative humidity in percent (0.0 = no vapor, 50.0 = 50%, ... )
	/**
	  * @return Relative humidity [%] ( 0.0f .. 50.0f ... 100.0f ... )
	  */
	virtual double GetHomogeneousMediumRelativeHumidity() = 0;

	//! Set homogeneous medium shift speed as 3-dim direction vector in m/s ("wind" speed with direction)
	/**
	  * Sets the shift speed of the homogeneous medium ("wind" speed). Significant changes to this parameter
	  * might cause audible artifacts due to its unphysical background.
	  *
	  * @param[in] v3TranslationSpeed Translatory speed [m/s]
	  */
	virtual void SetHomogeneousMediumShiftSpeed( const VAVec3& v3TranslationSpeed ) = 0;

	//! Get homogeneous medium shift speed as 3-dim direction vector in m/s ("wind" speed with direction)
	/**
	  * @return Translatory speed [m/s]
	  */
	virtual VAVec3 GetHomogeneousMediumShiftSpeed() const = 0;

	//! Set homogeneous medium special parameters
	/**
	  * Sets any further parameter of the medium, like Chemical constellation, particle density ...
	  * Intended to be used for prototyping. Can also be used to define non-homogeneous media by
	  * a proprietary definition that is used by a specialized rendering module, e.g. for air traffic
	  * noise.
	  *
	  * @param[in] oParams Magic medium parameters
	  */
	virtual void SetHomogeneousMediumParameters( const CVAStruct& oParams ) = 0;

	//! Get homogeneous medium special parameters with optional arguments
	/**
	  * Sets any further parameter of the medium, like Chemical constellation, particle density ...
	  * Intended to be used for prototyping. Can also be used to define non-homogeneous media by
	  * a proprietary definition that is used by a specialized rendering module, e.g. for air traffic
	  * noise.
	  *
	  * @param[in] oArgs Magic medium parameter request arguments
	  * @return Magic medium parameters
	  */
	virtual CVAStruct GetHomogeneousMediumParameters( const CVAStruct& oArgs ) = 0;


	//! Creates a virtual scene
	/**
	  * Scenes is a relatively loose description for a virtual environment. It may
	  * contain information of arbitrary form that can be intepreted by a rendering module.
	  * The scene methods are for prototyping in order to pass scene information to a 
	  * rendering module or an associated simulation backend that requires e.g. geometry input
	  * files.
	  *
	  * Scenes may be enabled and disabled for rapid scene switching.
	  *
	  * @param[in] oParams Scene parameters
	  * @param[in] sName Verbatin scene name
	  * @return Scene identifier string
	  */
	virtual std::string CreateScene( const CVAStruct& oParams, const std::string& sName = "" ) = 0;

	//! IDs of created scenes
	/**
	  * @param[out] vsIDs Scene identifier vector
	  */
	virtual void GetSceneIDs( std::vector< std::string >& vsIDs ) const = 0;

	//! Get scene info
	/**
	  * @param[in] sID Scene identifier
	  * @return Scene information
	  */
	virtual CVASceneInfo GetSceneInfo( const std::string& sID ) const = 0;

	//! Returns the name of given scene
	/**
	  * @param[in] sID Scene identifier
	  * @return Scene verbatim name
	  */
	virtual std::string GetSceneName( const std::string& sID ) const = 0;

	//! Set the name of given scene
	/**
	  * @param[in] sID Scene identifier
	  * @param[in] sName Scene verbatim name
	  */
	virtual void SetSceneName( const std::string& sID, const std::string& sName ) = 0;

	//! Set scene enabled or disabled
	/**
	  * @param[in] sID Scene identifier
	  * @param[in] bEnabled Enabled/disabled flag
	  */
	virtual void SetSceneEnabled( const std::string& sID, const bool bEnabled = true ) = 0;

	//! Scene enabled status (true, if enabled)
	/**
	  * @param[in] sID Scene identifier
	  * @return True, if scene is enabled
	  */
	virtual bool GetSceneEnabled( const std::string& sID ) const = 0;


	//! Create a sound portal
	/**
	  * This method creates a new sound portal and returns its ID.
	  *
	  * @param[in] sName					Name (optional)
	  *
	  * @return ID of the new sound portal, if the method succeeded, -1 otherwise
	  */
	virtual int CreateSoundPortal( const std::string& sName = "" ) = 0;

	//! Return all portal IDs
	/**
	  * @param[out] viIDs Sound portal identifiers
	  */
	virtual void GetSoundPortalIDs( std::vector< int >& viIDs ) = 0;
	
	//! Return sound portal info
	/**
	  * @param[in] iID Sound portal identifier
	  * @return Sound portal information
	  */
	virtual CVASoundPortalInfo GetSoundPortalInfo( const int iID ) const = 0;

	//! Returns the name of a portal
	/**
	  * @param[in] iID	Sound portal identifier
	  * @return Verbatim name
 	  *
	  */
	virtual std::string GetSoundPortalName( const int iID ) const = 0;

	//! Sets the name of a portal
	/**
	  * @param[in] iID	Sound portal identifier
	  * @param[in] sName Verbatim name
	  *
	  */
	virtual void SetSoundPortalName( const int iID, const std::string& sName ) = 0;

	//! Sound portal material setter
	/**
	  * @param[in] iSoundPortalID	Sound portal identifier
	  * @param[in] iMaterialID Assigned material identifier
	  *
	  */
	virtual void SetSoundPortalMaterial( const int iSoundPortalID, const int iMaterialID ) = 0;

	//! Sound portal material getter
	/**
	  * @param[in] iID	Sound portal identifier
	  * @return Assigned material identifier or -1
	  *
	  */
	virtual int GetSoundPortalMaterial( const int iID ) const = 0;

	//! Sound portal next portal setter
	/**
	  * @param[in] iSoundPortalID	Sound portal identifier
	  * @param[in] iNextSoundPortalID	Next connected sound portal identifier
	  *
	  */
	virtual void SetSoundPortalNextPortal( const int iSoundPortalID, const int iNextSoundPortalID ) = 0;

	//! Sound portal next portal getter
	/**
	  * @param[in] iID	Sound portal identifier
	  * @return	Next connected sound portal identifier, or -1
	  *
	  */
	virtual int GetSoundPortalNextPortal( const int iID ) const = 0;

	//! Sound portal target sound receiver setter
	/**
	  * @param[in] iSoundPortalID	Sound portal identifier
	  * @param[in] iSoundReceiverID	Connected sound receiver identifier
	  *
	  */
	virtual void SetSoundPortalSoundReceiver( const int iSoundPortalID, const int iSoundReceiverID ) = 0;

	//! Sound portal target sound receiver getter
	/**
	  * @param[in] iSoundPortalID	Sound portal identifier
	  * @return	Connected sound receiver identifier, or -1
	  *
	  */
	virtual int GetSoundPortalSoundReceiver( const int iSoundPortalID ) const = 0;

	//! Sound portal sound source setter
	/**
	  * @param[in] iSoundPortalID	Sound portal identifier
	  * @param[in] iSoundSourceID	Connected sound source identifier
	  *
	  */
	virtual void SetSoundPortalSoundSource( const int iSoundPortalID, const int iSoundSourceID ) = 0;

	//! Sound portal sound source getter
	/**
	  * @param[in] iSoundPortalID	Sound portal identifier
	  * @return	Connected sound source identifier, or -1
	  *
	  */
	virtual int GetSoundPortalSoundSource( const int iSoundPortalID ) const = 0;
	
	//! Get sound portal parameters
	/**
	  * This general parameter getter can be used for quick solutions retrieving
	  * sound portal parameters without introducing new methods to the main interface.
	  *
	  * @param[in] iID Sound portal identifier
	  * @param[in] oParams Magic struct with the parameters that are requested
	  * @return Magic struct with the parameter values requested
	  */
	virtual CVAStruct GetSoundPortalParameters( const int iID, const CVAStruct& oParams ) const = 0;
	
	//! Set sound portal parameters
	/**
	  * This general parameter setter can be used for quick solutions changing
	  * sound portal parameters without introducing new methods to the main interface.
	  *
	  * @param[in] iID Sound portal identifier
	  * @param[in] oParams Magic struct with the parameters
	  */
	virtual void SetSoundPortalParameters( const int iID, const CVAStruct& oParams ) = 0;

	//! Sound portal position setter
	/**
	  * @param[in] iID Sound portal identifier
	  * @param[in] vPos Position vector
	  */
	virtual void SetSoundPortalPosition( const int iID, const VAVec3& vPos ) = 0;

	//! Sound portal position getter
	/**
	  * @param[in] iID Sound portal identifier
	  * @return Position vector
	  */
	virtual VAVec3 GetSoundPortalPosition( const int iID ) const = 0;

	//! Sound portal orientation setter
	/**
	  * @param[in] iID Sound portal identifier
	  * @param[in] qOrient Orientation quaternion
	  */
	virtual void SetSoundPortalOrientation( const int iID, const VAQuat& qOrient ) = 0;

	//! Sound portal orientation getter
	/**
	  * @param[in] iID Sound portal identifier
	  * @return Orientation quaternion
	  */
	virtual VAQuat GetSoundPortalOrientation( const int iID ) const = 0;

	//! Sound portal enabled setter
	/**
	  * @param[in] iID Sound portal identifier
	  * @param[in] bEnabled Enables sound portal, if true
	  */
	virtual void SetSoundPortalEnabled( const int iID, const bool bEnabled = true ) = 0;

	//! Sound portal orientation getter
	/**
	  * @param[in] iID Sound portal identifier
	  * @return True, if enabled
	  */
	virtual bool GetSoundPortalEnabled( const int iID ) const = 0;




	//! Returns all available renderers (default: only those that are enabled)
	/**
	  * @param [out]	voRenderer	List of renderers modules
	  * @param [in]	bFilterEnabled	If true, only enabled renderers are returned (via configuration, during runtime)
	  */
	virtual void GetRenderingModules( std::vector< CVAAudioRendererInfo >& voRenderer, const bool bFilterEnabled = true ) const = 0;

	//! Mutes/unmutes a rendering module audio output
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] bMuted Mutes module, if true, unmutes otherwise
	  */
	virtual void SetRenderingModuleMuted( const std::string& sModuleID, const bool bMuted = true ) = 0;

	//! Returns if rendering module audio output is muted or not
	/**
	  * @param[in] sModuleID Module identifier
	  * @return True, if module muted, false otherwise
	  */
	virtual bool GetRenderingModuleMuted( const std::string& sModuleID ) const = 0;

	//! Sets the rendering module audio output gain
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] dGain Gain
	  *
	  * @warning Setting gains will potentially penetrate a calibrated rendering and reproduction chain
	  */
	virtual void SetRenderingModuleGain( const std::string& sModuleID, const double dGain ) = 0;

	//! Returns the rendering module audio output gain
	/**
	  * @param[in] sModuleID Module identifier
	  * @return Gain
	  */
	virtual double GetRenderingModuleGain( const std::string& sModuleID ) const = 0;

	//! Sets the rendering module auralization mode
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] iAuralizationMode Auralization mode integer
	  */
	virtual void SetRenderingModuleAuralizationMode( const std::string& sModuleID, const int iAuralizationMode ) = 0;

	//! Returns the rendering module auralization mode
	/**
	  * @param[in] sModuleID Module identifier
	  * @return Auralization mode bit-vector
	  */
	virtual int GetRenderingModuleAuralizationMode( const std::string& sModuleID ) const = 0;

	//! Sets the rendering module parameters
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] oParams Magic parameters
	  */
	virtual void SetRenderingModuleParameters( const std::string& sModuleID, const CVAStruct& oParams ) = 0;

	//! Returns the rendering module parameters
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] oParams Magic request parameters
	  * @return Magic parameters
	  */
	virtual CVAStruct GetRenderingModuleParameters( const std::string& sModuleID, const CVAStruct& oParams ) const = 0;


	//! Returns all available reproductions (default: only those that are enabled)
	/**
	 * @param [out]	voReproductions	List of reproduction modules
	 * @param [in]	bFilterEnabled	If true, only enabled renderers are returned (via configuration, during runtime)
	 */
	virtual void GetReproductionModules( std::vector< CVAAudioReproductionInfo >& voReproductions, const bool bFilterEnabled = true ) const = 0;

	//! Mutes/unmutes a rendering module audio output
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] bMuted Mutes module, if true, unmutes otherwise
	  */
	virtual void SetReproductionModuleMuted( const std::string& sModuleID, const bool bMuted = true ) = 0;

	//! Returns if rendering module audio output is muted or not
	/**
	  * @param[in] sModuleID Module identifier
	  * @return True, if module muted, false otherwise
	  */
	virtual bool GetReproductionModuleMuted( const std::string& sModuleID ) const = 0;

	//! Sets the reproduction module audio output gain
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] dGain Gain factor
	  *
	  * @warning Setting gains will potentially penetrate a calibrated rendering and reproduction chain
	  */
	virtual void SetReproductionModuleGain( const std::string& sModuleID, const double dGain ) = 0;

	//! Returns the rendering module audio output gain
	/**
	  * @param[in] sModuleID Module identifier
	  * @return Gain factor
	  */
	virtual double GetReproductionModuleGain( const std::string& sModuleID ) const = 0;
	
	//! Sets the reproduction module parameters
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] oParams Magic parameters
	  */
	virtual void SetReproductionModuleParameters( const std::string& sModuleID, const CVAStruct& oParams ) = 0;
	
	//! Returns the reproduction module parameters
	/**
	  * @param[in] sModuleID Module identifier
	  * @param[in] oParams Magic request parameters
	  * @return Magic parameters
	  */
	virtual CVAStruct GetReproductionModuleParameters( const std::string& sModuleID, const CVAStruct& oParams ) const = 0;

	
	//! Returns the global audio interface input gain
	/**
	  * @return Gain factor
	  */
	virtual double GetInputGain() const = 0;
	
	//! Sets the global audio interface input gain
	/**
	  * @param[in] dGain Gain factor [0 .. 1]
	  *
	  * @warning Setting gains will potentially penetrate a calibrated rendering and reproduction chain
	  */
	virtual void SetInputGain( const double dGain ) = 0;
	
	//! Returns if global audio interface input is muted or not
	/**
	  * @return True, if muted, false otherwise
	  */
	virtual bool GetInputMuted() const = 0;
	
	//! Mutes global audio interface input
	/**
	  * @param[in] bMuted Mutes global input if true, unmutes otherwise
	  */
	virtual void SetInputMuted( const bool bMuted = true ) = 0;
	
	//! Returns the global audio interface output gain
	/**
	  * @return Gain factor
	  */
	virtual double GetOutputGain() const = 0;

	//! Sets the global audio interface output gain
	/**
	  * @param[in] dGain Gain factor [0 .. 1]
	  *
	  * @warning Setting gains will potentially penetrate a calibrated rendering and reproduction chain
	  */
	virtual void SetOutputGain( const double dGain ) = 0;
	
	//! Returns if global audio interface output is muted or not
	/**
	  * @return True, if muted, false otherwise
	  */
	virtual bool GetOutputMuted() const = 0;

	//! Sets the global audio interface input gain
	/**
	  * @param[in] bMuted True to mute
	  *
	  */
	virtual void SetOutputMuted( const bool bMuted = true ) = 0;

	
	//! Returns the global auralization mode
	/**
	  * @return Auralization mode bit-vector
	  */
	virtual int GetGlobalAuralizationMode() const = 0;
	
	//! Sets the global auralization mode
	/**
	  * @param[in] iAuralizationMode Auralization mode integer (bit-vector)
	  */
	virtual void SetGlobalAuralizationMode( const int iAuralizationMode ) = 0;

	//! Returns the (global) active sound receiver
	/**
	  * The (global) active sound receiver is the virtual sound receiver for which
	  * the renderers will route the output for, if the sound receiver is
	  * not used explicitly for a given renderer.
	  *
	  * @return Active sound receiver id or -1, if no (global) active sound receiver
	  *
	  * @sa GetActiveSoundReceiverExplicitRenderer
	  */
	virtual int GetActiveSoundReceiver() const = 0;

	//! Sets the active sound receiver
	/**
	  * The renderers produce the output of this sound receiver, only
	  * (if sound receiver is not explicitly created for another renderer)
	  *
	  * @param[in] iSoundReceiverID	SoundReceiver identifier
	  *
	  * @sa GetActiveSoundReceiver
	  * @sa SetActiveSoundReceiverExplicitRenderer
	  */
	virtual void SetActiveSoundReceiver( const int iSoundReceiverID ) = 0;

	//! Returns the active sound receiver of an explicit renderer
	/**
	  * @param[in] sRendererID	Renderer identifier
	  * @return Active sound receiver id or -1, if the sound receiver is not activated explicitly
	  *
	  * @sa GetActiveSoundReceiver
	  * @sa SetActiveSoundReceiverExplicitRenderer
	  */
	virtual int GetActiveSoundReceiverExplicitRenderer( const std::string& sRendererID ) const = 0;

	//! Sets the active sound receiver for an explicit renderer
	/**
	 * The renderers forward the output of this sound receiver, only
	 * (if sound receiver is not explicitly created for another renderer)
	 *
	 * @param[in] iSoundReceiverID	SoundReceiver identifier
	 * @param[in] sRendererID	Renderer identifier
	 *
	 * @sa GetActiveSoundReceiverExplicitRenderer
	 * @sa SetActiveSoundReceiver
	 */
	virtual void SetActiveSoundReceiverExplicitRenderer( const int iSoundReceiverID, const std::string& sRendererID ) = 0;

	//! Get current core time
	/**
	  * This method returns the current clock of the core instance in seconds.
	  * This Clock is offset-cleaned from Stream Tracker, which uses the default clock
	  *
	  * @return Clock time in seconds
	  */
	virtual double GetCoreClock() const = 0;

	//! Set core time
	/**
	  * This methods (re)sets the core time to the given value in seconds.
	  * The new value must be a number greater or equal zero.
	  *
	  * @param[in] dSeconds Clock time in seconds
	  */
	virtual void SetCoreClock( const double dSeconds ) = 0;

	//! Substitute VA core macro definitions in a string (e.g. a filename)
	/**
	  * @param[in] sStr String with macro
	  * @return String where macros have been substituted, if macro has been set
	  */
	virtual std::string SubstituteMacros( const std::string& sStr ) const = 0;

	//! Searches for the file and returns full path (relative or absolute)
	/**
	  * @param[in] sFilePath non-empty relative file path
	  * @return If found, first valid combined file path; empty string otherwise
	  *
	  * @note Also substitutes macros
	  * @note Prints a warning if file not found or ambiguous name
	  */
	virtual std::string FindFilePath( const std::string& sFilePath ) const = 0;



	//! Auralization mode bit-vector validation
	/**
	  * @param[in] iAuralizationMode Auralization bit-vector
	  * @return True, if auralization bit-vector is valid (positive bits are reflecting a mode)
	  */
	static bool GetAuralizationModeValid( const int iAuralizationMode );
	
	//! Checks if volume value is positive
	/**
	  * @param[in] dVolume Auralization bit-vector
	  * @return True, if volume is valid (greater zero)
	  */
	static bool GetVolumeValid( const double dVolume );

	//! Validates buffer signal source playback action
	/**
	  * @param[in] iAction Action integer
	  * @return True, if action is recognized
	  */
	static bool GetSignalSourceBufferPlaybackActionValid( const int iAction );

	//! Log level string getter
	/**
	  * @param[in] iLogLevel
	  * @return Log level string
	  */
	static std::string GetLogLevelStr( const int iLogLevel );

	//! Parses an auralization mode from a string (short format only)
	/**
	  * This method takes a string with auralization modes in short format
	  * (e.g. "DS,ER") and interprets it as an auralization mode.
	  * More over the method allows to compute altered modes
	  * (e.g. "+DP" for enabling doppler effects) based on a given
	  * auralization mode.
	  *
	  * @param[in] sModeStr
	  * @param[in] iBaseMode Base mode for differential mode setter
	  * @return Modified auralization mode
	  */
	static int ParseAuralizationModeStr( const std::string& sModeStr, const int iBaseMode = VA_AURAMODE_NOTHING );

	//! Returns an auralization mode as formatted string
	/**
	  * @param[in] iAuralizationMode Input auralization mode
	  * @param[in] bShortFormat Flag for short mode
	  * @return Auralization mode string
	  */
	static std::string GetAuralizationModeStr( const int iAuralizationMode, const bool bShortFormat = false );

	//! Converst a volume to decibel as string (for formatting)
	/**
	  * @param[in] dVolume Volume
	  * @return Volume as decibel string (including dB)
	  */
	static std::string GetVolumeStrDecibel( const double dVolume );

	//! Parses an play state from a string
	/**
	  * This method takes a string with play state
	  * (e.g. "STOPPED", "PLAYING", "PAUSED") and converts it into a integer representation.
	  *
	  * @param[in] sStr Playback state string
	  * @return Playback state integer
	  */
	static int ParsePlaybackState( const std::string& sStr );

	//! Returns an play state as formatted string (short format)
	/**
	  * @param[in] iPlaybackState Playback state integer
	  * @return Playback state string
	  */
	static std::string GetPlaybackStateStr( const int iPlaybackState );

	//! Returns an play action as formatted string (short format)
	/**
	  * @param[in] iPlaybackAction Playback action integer
	  * @return Playback action string
	  */
	static std::string GetPlaybackActionStr( const int iPlaybackAction );

	//! Returns the playback action integer from a string representation (PLAY, STOP, PAUSE)
	/**
	  * @param[in] sPlaybackAction Playback action string
	  * @return Playback action integer
	  */
	static int ParsePlaybackAction( const std::string& sPlaybackAction );

	//! Returns the table of literals defined by the IVACore interface
	/** 
	  * (This function is useful, when you implement bindings for other programming languages)
	  *
	  * @return Integer literal vector
	  */
	static const std::vector< CVAIntLiteral >& GetLiterals();

protected:
	IVAInterface();

private:
	// This interface is uncopyable
	IVAInterface( const IVAInterface& );
	IVAInterface& operator=( const IVAInterface& );
};

#endif // IW_VABASE_INTERFACE
