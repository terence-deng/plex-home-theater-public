/*
 *  Copyright (C) 2010 Plex, Inc.   
 *
 *  Created on: Dec 18, 2010
 *      Author: Elan Feingold
 */

#pragma once

#include "Database.h"
#include "LibraryUpdater.h"
#include "NetworkServiceAdvertiser.h"
#include "Preferences.h"
#include "Serializable.h"

#ifdef _WIN32
string Cocoa_GetAppVersion();
#else
const char* Cocoa_GetAppVersion();
#endif

/////////////////////////////////////////////////////////////////////////////
class PlexMediaServer : public Serializable
{
  DEFINE_CLASS_NAME(PlexMediaServer);
  
  void serialize(ostream& out)
  {
    SERIALIZE_ELEMENT_START(out, className());
    {
      SERIALIZE_ELEMENT_START(out, "Library");
      
      // Library sections.
      Database db;
      vector<LibrarySectionPtr> sections = LibrarySection::FindAll(db);
      BOOST_FOREACH(LibrarySectionPtr section, sections)
      {
        section->setAttribute("refreshing", LibraryUpdater::GetUpdater().isUpdatingSection(section->id) ? "1" : "0");
        section->serialize(out, false);
      }
      
      SERIALIZE_ELEMENT_END(out, "Library");
    }

    SERIALIZE_FULL_CLASS_END(out);
  }
};

/////////////////////////////////////////////////////////////////////////////
class NetworkServiceAdvertiserPMS : public NetworkServiceAdvertiser
{
 public:
  
  /// Constructor.
  NetworkServiceAdvertiserPMS(io_service& ioService)
    : NetworkServiceAdvertiser(ioService, NS_PLEX_MEDIA_SERVER_PORT) {}
  
 protected:
  
  /// For subclasses to fill in.
  virtual void createReply(map<string, string>& headers) 
  {
    string friendlyName = Preferences::Get()->getStringValue(PREF_FRIENDLY_NAME);
    if (friendlyName.size() == 0)
    {
      char hostname[256];
      gethostname(hostname, 256);
      friendlyName = hostname;
    }
    
    headers["Name"] = friendlyName;
    headers["Port"] = "32400";
    headers["Version"] = Cocoa_GetAppVersion();
    headers["Updated-At"] = lexical_cast<string>(getUpdatedAt());
  }
  
  /// For subclasses to fill in.
  virtual string getType()
  {
    return "plex/media-server";
  }
  
  /// For subclasses to fill in.
  virtual string getResourceIdentifier()
  {
    return BonjourRequestHandler::Get()->GetMachineIdentifier();
  }
  
  /// For subclasses to fill in.
  virtual string getBody()
  {
    PlexMediaServer pms;
    return pms.toReply().content;
  }
};
