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

/*!

@mainpage

@section vabase_intro_sec Introduction

The Virtual Acoustics project is free, open source real-time auralization framework. It is distributed under the terms of the Apache License Version 2.0.
The official website can be found at http://www.virtualacoustics.org.

@section vabase_sec VABase

VABase provides basic classes and a sophisticated interface calls #IVAInterface that is used throughout the entire

@section vabase_license_sec License

Copyright 2015-2018 Institute of Technical Acoustics (ITA), RWTH Aachen University

Licensed under the Apache License, Version 2.0 (the "License");
you may not use the VA software package except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

You should also have received a copy of the License with the VA software package.

@section vabase_getting_started_sec Getting started

If you are a C++ developer and want to investigate the interface and integrate VA into your application, the first contact point should be the VABase interface represented by the #IVAInterface class.
If you are planning to create a binding for any other programming and/or scripting language, also have a look at VANet and the #IVANetClient class.
If you want to host a VA server or want to extend the core of VA by new rendering and reproduction modules, see #VACore.

*/

#ifndef IW_VA
#define IW_VA

#include "VAAudioSignalSource.h"
#include "VABase.h"
#include "VAEvent.h"
#include "VAEventHandlerGlobalLock.h"
#include "VAException.h"
#include "VAInterface.h"
#include "VAObject.h"
#include "VAObjectRegistry.h"
#include "VASamples.h"
#include "VAStruct.h"
#include "VAVersion.h"

#endif // IW_VA
