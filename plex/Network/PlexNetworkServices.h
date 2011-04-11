/*
 *  Copyright (C) 2010 Plex, Inc.   
 *
 *  Created on: Dec 18, 2010
 *      Author: Jamie Kirkpatrick
 */

#pragma once

#define BOOST_ASIO_DISABLE_IOCP 1; // IOCP reactor reads failed using boost 1.44.

#include "Plex/PlexUtils.h"
#include "Network/NetworkServiceBrowser.h"
#include "PlexSourceScanner.h"
#include "PlexNetworkServiceAdvertiser.h"

class PlexServiceListener;
typedef boost::shared_ptr < PlexServiceListener > PlexServiceListenerPtr;
typedef boost::shared_ptr < boost::thread > ThreadPtr;

///
/// Plex specific service browser.
///
class PlexNetworkServiceBrowser : public NetworkServiceBrowser
{
public:
  PlexNetworkServiceBrowser(boost::asio::io_service& ioService, unsigned short port, int refreshTime=NS_BROWSE_REFRESH_INTERVAL)
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
    
    // Scan the host.
    dprintf("NetworkServiceBrowser: SERVICE arrived: %s", service->address().to_string().c_str());
    CPlexSourceScanner::ScanHost(service->address().to_string(), service->getParam("Name"), service->getUrl());
  }
  
  /// Notify of a service going away.
  virtual void handleServiceDeparture(NetworkServicePtr& service) 
  {
    dprintf("NetworkServiceBrowser: SERVICE departed after not being seen for %f seconds: %s", service->timeSinceLastSeen(), service->address().to_string().c_str());
    CPlexSourceScanner::RemoveHost(service->address().to_string());
  }
  
  /// Notify of a service update.
  virtual void handleServiceUpdate(NetworkServicePtr& service)
  {
    // Scan the host.
    dprintf("NetworkServiceBrowser: SERVICE updated: %s", service->address().to_string().c_str());
    CPlexSourceScanner::ScanHost(service->address().to_string(), service->getParam("Name"), service->getUrl());
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
    m_ptrThread = ThreadPtr(new boost::thread(boost::bind(&boost::asio::io_service::run, &m_ioService)));
  }

  void stop()
  {
    m_ioService.stop();
    if (m_ptrThread)
	  {
		  m_ptrThread->join();
      m_ptrThread.reset();
	  }
  }

private:
  PlexServiceListener()
  : m_pmsBrowser(m_ioService, NS_PLEX_MEDIA_SERVER_PORT) 
  , m_plexAdvertiser(m_ioService)
  {
    start();
  }

  boost::asio::io_service      m_ioService;
  PlexNetworkServiceBrowser    m_pmsBrowser;
  PlexNetworkServiceAdvertiser m_plexAdvertiser;
  ThreadPtr                    m_ptrThread;
};

