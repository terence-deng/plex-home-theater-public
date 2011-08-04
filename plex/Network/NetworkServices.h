/*
 *  Copyright (C) 2010 Plex, Inc.   
 *
 *  Created on: Dec 18, 2010
 *      Author: Elan Feingold
 */

#pragma once

#include "NetworkServiceAdvertiserPMS.h"
#include "NetworkServiceBrowserPlayer.h"

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
    dprintf("NetworkServices: Initializing...");

#if defined(_WIN32) || defined(__linux__)
    // We start watching for changes in here.
    NetworkInterface::WatchForChanges();
#endif
    
    // Create the advertiser and start it.
    m_pmsAdvertiser = NetworkServiceAdvertiserPtr(new NetworkServiceAdvertiserPMS(m_ioService, NS_BROADCAST_ADDR, NS_PLEX_MEDIA_SERVER_PORT));
    m_pmsAdvertiser->start();
    
    // Create the old advertiser, which is needed for backwards compatiblity.
    m_pmsOldAdvertiser = NetworkServiceAdvertiserPtr(new NetworkServiceAdvertiserPMS(m_ioService, NS_BROADCAST_ADDR_OLD, NS_PLEX_MEDIA_SERVER_PORT_OLD));
    m_pmsOldAdvertiser->start();
    
    // Create the player browser.
    m_playerBrowser = NetworkServiceBrowserPtr(new NetworkServiceBrowserPlayer(m_ioService));
    
    // Create the server browser.
    //m_serverBrowser = NetworkServiceBrowserPtr(new NetworkServiceBrowserServer(m_ioService));
    
    // Start the I/O service in its own thread.
    boost::thread t(boost::bind(&boost::asio::io_service::run, &m_ioService));
    t.detach();
  }
  
  void broadcastUpdate(const string& parameter="")
  {
    if (m_pmsAdvertiser)
      m_pmsAdvertiser->update(parameter);
  }
  
  void stop()
  {
    m_pmsAdvertiser->stop();
    m_ioService.stop();
  }
 
  NetworkServiceBrowserPtr getPlayerBrowser() { return m_playerBrowser; }
  
 private:
 
  NetworkServices()
  {
    dprintf("Creating NetworkServices singleton.");
  }
  
  ~NetworkServices()
  {
  }
  
  boost::asio::io_service     m_ioService;
  NetworkServiceAdvertiserPtr m_pmsOldAdvertiser;
  NetworkServiceAdvertiserPtr m_pmsAdvertiser;
  NetworkServiceBrowserPtr    m_playerBrowser;
  NetworkServiceBrowserPtr    m_serverBrowser;
};

