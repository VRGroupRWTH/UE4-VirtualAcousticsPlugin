// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef VAPLUGIN_VASourceComponent_generated_h
#error "VASourceComponent.generated.h already included, missing '#pragma once' in VASourceComponent.h"
#endif
#define VAPLUGIN_VASourceComponent_generated_h

#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_RPC_WRAPPERS
#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_RPC_WRAPPERS_NO_PURE_DECLS
#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUVASourceComponent(); \
	friend struct Z_Construct_UClass_UVASourceComponent_Statics; \
public: \
	DECLARE_CLASS(UVASourceComponent, UActorComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/VAPlugin"), NO_API) \
	DECLARE_SERIALIZER(UVASourceComponent)


#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_INCLASS \
private: \
	static void StaticRegisterNativesUVASourceComponent(); \
	friend struct Z_Construct_UClass_UVASourceComponent_Statics; \
public: \
	DECLARE_CLASS(UVASourceComponent, UActorComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/VAPlugin"), NO_API) \
	DECLARE_SERIALIZER(UVASourceComponent)


#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UVASourceComponent(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UVASourceComponent) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UVASourceComponent); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UVASourceComponent); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UVASourceComponent(UVASourceComponent&&); \
	NO_API UVASourceComponent(const UVASourceComponent&); \
public:


#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UVASourceComponent(UVASourceComponent&&); \
	NO_API UVASourceComponent(const UVASourceComponent&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UVASourceComponent); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UVASourceComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UVASourceComponent)


#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__vGainOffset() { return STRUCT_OFFSET(UVASourceComponent, vGainOffset); } \
	FORCEINLINE static uint32 __PPO__vSoundName() { return STRUCT_OFFSET(UVASourceComponent, vSoundName); } \
	FORCEINLINE static uint32 __PPO__vLoop() { return STRUCT_OFFSET(UVASourceComponent, vLoop); } \
	FORCEINLINE static uint32 __PPO__vDelay() { return STRUCT_OFFSET(UVASourceComponent, vDelay); } \
	FORCEINLINE static uint32 __PPO__vMovement() { return STRUCT_OFFSET(UVASourceComponent, vMovement); } \
	FORCEINLINE static uint32 __PPO__vPos() { return STRUCT_OFFSET(UVASourceComponent, vPos); } \
	FORCEINLINE static uint32 __PPO__vRot() { return STRUCT_OFFSET(UVASourceComponent, vRot); }


#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_21_PROLOG
#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_PRIVATE_PROPERTY_OFFSET \
	VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_RPC_WRAPPERS \
	VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_INCLASS \
	VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_PRIVATE_PROPERTY_OFFSET \
	VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_RPC_WRAPPERS_NO_PURE_DECLS \
	VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_INCLASS_NO_PURE_DECLS \
	VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h_24_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID VAcousticsModule2_Plugins_VAPlugin_Source_VAPlugin_Public_VASourceComponent_h


#define FOREACH_ENUM_EMOVEMENT(op) \
	op(ObjectSpawnPoint) \
	op(MoveWithObject) \
	op(OwnPosition) 
PRAGMA_ENABLE_DEPRECATION_WARNINGS
