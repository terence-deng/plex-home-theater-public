//
//  CocoaUtilsPlus.cpp
//  Plex
//
//  Created by Max Feingold on 10/21/2011.
//  Copyright 2011 Plex Inc. All rights reserved.
//

#include "CocoaUtilsPlus.h"

#ifdef _WIN32

// TODO - mfeingol - Implement these correctly

string Cocoa_GetMachinePlatform()
{
  return "Windows";
}

string Cocoa_GetMachinePlatformVersion()
{
  return "6.1";
}

#endif
