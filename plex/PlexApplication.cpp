/*
 *  PlexApplication.cpp
 *  XBMC
 *
 *  Created by Jamie Kirkpatrick on 20/01/2011.
 *  Copyright 2011 Plex Inc. All rights reserved.
 *
 */

#include "PlexApplication.h"
#include "PlexNetworkServices.h"

////////////////////////////////////////////////////////////////////////////////
PlexApplicationPtr PlexApplication::Create()
{
  return PlexApplicationPtr( new PlexApplication() );
}

////////////////////////////////////////////////////////////////////////////////
PlexApplication::PlexApplication()
{
  m_serviceListener = PlexServiceListener::Create();
}
