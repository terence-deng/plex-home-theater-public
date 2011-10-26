#pragma once

//
//  PlexServerManager.h
//
//  Created by Elan Feingold on 10/24/11.
//  Copyright (c) 2011 Blue Mandrill Design. All rights reserved.
//

#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

#include "CocoaUtilsPlus.h"

using namespace std;

////////////////////////////////////////////////////////////////////
class PlexServer
{
 public:

  /// Constructor.
  PlexServer(const string& uuid, const string& name, const string& addr, unsigned short port, const string& token)
    : uuid(uuid), name(name), address(addr), port(port), token(token)
  {
    if (token.empty())
      local = Cocoa_IsHostLocal(addr);
    
    m_key = uuid + "-" + address + "-" + boost::lexical_cast<string>(port);
  }
  
  /// Is it alive? Blocks, can take time.
  bool reachable()
  {
    return true;
  }
  
  /// Return the root URL.
  string url()
  {
    string ret = "http://" + address + ":" + boost::lexical_cast<string>(port);
    if (token.empty() == false)
      ret += "?X-Plex-Token=" + token;
    
    return ret;
  }
  
  int score()
  {
    return 0;
  }
  
  string key() const
  {
    return m_key;
  }
  
  /// Was it auto-detected?
  bool detected()
  {
    return token.empty();
  }
  
  bool live;
  bool local;
  string uuid;
  string name;
  string token;
  string address;
  unsigned short port;
  
 private:
  
  string m_key;
};

typedef boost::shared_ptr<PlexServer> PlexServerPtr;
typedef pair<string, PlexServerPtr> key_server_pair;

////////////////////////////////////////////////////////////////////
class PlexServerManager
{
public:
  
  /// Singleton.
  static PlexServerManager& Get()
  {
    static PlexServerManager* instance = 0;
    if (instance == 0)
    {
      instance = new PlexServerManager();
      boost::thread t(boost::bind(&PlexServerManager::run, instance));
      t.detach();
    }
    
    return *instance;
  }
  
  /// Get me the best server.
  PlexServerPtr findBestServer()
  {
    boost::recursive_mutex::scoped_lock lk(m_mutex);
    
    return PlexServerPtr();
  }
  
  /// Server appeared.
  void addServer(const string& uuid, const string& name, const string& addr, unsigned short port, const string& token="")
  {
    boost::recursive_mutex::scoped_lock lk(m_mutex);
    PlexServerPtr server = PlexServerPtr(new PlexServer(uuid, name, addr, port, token));
    m_servers[server->key()] = server;
    dump();
  }
  
  /// Server disappeared.
  void removeServer(const string& uuid, const string& name, const string& addr, unsigned short port)
  {
    boost::recursive_mutex::scoped_lock lk(m_mutex);
    PlexServerPtr server = PlexServerPtr(new PlexServer(uuid, name, addr, port, ""));
    m_servers.erase(server->key());
    dump();
  }
  
  /// Remove all non-detected servers.
  void setRemoteServers(vector<PlexServerPtr>& remoteServers)
  {
    boost::recursive_mutex::scoped_lock lk(m_mutex);
    
    // Set to whack.
    set<string> whack;
    BOOST_FOREACH(key_server_pair pair, m_servers)
      if (pair.second->detected() == false)
        whack.insert(pair.first);
    
    // Whack 'em.
    BOOST_FOREACH(string key, whack)
      m_servers.erase(key);
    
    // Add the new ones.
    BOOST_FOREACH(PlexServerPtr server, remoteServers)
      m_servers[server->key()] = server;
    
    dump();
  }
  
 private:
  
  void dump()
  {
    boost::recursive_mutex::scoped_lock lk(m_mutex);
    
    dprintf("SERVERS:");
    BOOST_FOREACH(key_server_pair pair, m_servers)
      dprintf("  * %s [%s:%d] local: %d (%s)", pair.second->name.c_str(), pair.second->address.c_str(), pair.second->port, pair.second->local, pair.second->uuid.c_str());
  }
  
  void run()
  {
    while (true)
    {
      // Run connectivity checks.
      
      // Sleep.
    }
  }
  
  map<string, PlexServerPtr> m_servers;
  boost::recursive_mutex     m_mutex;
};