// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "VAPlugin/Public/VASourceComponent.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeVASourceComponent() {}
// Cross Module References
	VAPLUGIN_API UEnum* Z_Construct_UEnum_VAPlugin_EMovement();
	UPackage* Z_Construct_UPackage__Script_VAPlugin();
	VAPLUGIN_API UClass* Z_Construct_UClass_UVASourceComponent_NoRegister();
	VAPLUGIN_API UClass* Z_Construct_UClass_UVASourceComponent();
	ENGINE_API UClass* Z_Construct_UClass_UActorComponent();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FRotator();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
// End Cross Module References
	static UEnum* EMovement_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_VAPlugin_EMovement, Z_Construct_UPackage__Script_VAPlugin(), TEXT("EMovement"));
		}
		return Singleton;
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EMovement(EMovement_StaticEnum, TEXT("/Script/VAPlugin"), TEXT("EMovement"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_VAPlugin_EMovement_CRC() { return 266054372U; }
	UEnum* Z_Construct_UEnum_VAPlugin_EMovement()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_VAPlugin();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EMovement"), 0, Get_Z_Construct_UEnum_VAPlugin_EMovement_CRC(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "ObjectSpawnPoint", (int64)ObjectSpawnPoint },
				{ "MoveWithObject", (int64)MoveWithObject },
				{ "OwnPosition", (int64)OwnPosition },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "ModuleRelativePath", "Public/VASourceComponent.h" },
				{ "ToolTip", "struct used to get" },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_VAPlugin,
				UE4CodeGen_Private::EDynamicType::NotDynamic,
				"EMovement",
				RF_Public|RF_Transient|RF_MarkAsNative,
				nullptr,
				(uint8)UEnum::ECppForm::Regular,
				"EMovement",
				Enumerators,
				ARRAY_COUNT(Enumerators),
				METADATA_PARAMS(Enum_MetaDataParams, ARRAY_COUNT(Enum_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUEnum(ReturnEnum, EnumParams);
		}
		return ReturnEnum;
	}
	void UVASourceComponent::StaticRegisterNativesUVASourceComponent()
	{
	}
	UClass* Z_Construct_UClass_UVASourceComponent_NoRegister()
	{
		return UVASourceComponent::StaticClass();
	}
	struct Z_Construct_UClass_UVASourceComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vRot_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_vRot;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vPos_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_vPos;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vMovement_MetaData[];
#endif
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_vMovement;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vDelay_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_vDelay;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vLoop_MetaData[];
#endif
		static void NewProp_vLoop_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_vLoop;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vSoundName_MetaData[];
#endif
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_vSoundName;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vGainOffset_MetaData[];
#endif
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_vGainOffset;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UVASourceComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UActorComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_VAPlugin,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVASourceComponent_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Custom" },
		{ "IncludePath", "VASourceComponent.h" },
		{ "ModuleRelativePath", "Public/VASourceComponent.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vRot_MetaData[] = {
		{ "Category", "VASourceComponent" },
		{ "DisplayName", "Rotation" },
		{ "ModuleRelativePath", "Public/VASourceComponent.h" },
		{ "ToolTip", "Manual Rotation (if used)" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vRot = { UE4CodeGen_Private::EPropertyClass::Struct, "vRot", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0040000000000001, 1, nullptr, STRUCT_OFFSET(UVASourceComponent, vRot), Z_Construct_UScriptStruct_FRotator, METADATA_PARAMS(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vRot_MetaData, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vRot_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vPos_MetaData[] = {
		{ "Category", "VASourceComponent" },
		{ "DisplayName", "Position" },
		{ "ModuleRelativePath", "Public/VASourceComponent.h" },
		{ "ToolTip", "Manual Position (if used)" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vPos = { UE4CodeGen_Private::EPropertyClass::Struct, "vPos", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0040000000000001, 1, nullptr, STRUCT_OFFSET(UVASourceComponent, vPos), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vPos_MetaData, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vPos_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vMovement_MetaData[] = {
		{ "Category", "VASourceComponent" },
		{ "CustomStructureParam", "Objects Spawn Point, Move With Object, Own Position (below)" },
		{ "DisplayName", "Position Settings" },
		{ "ModuleRelativePath", "Public/VASourceComponent.h" },
		{ "ToolTip", "Decide whether to use manual Transform (below) or use Transform / Movement of Actor" },
	};
#endif
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vMovement = { UE4CodeGen_Private::EPropertyClass::Byte, "vMovement", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0040000000000001, 1, nullptr, STRUCT_OFFSET(UVASourceComponent, vMovement), Z_Construct_UEnum_VAPlugin_EMovement, METADATA_PARAMS(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vMovement_MetaData, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vMovement_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vDelay_MetaData[] = {
		{ "Category", "VASourceComponent" },
		{ "DisplayName", "Starting time (s)" },
		{ "ModuleRelativePath", "Public/VASourceComponent.h" },
		{ "ToolTip", "Start in x Seconds" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vDelay = { UE4CodeGen_Private::EPropertyClass::Float, "vDelay", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0040000000000001, 1, nullptr, STRUCT_OFFSET(UVASourceComponent, vDelay), METADATA_PARAMS(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vDelay_MetaData, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vDelay_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vLoop_MetaData[] = {
		{ "Category", "VASourceComponent" },
		{ "DisplayName", "Loop" },
		{ "ModuleRelativePath", "Public/VASourceComponent.h" },
		{ "ToolTip", "Loop sound?" },
	};
#endif
	void Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vLoop_SetBit(void* Obj)
	{
		((UVASourceComponent*)Obj)->vLoop = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vLoop = { UE4CodeGen_Private::EPropertyClass::Bool, "vLoop", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0040000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UVASourceComponent), &Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vLoop_SetBit, METADATA_PARAMS(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vLoop_MetaData, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vLoop_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vSoundName_MetaData[] = {
		{ "Category", "VASourceComponent" },
		{ "DisplayName", "Sound Name" },
		{ "ModuleRelativePath", "Public/VASourceComponent.h" },
		{ "ToolTip", "In- or decrease Gain Offset" },
	};
#endif
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vSoundName = { UE4CodeGen_Private::EPropertyClass::Str, "vSoundName", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0040000000000001, 1, nullptr, STRUCT_OFFSET(UVASourceComponent, vSoundName), METADATA_PARAMS(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vSoundName_MetaData, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vSoundName_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vGainOffset_MetaData[] = {
		{ "Category", "VASourceComponent" },
		{ "DisplayName", "(in work) Gain" },
		{ "ModuleRelativePath", "Public/VASourceComponent.h" },
		{ "ToolTip", "In- or decrease Gain Offset  TODO not working yet" },
	};
#endif
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vGainOffset = { UE4CodeGen_Private::EPropertyClass::Int, "vGainOffset", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0040000000000001, 1, nullptr, STRUCT_OFFSET(UVASourceComponent, vGainOffset), METADATA_PARAMS(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vGainOffset_MetaData, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vGainOffset_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UVASourceComponent_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vRot,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vPos,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vMovement,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vDelay,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vLoop,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vSoundName,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UVASourceComponent_Statics::NewProp_vGainOffset,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UVASourceComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UVASourceComponent>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UVASourceComponent_Statics::ClassParams = {
		&UVASourceComponent::StaticClass,
		DependentSingletons, ARRAY_COUNT(DependentSingletons),
		0x00B000A4u,
		nullptr, 0,
		Z_Construct_UClass_UVASourceComponent_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::PropPointers),
		"Engine",
		&StaticCppClassTypeInfo,
		nullptr, 0,
		METADATA_PARAMS(Z_Construct_UClass_UVASourceComponent_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UVASourceComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UVASourceComponent()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UVASourceComponent_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UVASourceComponent, 1370406422);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UVASourceComponent(Z_Construct_UClass_UVASourceComponent, &UVASourceComponent::StaticClass, TEXT("/Script/VAPlugin"), TEXT("UVASourceComponent"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UVASourceComponent);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
