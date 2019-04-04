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

#ifndef IW_VABASE_STRUCT
#define IW_VABASE_STRUCT

// VA includes
#include <VABaseDefinitions.h>
#include <VASamples.h>

// STL includes
#include <algorithm>
#include <iterator>
#include <map>
#include <ostream>
#include <cstring>

// Forward declarations
class VABASE_API CVAStruct;
class VABASE_API CVAStructValue;

//! Case insensitive string comparison
class VABASE_API CompareStringCaseInsensitive
{
public:
	//! Compare operator
	/**
	  * @param[in] a Left-hand string
	  * @param[in] b Left-hand string
	  * @return True, if strings are equal (ignoring case)
	  */
	inline bool operator()( const std::string& a, const std::string& b ) const
	{
		std::string la( a ), lb( b );
		std::transform( la.begin(), la.end(), la.begin(), ::tolower );
		std::transform( lb.begin(), lb.end(), lb.begin(), ::tolower );
		return la < lb;
	};
};

//! Associative array container
/**
  * Associative array container, mapping a unique collection of
  * string keys to values. A variety of key values is supported,
  * for instance primitive types (bool, int, double, string).
  * Nesting is also supported by assigning keys another
  * associative array. Keys names are case-insensitive.
  * The class features simple and effective use by defining
  * data access and cast operators.
  * It can be efficiently serialized/deserialized,
  * making it a suitable container for message mechanisms.
  */
class VABASE_API CVAStruct
{
public:
	// Type aliases
	typedef std::map< std::string, CVAStructValue, CompareStringCaseInsensitive >::iterator iterator;
	typedef std::map< std::string, CVAStructValue, CompareStringCaseInsensitive >::const_iterator const_iterator;

	const static char DEFAULT_PATH_SEPARATOR = '/'; //!< Literals

	//! Default constructor
	CVAStruct();

	//! Copy constructor
	/**
	  * @param[in] oOther Other struct
	  */
	CVAStruct( const CVAStruct& oOther );

	//! Destructor
	virtual ~CVAStruct();

	//! Returns the number of keys (elements)
	/**
	  * @return Number of keys / elements
	  */
	int Size() const;

	//! Returns if the container is empty
	/**
	  * @return True, if there are no keys
	  */
	bool IsEmpty() const;

	//! Returns if the given key exists
	/**
	  * @param[in] sKey Key as string
	  * @return True, if key is present
	  */
	bool HasKey( const std::string& sKey ) const;

	//! Deletes all keys
	void Clear();

	//! Removes a key and its value (supports paths)
	/**
	  * @param[in] sKey Key as string
	  * @return True, if key could be removed
	  */
	bool RemoveKey( const std::string& sKey );

	//! Return the pointer to a key  (read-only)
	/**
	  * The key can be specified either by a name or a path.
	  * In the latter case the method traverses through the structure.
	  *
	  * @note This functions never creates a non-existent key
	  * @note This functions is more complex than GetKey(const std::string&)
	  *
	  * @param[in] sPath Path
	  * @param[in] cPathSeparator Path seperator, either "/" (default) or "\"
	  *
	  * @return Valid pointer, if the given key exists, nullptr otherwise
	  */
	const CVAStructValue* GetValue( const std::string& sPath, const char cPathSeparator = DEFAULT_PATH_SEPARATOR ) const;

	//! Return the pointer to a key (read-write)
	/**
	  * The key can be specified either by a name or a path.
	  * In the latter case the method traverses through the structure.
	  *
	  * @param[in] sPath Path
	  * @param[in] cPathSeparator Path seperator, either "/" (default) or "\"
	  * @note This functions never creates a non-existent key
	  * @note This functions is more complex than GetKey(const std::string&)
	  *
	  * @return Valid pointer, if the given key exists, nullptr otherwise
	  */
	CVAStructValue* GetValue( const std::string& sPath, const char cPathSeparator = DEFAULT_PATH_SEPARATOR );

	//! Read-only data access
	/**
	  * @param[in] pcKey Key as character pointer
	  * @return Struct value of this key
	  * @note Will raise CVAEception on error
	  */
	const CVAStructValue& operator[]( const char* pcKey ) const;

	//! Read-only data access
	/**
	  * @param[in] sKey Key as string
	  * @return Struct value of this key
	  * @note Will raise CVAEception on error
	  */
	inline const CVAStructValue& operator[]( const std::string& sKey ) const
	{
		return ( *this )[ sKey.c_str() ];
	};

	//! Read/write data access
	/**
	  * @param[in] pcKey Key as character pointer
	  * @return Struct value of this key
	  * @note Will raise CVAEception on error
	  */
	CVAStructValue& operator[]( const char* pcKey );

	//! Read/write data access
	/**
	  * @param[in] sKey Key as string
	  * @return Struct value of this key
	  * @note Will raise CVAEception on error
	  */
	inline CVAStructValue& operator[]( const std::string& sKey )
	{
		return ( *this )[ sKey.c_str() ];
	};

	//! Assignment
	/**
	  * @param[in] rhs Right hand side struct
	  * @return Assigned struct
	  */
	CVAStruct& operator=( const CVAStruct& rhs );

	//! Iterator pattern begin
	/**
	  * @return Iterator beginning
	  */
	CVAStruct::iterator Begin();

	//! Const iterator pattern begin
	/**
	  * @return Const iterator beginning
	  */
	CVAStruct::const_iterator Begin() const;

	//! Iterator pattern end
	/**
	  * @return Const iterator end
	  */
	CVAStruct::iterator End();

	//! Const iterator pattern end
	/**
	  * @return Const iterator end
	  */
	CVAStruct::const_iterator End() const;

	//! Merge struct with another one
	/**
	  * @param[in] rhs Right hand side struct where to merge from
	  * @param[in] bUnique Raise exception in case uniqueness is violated
	  */
	void Merge( const CVAStruct& rhs, const bool bUnique = true );

	//! Return a string representation of the container
	/**
	  * @param[in] iIndent Indentation for printing
	  * @return Formatted string
	  */
	std::string ToString( const int iIndent = 0 ) const;
private:
	typedef std::map<std::string, CVAStructValue, CompareStringCaseInsensitive> Map; //!< Key-value map typedef
	typedef Map::const_iterator MapCit; //!< Map const iterator typedef
	typedef Map::iterator MapIt; //!< Map iterator typedef

	Map m_mData; //!< Internal key-value data map

	//! Traversal methods for read-only access
	/**
	  * @param[in] sPath Path
	  * @param[in] iPathCursor Path cursor
	  * @param[in] cPathSeparator Path separator
	  * @return Pointer to struct value
	  */
	const CVAStructValue* Traverse( const std::string& sPath, size_t iPathCursor = 0, const char cPathSeparator = DEFAULT_PATH_SEPARATOR ) const;

	//! Traversal methods
	/**
	  * @param[in] sPath Path
	  * @param[in] iPathCursor Path cursor
	  * @param[in] cPathSeparator Path separator
	  * @return Pointer to struct value
	  */
	CVAStructValue* Traverse( const std::string& sPath, size_t iPathCursor = 0, const char cPathSeparator = DEFAULT_PATH_SEPARATOR );
};

//! Associative array value class
/**
  * This class realized keys is the associative array CVAStruct
  */
class VABASE_API CVAStructValue
{
public:
	//! Key types
	enum KeyType
	{
		UNASSIGNED = -1,	//!< Uninitialized key without a type
		BOOL = 0,			//!< Boolean key
		INT,			//!< Integer key
		DOUBLE,			//!< Double-precision floating-point key
		STRING,			//!< String key
		STRUCT,			//!< Associative array key (CVAStruct key)
		DATA,			//!< Binary data (BLOB)
		SAMPLEBUFFER,	//!< VA sample buffer
	};

	//! Default constructor. Creates an uninitialized key without a type
	CVAStructValue();

	//! Initialization constructors
	/**
	  * @param[in] value Value as bool
	  */
	CVAStructValue( const bool value );

	//! Initialization constructors
	/**
	  * @param[in] value Value as int
	  */
	CVAStructValue( const int value );

	//! Initialization constructors
	/**
	  * @param[in] value Value as double
	  */
	CVAStructValue( const double value );

	//! Initialization constructors
	/**
	  * @param[in] value Value as character pointer
	  */
	CVAStructValue( const char* value );

	//! Initialization constructors
	/**
	  * @param[in] value Value as string
	  */
	CVAStructValue( const std::string& value );

	//! Initialization constructors
	/**
	  * @param[in] value Value as another struct (nested object)
	  */
	CVAStructValue( const CVAStruct& value );

	//! Initialization constructors
	/**
	  * @param[in] pData Value as byte array (data pointer)
	  * @param[in] iBytes Value byte count
	  */
	CVAStructValue( void* pData, const int iBytes );

	//! Initialization constructors
	/**
	  * @param[in] value Value
	  */
	CVAStructValue( const CVASampleBuffer& value );

	//! Copy constructor
	/**
	  * @param[in] oOther Copy from struct value
	  */
	CVAStructValue( const CVAStructValue& oOther );

	//! Destructor
	virtual ~CVAStructValue();

	//! Returns the datatype of the key
	/**
	  * @return Data type identifier
	  */
	int GetDatatype() const;

	//! Returns the data size in bytes (only for datatype DATA)
	/**
	  * @return Data size number (bytes)
	  */
	int GetDataSize() const;

	//! Returns a pointer to the data (only for datatype DATA)
	/**
	  * @return Generic pointer in data (read-only)
	  */
	const void* GetData() const;

	//! Returns a pointer to the data (only for datatype DATA)
	/**
	  * @return Generic pointer in data
	  */
	void* GetData();

	//! Sets the data (only for datatype DATA)
	/**
	  * @param[in] pData Data pointer
	  * @param[in] iBytes Number of bytes
	  */
	void SetData( const void* pData, const int iBytes );

	//! Returns a pointer to the struct value (only for datatype STRUCT)
	/**
	  * @return Struct value read-only
	  */
	const CVAStruct& GetStruct() const;

	//! Returns a const pointer to the struct value (only for datatype STRUCT)
	/**
	  * @return Struct value
	  */
	CVAStruct& GetStruct();

	//! Test functions for datatypes
	/**
	  * @return True, if data type has been assigned
	  */
	bool IsAssigned() const;

	//! Data type is boolean
	/**
	  * @return True, if type matches
	  */
	bool IsBool() const;

	//! Data type is integer
	/**
	  * @return True, if type matches
	  */
	bool IsInt() const;

	//! Data type is double
	/**
	  * @return True, if type matches
	  */
	bool IsDouble() const;

	//! Data type is string
	/**
	  * @return True, if type matches
	  */
	bool IsString() const;

	//! Data type is struct
	/**
	  * @return True, if type matches
	  */
	bool IsStruct() const;

	//! Data type is integer or double
	/**
	  * @return True, if type matches
	  */
	bool IsNumeric() const;

	//! Data type is data
	/**
	  * @return True, if type matches
	  */
	bool IsData() const;

	//! Data type is sample buffer
	/**
	  * @return True, if type matches
	  */
	bool IsSampleBuffer() const;

	//! Assignment operators for type struct value
	/**
	  * @param[in] v Value as struct
	  * @return Struct value
	  */
	CVAStructValue& operator=( const CVAStructValue& v );

	//! Assignment operators for type bool value
	/**
	  * @param[in] v Value as bool
	  * @return Struct value
	  */
	CVAStructValue& operator=( const bool v );

	//! Assignment operators for type int value
	/**
	  * @param[in] v Value as int
	  * @return Struct value
	  */
	CVAStructValue& operator=( const int v );

	//! Assignment operators for type double value
	/**
	  * @param[in] v Value as double
	  * @return Struct value
	  */
	CVAStructValue& operator=( const double v );

	//! Assignment operators for type character pointer value
	/**
	  * @param[in] v Value as character pointer
	  * @return Struct value
	  */
	CVAStructValue& operator=( const char* v );

	//! Assignment operators for type string value
	/**
	  * @param[in] v Value as string
	  * @return Struct value
	  */
	CVAStructValue& operator=( const std::string& v );

	//! Assignment operators for type sample buffer value
	/**
	  * @param[in] v Value as sample buffer
	  * @return Struct value
	  */
	CVAStructValue& operator=( const CVASampleBuffer& v );

	//! Read-only data access
	/**
	  * @note Is applicable only for struct keys. Also raises CVAException on error.
	  * @param[in] pcKey Key as char pointer
	  * @return Struct value
	  */
	const CVAStructValue& operator[]( const char* pcKey ) const;

	//! Read/write data access
	/**
	  * @note Is applicable only for struct keys. Also raises CVAException on error.
	  * @param[in] pcKey Key as char pointer
	  * @return Struct value
	  */
	CVAStructValue& operator[]( const char* pcKey );
	
	//! Read/write data access via string key
	/**
	  * @note Is applicable only for struct keys. Also raises CVAException on error.
	  }
	  * @param[in] sKey Key as string
	  * @return Struct value
	  */
	inline CVAStructValue& operator[]( const std::string& sKey )
	{
		return ( *this )[ sKey.c_str() ];
	};

	//! Cast to bool operator (required for assignments of the form 'bool = CVAStructKey')
	operator bool() const;

	//! Cast to int operator (required for assignments of the form 'int = CVAStructKey')
	operator int() const;

	//! Cast to double operator (required for assignments of the form 'double = CVAStructKey')
	operator double() const;

	//! Cast to std::string operator (required for assignments of the form 'std::string = CVAStructKey')
	operator std::string() const;

	//! Cast to void pointer (required for assignments of the form 'void* = CVAStructKey')
	operator void*( ) const;

	//! Cast to sample buffer
	operator const CVASampleBuffer&( ) const;

	//! Cast to sample buffer
	operator CVASampleBuffer&( );

	//! Cast to const CVAStruct (required for assignments of the form 'CVAStruct = CVAStructKey')
	operator const CVAStruct&( ) const;

	//! Cast to CVAStruct (required for assignments of the form 'CVAStruct = CVAStructKey')
	operator CVAStruct&( );

	//! Return a string representation of the key
	/**
	  * @return Formatted string
	  */
	std::string ToString() const;

private:
	int iDatatype; //!< Data type
	const std::string* psKey;	//!< Back-reference to the keys name

	bool bValue; //!< Value if type is bool
	int iValue; //!< Value if type is int
	double dValue; //!< Value if type is double
	std::string sValue; //!< Value if type is string
	CVAStruct* pStruct; //!< Value if type is struct
	std::vector< char > vcData; //!< Value if type is data
	int iDataSize; //!< Value byte number if type is data
	CVASampleBuffer sbValue; //!< Value if type is sample buffer

	friend class CVAStruct;
};

//! STL stream output operator
/**
  * @param[in] os Outstream object
  * @param[in] s Struct value
  * @return Outstream with info attached
  */
VABASE_API std::ostream& operator<<( std::ostream& os, const CVAStruct& s );

//! STL stream output operator
/**
  * @param[in] os Outstream object
  * @param[in] key Struct value read-only
  * @return Outstream with info attached
  */
VABASE_API std::ostream& operator<<( std::ostream& os, const CVAStructValue& key );

#endif // IW_VABASE_STRUCT
