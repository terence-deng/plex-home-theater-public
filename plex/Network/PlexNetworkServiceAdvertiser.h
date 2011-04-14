/*
 *  Copyright (C) 2011 Plex, Inc.
 *
 *  Created on: Apr 10, 2011
 *      Author: Elan Feingold
 */

#pragma once

#include "GUISettings.h"
#include "NetworkServiceAdvertiser.h"

/////////////////////////////////////////////////////////////////////////////
class PlexNetworkServiceAdvertiser : public NetworkServiceAdvertiser
{
 public:
  
  /// Constructor.
  PlexNetworkServiceAdvertiser(boost::asio::io_service& ioService)
    : NetworkServiceAdvertiser(ioService, NS_PLEX_MEDIA_CLIENT_PORT) {}
  
 protected:
  
  /// For subclasses to fill in.
  virtual void createReply(map<string, string>& headers) 
  {
    //headers["Name"] = GetMachineName();
    //headers["Port"] = "32400";
    //headers["Version"] = Cocoa_GetAppVersion();
    //headers["Updated-At"] = lexical_cast<string>(getUpdatedAt());
  }
  
  /// For subclasses to fill in.
  virtual string getType()
  {
    return "plex/media-player";
  }
  
  virtual string getResourceIdentifier() { return g_guiSettings.GetString("system.uuid");  }
  virtual string getBody() { return ""; }
};
