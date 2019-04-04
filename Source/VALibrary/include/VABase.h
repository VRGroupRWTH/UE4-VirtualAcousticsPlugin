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

#ifndef IW_VABASE
#define IW_VABASE

#include <VABaseDefinitions.h>

#include <VAStruct.h>

#include <string>
#include <vector>
#include <cmath>

// L_w = L_p + 10 * log10( 1m / 1m ) dB re 1pW + 10 * log10 ( 4 * pi ) dB re 1pW; P = 10^( L_w / 10 ) * 1pW
static const double g_dSoundPower_94dB_SPL_1m = 0.0316227766f; //!< [W] Sound power that results in 94 dB SPL re 20uPa @ 1m for spherical spreading
static const double g_dSoundPower_128dB_SPL_1m = 31.6227766017f; //!< [W] Sound power that results in 124 dB SPL re 20uPa @ 1m for spherical spreading

// c ~= ( 331.5 + 0.6 * 20 degree / degree ) m/s
static const double g_dDefaultSpeedOfSound = 343.0f; //!< [m/s] Sound speed at 20 degree centigrade with adiabatic approximation
static const double g_dDefaultStaticPressure = 101325.0f; //!< [Pa] Norm pressure over NN; 1 bar; 1 atmosphere; *100 hPa
static const double g_dDefaultTemperature = 20.0f; //!< [Degree centigrade] Arbitrarily defined, satisfactory for indoor and outdoor
static const double g_dDefaultRelativeHumidity = 20.0f; //!< [Percent]

//! 3-element Vector (double precision), in a geometrical context in meters (if not stated otherwise)
/**
  * Uses Cartesian style, mostly operated within OpenGL-rotation convention context (especially for view-up direction vectors)
  */
class VABASE_API VAVec3
{
public:
	union
	{
		//!< Vector compontents in order x,y,z
		double comp[ 3 ];
		struct
		{
			double x, y, z;
		};
	};

	//! Default constructur, sets values to zero
	inline VAVec3() : x( 0 ), y( 0 ), z( 0 ) {};

	//! Copy constructor
	/**
	  * @param[in] v Copy values from this vector
	  */
	inline VAVec3( const VAVec3& v ) : x( v.x ), y( v.y ), z( v.z ) {};

	//! Init constructor
	/**
	  * @param[in] x X component
	  * @param[in] y Y component
	  * @param[in] z Z component
	  */
	inline VAVec3( const double x, const double y, const double z ) : x( x ), y( y ), z( z ) {};

	//! Destructor
	inline virtual ~VAVec3() {};

	//! Returns RMS lengs / L2 norm
	/*
	 * @return L2 norm length of cartesian cevrot
	 */
	inline virtual double Length() const
	{
		return sqrt( x*x + y*y + z*z );
	};

	//! Component setter
	/**
	  * @param[in] x_ X component
	  * @param[in] y_ Y component
	  * @param[in] z_ Z component
	  */
	inline virtual void Set( const double x_, const double y_, const double z_ )
	{
		x = x_;
		y = y_;
		z = z_;
	};

	//! Get cross product
	/**
	  * Performs cross product of internal components with parameter vector
	  * @param[in] vCrossProduct Cross product right-hand value
	  * @return Cross product vector
	  */
	inline VAVec3 Cross( const VAVec3& vCrossProduct ) const
	{
		VAVec3 vCrossResult;
		vCrossResult.x = y * vCrossProduct.z - z * vCrossProduct.y;
		vCrossResult.y = z * vCrossProduct.x - x * vCrossProduct.z;
		vCrossResult.z = x * vCrossProduct.y - y * vCrossProduct.x;
		return vCrossResult;
	};

	//! Get dot product
	/**
	  * Performs dot product of internal components with parameter vector
	  * @param[in] vDotProduct Dot product right-hand value
	  * @return Dot product scalar
	  */
	inline double Dot( const VAVec3& vDotProduct ) const
	{
		return ( x * vDotProduct.x + y * vDotProduct.y + z * vDotProduct.z );
	};

	//!< Normalizes internal components
	/**
	  * Will return in a L2 norm of 1.0
	  */
	inline void Norm()
	{
		const double l = Length();
		x /= l;
		y /= l;
		z /= l;
	};

	//!< Get vector as struct
	/**
	  * For struct payload serializaton / deserialization. Make sure that keys are not already assigned.
	  *
	  * @return Struct with vector as x,y,z components with keys "x", "y", "z".
	  */
	inline CVAStruct GetAsStruct() const
	{
		CVAStruct oVec;
		oVec[ "x" ] = x;
		oVec[ "y" ] = y;
		oVec[ "z" ] = z;
		return oVec;
	};
};

//!< Compare operator for vectors
/**
  * @param[in] a Left-hand vector
  * @param[in] b Right-hand vector
  *
  * @return True, if equal
  */
inline VABASE_API bool operator==( const VAVec3& a, const VAVec3& b )
{
	return ( ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) );
};

//!< Unequal operator for vectors
/**
  * @param[in] a Left-hand vector
  * @param[in] b Right-hand vector
  *
  * @return True, if unequal
  */
inline VABASE_API bool operator!=( const VAVec3& a, const VAVec3& b )
{
	return !( a == b );
};

//!< Add operator for vectors
/**
  * @param[in] oSummand1 Left-hand vector
  * @param[in] oSummand2 Right-hand vector
  *
  * @return Addition result
  */
inline VABASE_API VAVec3 operator+( const VAVec3& oSummand1, const VAVec3& oSummand2 )
{
	VAVec3 vSum;
	vSum.x = oSummand1.x + oSummand2.x;
	vSum.y = oSummand1.y + oSummand2.y;
	vSum.z = oSummand1.z + oSummand2.z;
	return vSum;
};

//!< Subtract operator for vectors
/**
  * @param[in] oSummand1 Left-hand vector
  * @param[in] oSummand2 Right-hand vector
  *
  * @return Subtraction result
  */
inline VABASE_API VAVec3 operator-( const VAVec3& oSummand1, const VAVec3& oSummand2 )
{
	VAVec3 vDiff;
	vDiff.x = oSummand1.x - oSummand2.x;
	vDiff.y = oSummand1.y - oSummand2.y;
	vDiff.z = oSummand1.z - oSummand2.z;
	return vDiff;
};

//!< Scalar multiply operator for vectors
/**
  * @param[in] oVec Vector
  * @param[in] dScalar Scalar
  *
  * @return Scaled vector
  */
inline VABASE_API VAVec3 operator*( const VAVec3& oVec, const double dScalar )
{
	VAVec3 vScaledVector = oVec;
	vScaledVector.x *= dScalar;
	vScaledVector.y *= dScalar;
	vScaledVector.z *= dScalar;
	return vScaledVector;
};

//! Stream output operator for VAVec3
/**
  * Output format: "< x, y, z >"
  *
  * @param[in] os Outstream
  * @param[in] oVec Vector
  *
  * @return Outstream with vector output attached
  */
VABASE_API std::ostream& operator<<( std::ostream& os, const VAVec3& oVec );


//! 4-element Quaternion (double precision)
/**
  * Quaternion representation for solid rotation and orientation representation.
  */
class VABASE_API VAQuat
{
public:
	union
	{
		//!< Components in order x,y,z,w
		double comp[ 4 ];
		struct
		{
			double x, y, z, w;
		};
	};

	//! Default constructur, sets values to zero and w to 1.0
	inline VAQuat() : x( 0 ), y( 0 ), z( 0 ), w( 1.0f ) {};

	//! Copy constructor
	/**
	  * @param[in] v Will copy values from this quaternion
	  */
	inline VAQuat( const VAQuat& v ) : x( v.x ), y( v.y ), z( v.z ), w( v.w ) {};
	
	//! Init constructor
	/**
	  * @param[in] x X component
	  * @param[in] y Y component
	  * @param[in] z Z component
	  * @param[in] w W component
	  */
	inline VAQuat( const double x, const double y, const double z, const double w = 1.0f ) : x( x ), y( y ), z( z ), w( w ) {};

	//! Destructor
	inline virtual ~VAQuat() {};
	
	//! Setter
	/**
	  * @param[in] x_ X component
	  * @param[in] y_ Y component
	  * @param[in] z_ Z component
	  * @param[in] w_ W component
	  */
	inline virtual void Set( const double x_, const double y_, const double z_, const double w_ )
	{
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	};
	
	//!< Get vector as struct
	/**
	  * For struct payload serializaton / deserialization. Make sure that keys are not already assigned.
	  *
	  * @return Struct with vector as x,y,z,w components with keys "x", "y", "z", "w".
	  */
	inline CVAStruct GetAsStruct() const
	{
		CVAStruct oOrient;
		oOrient[ "x" ] = x;
		oOrient[ "y" ] = y;
		oOrient[ "z" ] = z;
		oOrient[ "w" ] = w;
		return oOrient;
	};
};

//! Stream output operator for Quaternion
/**
  * Output format: "< x, y, z, w >"
  *
  * @param[in] os Outstream
  * @param[in] qOrient Quaternion orientation
  *
  * @return Outstream with quaternion output attached
  */
VABASE_API std::ostream& operator<<( std::ostream& os, const VAQuat& qOrient );


//! Data class describing callable and registrable objects
/**
  * In VA, anything can be an object. If an object is registered,
  * it can also be listed and called over the VA interface. Then, it
  * is called a module, @sa CVAModuleInfo, GetModules
  */
struct VABASE_API CVAObjectInfo
{
	int iID;			//!< Object ID
	std::string sName;	//!< Object name
	std::string sDesc;	//!< Object description

	//! Default constructor will set identifier to -1
	inline CVAObjectInfo()
		: iID( -1 )
	{};
};


//! Pure data class describing modules of the core
/**
  * For module info getter.
  */
struct VABASE_API CVAModuleInfo
{
	std::string sName;	//!< Module name
	std::string sDesc;	//!< Module description
	CVAStruct oParams; //!< Parameters
};

//! Struct describing an audio renderer
/**
  * Rendering module info for getters.
  */
struct VABASE_API CVAAudioRendererInfo
{
	std::string sID;	//!< Renderer identifier
	std::string sClass; //!< Renderer class
	std::string sDescription; //!< Renderer description
	bool bEnabled; //!< Availability during runtime
	bool bOutputDetectorEnabled; //!< Indicates if output detector is enabled
	bool bOutputRecordingEnabled; //!< Indicates if output detector is enabled
	std::string sOutputRecordingFilePath; //!< Output recording file path
	CVAStruct oParams; //!< Additional parameters, may be changed during runtime

	//! Constructor with default init values
	inline CVAAudioRendererInfo()
		: bEnabled( false )
		, bOutputDetectorEnabled( false )
		, bOutputRecordingEnabled( false )
	{};
};

//! Struct describing an audio reproduction module
/**
  * Reproduction module info for getters.
  */
struct VABASE_API CVAAudioReproductionInfo
{
	std::string sID;	//!< Reproduction module identifier
	std::string sClass; //!< Reproduction module class
	std::string sDescription; //!< Reproduction module description
	bool bEnabled; //!< Availability during runtime
	bool bInputDetectorEnabled; //!< Indicates if output detector is enabled
	bool bInputRecordingEnabled; //!< Indicates if output detector is enabled
	std::string sInputRecordingFilePath; //!< Output recording file path
	bool bOutputDetectorEnabled; //!< Indicates if output detector is enabled
	bool bOutputRecordingEnabled; //!< Indicates if output detector is enabled
	std::string sOutputRecordingFilePath; //!< Output recording file path
	CVAStruct oParams; //!< Additional parameters, may be changed during runtime

	//! Constructor with default init values
	inline CVAAudioReproductionInfo()
		: bEnabled( false )
		, bInputDetectorEnabled( false )
		, bInputRecordingEnabled( false )
		, bOutputDetectorEnabled( false )
		, bOutputRecordingEnabled( false )
	{};
};

//! Describes the current state of a progress
/**
  * This data class is used to describe the current state of a
  * running process (progress). It is intended for the purpose
  * of displaying the progress of an action that takes some time
  * to be shown in a user interface (i.e. the initialisation of
  * a core)
  */
class VABASE_API CVAProgress
{
public:
	int iCurrentStep;		//!< Current step number (0 = nothing happened yet)
	int iMaxStep;			//!< Maximum step number (end)
	std::string sAction;	//!< Currently performed action (top level)
	std::string sSubaction;	//!< Currently performed sub action

	//! Constructor inits values
	inline CVAProgress()
		: iCurrentStep( 0 )
		, iMaxStep( 0 )
	{
	};
};


//! Data class describing integer literals within the core interface (reflexions)
/**
  * For literals getter.
  */
class VABASE_API CVAIntLiteral
{
public:
	std::string sScope;		//!< Interface name (e.g. IVACore)
	std::string sName;		//!< Literal name (e.g. 'VA_AURAMODE_DIRECT_SOUND')
	int iValue;				//!< Literal integer value

	//! Constructor
	/**
	  * Create an integer literal.
	  *
	  * @param[in] sTheScope Scope of the literal, i.e. IVACore
	  * @param[in] sTheName Name string of the integer literal, i.e. 'VA_AURAMOD_DEFAULT'
	  * @param[in] iTheValue Value of the integer
	  */
	inline CVAIntLiteral( const std::string& sTheScope, const std::string& sTheName, const int iTheValue )
		: sScope( sTheScope )
		, sName( sTheName )
		, iValue( iTheValue )
	{
	};

private:
	//! No default construction
	inline CVAIntLiteral() {};
};

//! Data class containing information on directivities of sound sources and sound receivers
/**
  * This class describes a directivity: the sound changes depending on the directional component
  * of a sending or receiving entity. It may also include distance-dependent data.
  *
  * There are certain classes available, that will be recognized by most renderers. However,
  * you might want to use the PROTOTYPE class if you are testing new types that are not part
  * of the VA base interface.
  *
  * Special parameters are stores in the parameter struct.
  */
class VABASE_API CVADirectivityInfo
{
public:
	//! Directivity class definitions
	enum DirectivityClass
	{
		UNSPECIFIED = -1,
		PROTOTYPE = 0,
		IMPULSE_RESPONSE,
		TRANSFER_FUNCTION,
		ENERGETIC_BAND_SPECTRUM,
	};

	int iID;					//!< Identifier
	int iClass;					//!< Class identifier
	std::string sName;			//!< Name
	std::string sDesc;			//!< Verbose description
	int iReferences;			//!< Number of references
	CVAStruct oParams; 			//!< Special parameters (including file path etc.)

	//! Default constructor inits some values
	inline CVADirectivityInfo()
		: iID( -1 )
		, iClass( UNSPECIFIED )
		, iReferences( 0 )
	{
	};

	//! Initializing constructor
	/**
	  * @param[in] iID Identifier
	  * @param[in] iClass Directivity class
	  * @param[in] sName Name of directivity
	  * @param[in] sDesc Brief description of the directivity
	  * @param[in] iReferences Usage reference count of directivity data
	  * @param[in] oParams Special parameters
	  */
	inline CVADirectivityInfo( const int iID, const int iClass, const std::string& sName, const std::string& sDesc, const int iReferences, const CVAStruct& oParams )
		: iID( iID )
		, iClass( iClass )
		, sName( sName )
		, sDesc( sDesc )
		, iReferences( iReferences )
		, oParams( oParams )
	{
	};
};


//! Data class for signal source information
/**
  * For info getter.
  */
class VABASE_API CVASignalSourceInfo
{
public:
	//! Signal source type
	enum Type
	{
		UNSPECIFIED = -1,
		FILE = 0,
		SAMPLER,
		DEVICE_INPUT,
		NETSTREAM,
		MACHINE,
		ENGINE
	};

	std::string	sID;			//!< ID (i.e. "af1")
	int	iType;					//!< Type
	std::string sName;			//!< Name (i.e. "Blue horn 2")
	std::string sDesc;			//!< Description
	std::string sState;			//!< State description
	int iReferences;			//!< Reference counter

	//! Default constructor
	inline CVASignalSourceInfo()
		: iReferences( 0 )
		, iType( UNSPECIFIED )
	{};

	// Signal source destructor
	virtual inline ~CVASignalSourceInfo() {};

	//! Initializing constructor
	/**
	  * @param[in] sID Identifier string
	  * @param[in] iType Signal source type
	  * @param[in] sName Name of signal source info
	  * @param[in] sDesc Brief description of signal source info
	  * @param[in] sState State description of signal source info
	  * @param[in] iReference Usage reference counter
	  */
	inline CVASignalSourceInfo( const std::string& sID, const int iType, const std::string& sName, const std::string& sDesc, const std::string& sState, const int iReferences )
		: sID( sID )
		, iType( iType )
		, sName( sName )
		, sDesc( sDesc )
		, sState( sState )
		, iReferences( iReferences )
	{
	};
};


//! Data class containing information of a virtual acoustic scene
/**
  * For scene getter.
  */
class VABASE_API CVASceneInfo
{
public:
	std::string sID;	//!< Scene identifier
	std::string sName;	//!< Scene name
	bool bEnabled; //!< Enabled/disabled flag
	CVAStruct oParams; //!< Scene parameters

	//! Constructor settng scene info as enabled
	inline CVASceneInfo()
		: bEnabled( true )
	{
	};

	//! Scene info destructor
	inline virtual ~CVASceneInfo()
	{
	};
};


//! Data class describing states of audio streams
/**
  * For audio stream state getter.
  */
class VABASE_API CVAAudiostreamState
{
public:
	int64_t	i64Sample;	//!< Index of the first sample since last clock reset
	double dSysTime;	//!< Associated system time in time [s] (internal core clock) of the first sample of the requested output block.
	double dCoreTime;	//!< Associated playback time in core clock time [s] (world time) of the first sample of the requested output block.
	bool bTimeReset;	//!<  Flag that indicates a discontinous change of the playback time (world time)

	//!< Synchronise commands on the stream
	/**  Flag that indicates, that a synchronized
	  * sequence of atomic commands is executed on
	  * elements of the stream. If this flag is true
	  * the source shall handle parameter
	  * changes and buffer them internally, but
	  * first apply them for the stream, if the
	  * flag is set to false again.
	  * Example: Synchronized start/stop of file sources
	  */
	bool bSyncMod;

	//! Destructor
	inline virtual ~CVAAudiostreamState() {};
};


//! Data class containing information of a sound source (acoustic actuator)
/**
  * For sound source info getter.
  */
class VABASE_API CVASoundSourceInfo
{
public:
	int iID;				//!< ID
	std::string sName;		//!< Displayed name
	double dSpoundPower;	//!< Sound power [W]
	bool bMuted;			//!< Muted?
	int iSignalSourceID;	//!< ID of assigned signal source (-1 => none)
	int iDirectivityID;		//!< ID of assigned directivity (-1 => none)
	int iAuraMode;			//!< Auralization mode (flags)

	VAVec3 v3Pos;			//!< Position vector of location between ears [m]
	VAVec3 v3View, v3Up;	//!< Orientation of ears/eyes (view and up vectors in OpenGL convention)
	VAQuat qOrient;			//!< Orientation of ears/eyes (quaternion)

	CVAStruct oParams;		//!< Special parameters

	//! Constructor for sound source info
	inline CVASoundSourceInfo()
		: iID( -1 )
		, bMuted( false )
		, dSpoundPower( g_dSoundPower_94dB_SPL_1m )
		, iSignalSourceID( -1 )
		, iDirectivityID( -1 )
		, iAuraMode( -1 )
	{};

	//! Destructor
	virtual inline ~CVASoundSourceInfo() {};
};


//! Data class containing information of a sound receiver (acoustic receiver)
/**
  * For sound source info receiver.
  */
class VABASE_API CVASoundReceiverInfo
{
public:
	int iID;				//!< ID
	std::string sName;		//!< Displayed name
	int iDirectivityID;		//!< ID of assigned directivity (-1 => none)
	int iAuraMode;			//!< Auralization mode (flags)
	double dSensivitity;	//!< Sensitivity of receiving entity
	bool bEnabled; 			//!< Enabled/disable from processing

	VAVec3 v3Pos;			//!< Position vector of location between ears [m]
	VAVec3 v3View;//!< Orientation of ears/eyes (view and up vectors in OpenGL convention)
	VAVec3 v3Up;		//!< Orientation of ears/eyes (view and up vectors in OpenGL convention)
	VAQuat qOrient;			//!< Orientation of ears/eyes (quaternion)
	VAQuat qHeadAboveTorsoOrientation;	//!< Orientation of head above torso relative to ears

	VAVec3 v3RealWorldPos;					//!< Position vector of location between ears [m] in real world (from tracking)
	VAVec3 v3RealWorldView; //!< Orientation of ears/eyes (view and up vectors in OpenGL convention) in real world (from tracking)
	VAVec3 v3RealWorldUp;	//!< Orientation of ears/eyes (view and up vectors in OpenGL convention) in real world (from tracking)
	VAQuat qRealWorldOrient;				//!< Orientation of ears/eyes (quaternion) in real world (from tracking)
	VAQuat qRealWorldHeadAboveTorsoOrientation;	//!< Orientation of head above torso in real world (from tracking) relative to ears

	CVAStruct oParams; 	//!< Special parameters

	//! Constructor for sound receiver info
	inline CVASoundReceiverInfo()
		: iID( -1 )
		, iDirectivityID( -1 )
		, iAuraMode( -1 )
		, dSensivitity( 1.0f )
		, bEnabled( true )
	{};

	//! Destructor
	virtual inline ~CVASoundReceiverInfo() {};
};


//! Data class containing information of a sound portal (acoustic portal)
class VABASE_API CVASoundPortalInfo
{
public:
	int iID;			//!< ID
	std::string sName;	//!< Displayed name (optional)
	bool bEnabled; //!< Enabled flag

	int iMaterialID; //!< Material ID with transmission data
	int iNextPortalID; //!< Next sound portal identifier, if existing
	int iSoundReceiverID; //!< Source receiver identifier, if existing
	int iSoundSourceID; //!< Source sound identifier, if existing

	VAVec3 v3Pos;		//!< Position vector [m]
	VAVec3 v3View, v3Up;	//!< View-/Up-vector
	VAQuat qOrient;		//!< Orientation (quaternion)

	CVAStruct oParams; 	//!< Special parameters

	//! Default constructor for sound portal
	inline CVASoundPortalInfo()
		: iID( -1 )
		, iNextPortalID( -1 )
		, iSoundSourceID( -1 )
		, iSoundReceiverID( -1 )
		, iMaterialID( -1 )
	{};

	//! Destructor
	virtual inline ~CVASoundPortalInfo() {};
};


//! Data class containing information of an acoustic material
/**
  * This data class holds information on acoustic materials such as
  * absorption, scattering and transmission coefficients. Data has
  * a type, a data vector for floating point values and a set of parametres.
  */
class VABASE_API CVAAcousticMaterial
{
public:
	//! Material base frequency resolution
	enum Type
	{
		UNSPECIFIED = -1,
		PROTOTYPE = 0,
		ENERGETIC_BAND_FILTER_WHOLE_OCTAVE,
		ENERGETIC_BAND_FILTER_THIRD_OCTAVE,
	};

	int iID;			//!< Material identifier
	int iType; //!< Type, see #Type
	std::string sName; //!< Verbatim name
	CVAStruct oParams; //!< Parameters for prototyping
	std::vector< float > vfAbsorptionValues; //!< Absorption values
	std::vector< float > vfScatteringValues; //!< Scattering values
	std::vector< float > vfTransmissionValues; //!< Transmission values

	//! Instantiate empty material
	inline CVAAcousticMaterial()
		: iID( -1 )
	{};

	//! Destructor
	virtual inline ~CVAAcousticMaterial() {};
};



//! Geometry mesh class
/**
  * This is only intended to be used as an exchange
  * format of the simplest mesh structure. Renderers
  * use more elaborate representations.
  *
  * It can be used for rudimentary rooms and dynamic geometry objects.
  */
class VABASE_API CVAGeometryMesh
{
public:
	//! Vertex representation
	class CVAVertex
	{
	public:
		int iID; //!< Vertex identifier
		VAVec3 v3Point; //!< Vertext point / position

		//! Constructor initializing defaults
		inline CVAVertex() : iID( -1 ) {};
	};

	//! Face representation
	class CVAFace
	{
	public:
		int iID; //!< Face identifier
		int iMaterialID; //!< Assigned material
		std::vector< int > viVertexList; //!< Face vertices
		
		//! Constructor initializing defaults
		inline CVAFace() : iID( -1 ), iMaterialID( -1 ) {};
	};

	int iID; //!< Geometry mesh identifier
	bool bEnabled; //!< Enabled flag
	std::vector< CVAVertex > voVertices; //!< List of available vertices
	std::vector< CVAFace > voFaces; //!< List of faces defined by vertices
	CVAStruct oParams; //!< Additional parameters

	//! Constructor initializing members
	inline CVAGeometryMesh()
		: iID( -1 )
		, bEnabled( true )
	{};
};

#endif // IW_VABASE
