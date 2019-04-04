// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "VAPlugin/Public/VAReceiverActor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeVAReceiverActor() {}
// Cross Module References
	VAPLUGIN_API UEnum* Z_Construct_UEnum_VAPlugin_EAdress();
	UPackage* Z_Construct_UPackage__Script_VAPlugin();
	VAPLUGIN_API UEnum* Z_Construct_UEnum_VAPlugin_EDir();
	VAPLUGIN_API UClass* Z_Construct_UClass_AVAReceiverActor_NoRegister();
	VAPLUGIN_API UClass* Z_Construct_UClass_AVAReceiverActor();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
// End Cross Module References
	static UEnum* EAdress_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_VAPlugin_EAdress, Z_Construct_UPackage__Script_VAPlugin(), TEXT("EAdress"));
		}
		return Singleton;
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EAdress(EAdress_StaticEnum, TEXT("/Script/VAPlugin"), TEXT("EAdress"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_VAPlugin_EAdress_CRC() { return 4182445685U; }
	UEnum* Z_Construct_UEnum_VAPlugin_EAdress()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_VAPlugin();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EAdress"), 0, Get_Z_Construct_UEnum_VAPlugin_EAdress_CRC(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "localhost", (int64)localhost },
				{ "Cave", (int64)Cave },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "ModuleRelativePath", "Public/VAReceiverActor.h" },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_VAPlugin,
				UE4CodeGen_Private::EDynamicType::NotDynamic,
				"EAdress",
				RF_Public|RF_Transient|RF_MarkAsNative,
				nullptr,
				(uint8)UEnum::ECppForm::Regular,
				"EAdress",
				Enumerators,
				ARRAY_COUNT(Enumerators),
				METADATA_PARAMS(Enum_MetaDataParams, ARRAY_COUNT(Enum_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUEnum(ReturnEnum, EnumParams);
		}
		return ReturnEnum;
	}
	static UEnum* EDir_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_VAPlugin_EDir, Z_Construct_UPackage__Script_VAPlugin(), TEXT("EDir"));
		}
		return Singleton;
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EDir(EDir_StaticEnum, TEXT("/Script/VAPlugin"), TEXT("EDir"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_VAPlugin_EDir_CRC() { return 4074134061U; }
	UEnum* Z_Construct_UEnum_VAPlugin_EDir()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_VAPlugin();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EDir"), 0, Get_Z_Construct_UEnum_VAPlugin_EDir_CRC(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "DefaultHRIR", (int64)DefaultHRIR },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "ModuleRelativePath", "Public/VAReceiverActor.h" },
				{ "ToolTip", "struct used to input Directivity via Variable in UE4 Editor" },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_VAPlugin,
				UE4CodeGen_Private::EDynamicType::NotDynamic,
				"EDir",
				RF_Public|RF_Transient|RF_MarkAsNative,
				nullptr,
				(uint8)UEnum::ECppForm::Regular,
				"EDir",
				Enumerators,
				ARRAY_COUNT(Enumerators),
				METADATA_PARAMS(Enum_MetaDataParams, ARRAY_COUNT(Enum_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUEnum(ReturnEnum, EnumParams);
		}
		return ReturnEnum;
	}
	void AVAReceiverActor::StaticRegisterNativesAVAReceiverActor()
	{
	}
	UClass* Z_Construct_UClass_AVAReceiverActor_NoRegister()
	{
		return AVAReceiverActor::StaticClass();
	}
	struct Z_Construct_UClass_AVAReceiverActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vPort_MetaData[];
#endif
		static const UE4CodeGen_Private::FUInt16PropertyParams NewProp_vPort;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vAdress_MetaData[];
#endif
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_vAdress;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vScale_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_vScale;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vDirectivity_MetaData[];
#endif
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_vDirectivity;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_vGainFactor_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_vGainFactor;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AVAReceiverActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_VAPlugin,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AVAReceiverActor_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "VAReceiverActor.h" },
		{ "ModuleRelativePath", "Public/VAReceiverActor.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vPort_MetaData[] = {
		{ "Category", "VAReceiverActor" },
		{ "ClampMax", "65535" },
		{ "ClampMin", "0" },
		{ "DisplayName", "Port [0, 65535]" },
		{ "ModuleRelativePath", "Public/VAReceiverActor.h" },
		{ "ToolTip", "Factor for global output Gain" },
		{ "UIMax", "65535" },
		{ "UIMin", "0" },
	};
#endif
	const UE4CodeGen_Private::FUInt16PropertyParams Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vPort = { UE4CodeGen_Private::EPropertyClass::UInt16, "vPort", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(AVAReceiverActor, vPort), METADATA_PARAMS(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vPort_MetaData, ARRAY_COUNT(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vPort_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vAdress_MetaData[] = {
		{ "Category", "VAReceiverActor" },
		{ "DisplayName", "Usecase" },
		{ "ModuleRelativePath", "Public/VAReceiverActor.h" },
		{ "ToolTip", "Choose Where To use the Plugin (Important for IP Adress and Port)" },
	};
#endif
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vAdress = { UE4CodeGen_Private::EPropertyClass::Byte, "vAdress", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(AVAReceiverActor, vAdress), Z_Construct_UEnum_VAPlugin_EAdress, METADATA_PARAMS(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vAdress_MetaData, ARRAY_COUNT(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vAdress_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vScale_MetaData[] = {
		{ "Category", "VAReceiverActor" },
		{ "DisplayName", "Scale" },
		{ "ModuleRelativePath", "Public/VAReceiverActor.h" },
		{ "ToolTip", "How many units in UE equal 1m in World" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vScale = { UE4CodeGen_Private::EPropertyClass::Float, "vScale", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(AVAReceiverActor, vScale), METADATA_PARAMS(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vScale_MetaData, ARRAY_COUNT(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vScale_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vDirectivity_MetaData[] = {
		{ "Category", "VAReceiverActor" },
		{ "DisplayName", "Directivity" },
		{ "ModuleRelativePath", "Public/VAReceiverActor.h" },
		{ "ToolTip", "Choose Directivity File for Receiver" },
	};
#endif
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vDirectivity = { UE4CodeGen_Private::EPropertyClass::Byte, "vDirectivity", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(AVAReceiverActor, vDirectivity), Z_Construct_UEnum_VAPlugin_EDir, METADATA_PARAMS(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vDirectivity_MetaData, ARRAY_COUNT(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vDirectivity_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vGainFactor_MetaData[] = {
		{ "Category", "VAReceiverActor" },
		{ "ClampMax", "1.0" },
		{ "ClampMin", "0.0" },
		{ "DisplayName", "Gain Factor [0,1]" },
		{ "ModuleRelativePath", "Public/VAReceiverActor.h" },
		{ "ToolTip", "Factor for global output Gain" },
		{ "UIMax", "1.0" },
		{ "UIMin", "0.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vGainFactor = { UE4CodeGen_Private::EPropertyClass::Float, "vGainFactor", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(AVAReceiverActor, vGainFactor), METADATA_PARAMS(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vGainFactor_MetaData, ARRAY_COUNT(Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vGainFactor_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AVAReceiverActor_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vPort,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vAdress,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vScale,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vDirectivity,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVAReceiverActor_Statics::NewProp_vGainFactor,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AVAReceiverActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AVAReceiverActor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AVAReceiverActor_Statics::ClassParams = {
		&AVAReceiverActor::StaticClass,
		DependentSingletons, ARRAY_COUNT(DependentSingletons),
		0x009000A0u,
		nullptr, 0,
		Z_Construct_UClass_AVAReceiverActor_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UClass_AVAReceiverActor_Statics::PropPointers),
		nullptr,
		&StaticCppClassTypeInfo,
		nullptr, 0,
		METADATA_PARAMS(Z_Construct_UClass_AVAReceiverActor_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_AVAReceiverActor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AVAReceiverActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AVAReceiverActor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AVAReceiverActor, 3552912630);
	static FCompiledInDefer Z_CompiledInDefer_UClass_AVAReceiverActor(Z_Construct_UClass_AVAReceiverActor, &AVAReceiverActor::StaticClass, TEXT("/Script/VAPlugin"), TEXT("AVAReceiverActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AVAReceiverActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
