/*
 *  Copyright (C) 2010 Plex, Inc.   
 *
 *  Created on: Dec 16, 2010
 *      Author: Elan Feingold
 */

#pragma once

#include <boost/asio.hpp>
#include "Plex/PlexUtils.h"

using namespace boost::asio;

#define NS_BROWSE_REFRESH_INTERVAL  5000
#define NS_REMOVAL_INTERVAL         2000
#define NS_DEAD_SERVER_TIME        10000

#define NS_MAX_PACKET_SIZE 2048
#define NS_BROADCAST_ADDR  ip::address::from_string("239.255.255.250")
#define NS_SEARCH_MSG      "M-SEARCH * HTTP/1.1\r\n"

#define NS_PLEX_MEDIA_SERVER_PORT 32410
#define NS_PLEX_MEDIA_CLIENT_PORT 32412

/////////////////////////////////////////////////////////////////////////////
class WallclockTimer
{
 public:
  
  WallclockTimer() { gettimeofday(&m_start, 0); }
  void restart() { gettimeofday(&m_start, 0); }
  
  double elapsed()
  {
    struct timeval end;
    gettimeofday(&end, 0);
    
    long seconds  = end.tv_sec  - m_start.tv_sec;
    long useconds = end.tv_usec - m_start.tv_usec;

    return (seconds + (double)useconds/1000000.0);
  }
  
 private:
  
  struct timeval m_start;
};

/////////////////////////////////////////////////////////////////////////////
class NetworkService
{
 public:
  
  NetworkService(const ip::address& address, const map<string, string>& params)
    : m_address(address)
    , m_parameters(params) 
  {}
  
  bool        hasParam(const string& name) { return m_parameters.find(name) != m_parameters.end(); }
  string      getParam(const string& name) { return m_parameters[name]; }
  map<string, string> getParams() { return m_parameters; }
  
  ip::address address() { return m_address; }    
  void        freshen(map<string, string>& params) { m_parameters = params; m_timeSinceLastSeen.restart(); }
  double      timeSinceLastSeen() { return m_timeSinceLastSeen.elapsed(); }
  double      timeSinceCreation() { return m_timeSinceCreation.elapsed(); }
  string      getUrl() { return "http://" + m_address.to_string() + ":" + getParam("Port"); }
  
 private:
  
  WallclockTimer      m_timeSinceLastSeen;
  WallclockTimer      m_timeSinceCreation;
  ip::address         m_address;
  map<string, string> m_parameters;
};

typedef boost::shared_ptr<NetworkService> NetworkServicePtr;
typedef pair<ip::address, NetworkServicePtr> address_service_pair;
