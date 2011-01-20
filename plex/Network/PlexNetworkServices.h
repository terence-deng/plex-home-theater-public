/*
 *  Copyright (C) 2010 Plex, Inc.   
 *
 *  Created on: Dec 18, 2010
 *      Author: Jamie Kirkpatrick
 */

#pragma once

#include "Network/NetworkServiceBrowser.h"

class PlexServiceListener;
typedef boost::shared_ptr < PlexServiceListener > PlexServiceListenerPtr;

///
/// Plex specific service browser.
///
class PlexNetworkServiceBrowser : public NetworkServiceBrowser
{
public:
  PlexNetworkServiceBrowser(io_service& ioService, unsigned short port, int refreshTime=NS_BROWSE_REFRESH_INTERVAL)
    : NetworkServiceBrowser(ioService, port, refreshTime)
  {
  }

  /// Notify of a new service.
  virtual void handleServiceArrival(NetworkServicePtr& service) 
  {
    map<string, string> params = service->getParams();
    for (map<string, string>::iterator param = params.begin(); param != params.end(); ++param)
    {
      string name = param->first;
      string value = param->second;
      CLog::CLog().Log(LOGINFO, "%s -> %s", param->first.c_str(), param->second.c_str());
    }
    
    dprintf("NetworkServiceBrowser: SERVICE arrived: %s", service->address().to_string().c_str());
  }
  
  /// Notify of a service going away.
  virtual void handleServiceDeparture(NetworkServicePtr& service) 
  {
    dprintf("NetworkServiceBrowser: SERVICE departed after not being seen for %f seconds: %s", service->timeSinceLastSeen(), service->address().to_string().c_str());
  }
  
  /// Notify of a service update.
  virtual void handleServiceUpdate(NetworkServicePtr& service)
  {
    dprintf("NetworkServiceBrowser: SERVICE updated: %s", service->address().to_string().c_str());
  }
};

///
/// Network service manager
/// 
class PlexServiceListener 
{
public:
  static PlexServiceListenerPtr Create()
  {
    return PlexServiceListenerPtr( new PlexServiceListener );
  }
  
  ~PlexServiceListener()
  {
    stop();
  }

  void start()
  {
    dprintf("NetworkService: Initializing.");
    
    // We start watching for changes in here.
    NetworkInterface::WatchForChanges();
       
    // Start the I/O service in its own thread.
    boost::thread t(boost::bind(&boost::asio::io_service::run, &m_ioService));
    t.detach();
  }

  void stop()
  {
    m_ioService.stop();
  }

private:
  PlexServiceListener()
  : m_pmsBrowser(m_ioService, NS_PLEX_MEDIA_SERVER_PORT) 
  {
    start();
  }

  boost::asio::io_service     m_ioService;
  PlexNetworkServiceBrowser   m_pmsBrowser;
};

