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

@page VANet

@section vanet_sec VANet

VANet provides a transparent network communication of calls to the VA interface. It is useful to control a VA server remotely and is the standard way to communicate with VA for inter-process methods and bindings for other scripting languages.

@section vanet_license_sec License

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

@section vanet_getting_started_sec Getting started

If you are a C++ developer and want to integrate VANet into your application, the first contact point should be the #IVANetClient.
If you want to provide a VA server connection, have a look at #IVANetServer.

*/

#ifndef IW_VANET
#define IW_VANET

#include "VANetDefinitions.h"
#include "VANetClient.h"
#include "VANetServer.h"
#include "VANetUtils.h"
#include "VANetVersion.h"

#ifdef VANET_WITH_NETAUDIO_STREAM_SERVER
#include "VANetAudioStreamServer.h"
#endif

#endif // IW_VANET
