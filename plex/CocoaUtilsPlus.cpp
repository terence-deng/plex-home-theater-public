//
//  CocoaUtilsPlus.cpp
//  Plex
//
//  Created by Max Feingold on 10/21/2011.
//  Copyright 2011 Plex Inc. All rights reserved.
//

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#include <boost/lexical_cast.hpp>
#include "CocoaUtilsPlus.h"

///////////////////////////////////////////////////////////////////////////////
string Cocoa_GetMachinePlatform()
{
  string platform;
  
#if defined(_WIN32)
  platform = "Windows";
#elif defined(__linux__)
  platform = "Linux";
#else
  platform = "MacOSX";
#endif
  
  return platform;
}

/////////////////////////////////////////////////////////////////////////////
string Cocoa_GetMachinePlatformVersion()
{
  string ver;
  
#if defined(_WIN32)
  
  DWORD dwVersion = GetVersion();
  DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
  DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
  DWORD dwBuildNumber  = (DWORD)(HIWORD(dwVersion));
  
  char str[256];
  sprintf(str, "%d.%d (Build %d)", dwMajorVersion, dwMinorVersion, dwBuildNumber);
  ver = str;
  
#elif defined(__linux__)
  
  struct utsname buf;
  if (uname(&buf) == 0)
  {
    ver = buf.release;
    ver = " (" + string(buf.version) + ")";
  }
  
#else
  
  SInt32 res = 0; 
  Gestalt(gestaltSystemVersionMajor, &res);
  ver = boost::lexical_cast<string>(res) + ".";
  
  Gestalt(gestaltSystemVersionMinor, &res);
  ver += boost::lexical_cast<string>(res) + ".";
  
  Gestalt(gestaltSystemVersionBugFix, &res);
  ver += boost::lexical_cast<string>(res);
  
#endif
  
  return ver;
}
