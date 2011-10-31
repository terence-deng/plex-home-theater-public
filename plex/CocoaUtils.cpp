//
//  CocoaUtils.cpp
//  Plex
//
//  Created by Max Feingold on 10/21/2011.
//  Copyright 2011 Plex Inc. All rights reserved.
//

#include "CocoaUtils.h"
#include "version.h"

#ifdef _WIN32

const char* Cocoa_GetAppVersion()
{
  return APPLICATION_VERSION;
}

#endif
