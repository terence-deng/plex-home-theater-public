/*
 *  Copyright (C) 2010 Plex, Inc.   
 *
 *  Created on: Dec 18, 2010
 *      Author: Elan Feingold
 */

#pragma once

#include "NetworkServiceAdvertiserPMS.h"
#include "NetworkServiceBrowser.h"

class NetworkServices
{
 public:

  static NetworkServices& Get()
  {
    static NetworkServices* services = 0;
    if (services == 0)
      services = new NetworkServices();
    
    return *services;
  }
  
  void start()
  {
    dprintf("NetworkService: Initializing.");
    
#ifdef _WIN32
    // We start watching for changes in here.
    NetworkInterface::WatchForChanges();
#endif

    // Advertise PMS.
    m_pmsAdvertiser.start();
    
    // Start the I/O service in its own thread.
    boost::thread t(boost::bind(&boost::asio::io_service::run, &m_ioService));
    t.detach();
  }
  
  void stop()
  {
    m_pmsAdvertiser.stop();
    m_ioService.stop();
  }
  
 private:
 
  NetworkServices()
   : m_pmsAdvertiser(m_ioService)
   , m_pmsBrowser(m_ioService, NS_PLEX_MEDIA_SERVER_PORT) {}
  
  boost::asio::io_service     m_ioService;
  NetworkServiceAdvertiserPMS m_pmsAdvertiser;
  NetworkServiceBrowser       m_pmsBrowser;
};

