# Unreal Virtual Acoustics Plugin

This Plugin allows the user to use the [VA Server](http://www.virtualacoustics.org/) from the [Institute of Acoustics](https://www.akustik.rwth-aachen.de/cms/~dwma/Technische-Akustik/lidx/1/) (ITA) of the RWTH Aachen to playback sounds in Unreal.

# Installation 
To install the Plugin, clone the repository into the "Plugins" folder of your Unreal Engine 4 Project. Moreover make sure to have the VA Server prepared. 

The Server can either be used in its original form ([Link](http://www.virtualacoustics.org/)) or with the especially for this Plugin optimized Python Connection script ([Link](https://devhub.vr.rwth-aachen.de/VR-Group/vaserver), Recommended). 
Using the Version with the Python script has the adventage of not needing to Restart the Server all the time manually and make sure to follow the instructions which are currently in its Readme file.
If you are using the automatic VAServer launching either have VAServer cloned out next to the folder of your project or specify the VALauncher Path in the Engine/Virtual Acoustics(VA) section of the project settings. 

The Plugin requires to have the [nDisplayExtensions](https://devhub.vr.rwth-aachen.de/VR-Group/unreal-development/ndisplayextensions) installed.



## Usage 
For a more detailed C++ / Blueprint usage and a Documentation of the Plugins public functions, please check out the matching wiki page for each public Class and Enum: 
* [VAReceiverActor](https://devhub.vr.rwth-aachen.de/VR-Group/unreal-development/unreal-va-plugin/-/wikis/Documentation/VAReceiverActor) 
  * Actor handling the Connection and Scene Settings for the current world, as well as the Position updates for the Receiver. If there is no Receiver Actor placed in the Scene, there will be created a new one with Default values at Runtime. 
* [VAReflectionWall](https://devhub.vr.rwth-aachen.de/VR-Group/unreal-development/unreal-va-plugin/-/wikis/Documentation/VAReflectionWall)
  * Actor representing a wall on which the Sound Sources should reflect on. Make sure that its shape alligns with the wall. Please make sure to use symmetrical Directivities for the Sound Sources due to the fact that the directivities cannot be mirrored among the wall.
* [VASoundSourceComponent](https://devhub.vr.rwth-aachen.de/VR-Group/unreal-development/unreal-va-plugin/-/wikis/Documentation/VASoundSourceComponent)
  * Actor Component representing a Sound Source. Has to be attatched to something. It can have a graphical representation and reflecitons, which can be created with the Reflection Walls
* [VAEnums](https://devhub.vr.rwth-aachen.de/VR-Group/unreal-development/unreal-va-plugin/-/wikis/Documentation/VAEnmus)
  * All Enums used within the Plugin. At current state, only the EPlayAction is used for the Interface functions