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
    static boost::mutex g_mutex;
    boost::mutex::scoped_lock lk(g_mutex);
    
    static NetworkServices* services = 0;
    if (services == 0)
      services = new NetworkServices();
    
    return *services;
  }
  
  void start()
  {
    dprintf("NetworkServices: Initializing.");

#ifdef _WIN32
    // We start watching for changes in here.
    NetworkInterface::WatchForChanges();
#endif
    
    // Create the advertiser and start it.
    m_pmsAdvertiser = new NetworkServiceAdvertiserPMS(m_ioService);
    m_pmsAdvertiser->start();
    
    // Start the I/O service in its own thread.
    boost::thread t(boost::bind(&boost::asio::io_service::run, &m_ioService));
    t.detach();
  }
  
  void broadcastUpdate()
  {
    if (m_pmsAdvertiser)
      m_pmsAdvertiser->update();
  }
  
  void stop()
  {
    m_pmsAdvertiser->stop();
    m_ioService.stop();
  }
  
 private:
 
  NetworkServices()
   : m_pmsAdvertiser(0)
   //, m_pmsBrowser(m_ioService, NS_PLEX_MEDIA_SERVER_PORT) {}
  {
    dprintf("Creating NetworkServices singleton.");
  }
  
  ~NetworkServices()
  {
    delete m_pmsAdvertiser;
    m_pmsAdvertiser = 0;
  }
  
  boost::asio::io_service      m_ioService;
  NetworkServiceAdvertiserPMS* m_pmsAdvertiser;
  //NetworkServiceBrowser      m_pmsBrowser;
};

