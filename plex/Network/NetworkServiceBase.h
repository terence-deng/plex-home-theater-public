#pragma once

#include <boost/asio.hpp>

#include "Network/NetworkInterface.h"
#include "Network/NetworkService.h"

typedef boost::shared_ptr<ip::udp::socket> udp_socket_ptr;
typedef pair<string, udp_socket_ptr> address_socket_pair;

class NetworkServiceBase
{
 protected:

  /// Constructor.
  NetworkServiceBase(boost::asio::io_service& ioService)
    : m_ioService(ioService)
    , m_timer(ioService, posix_time::seconds(2))
    , m_firstChange(true)
  {
    // Register for network changes.
    NetworkInterface::RegisterObserver(boost::bind(&NetworkServiceBase::onNetworkChanged, this, _1));
  }

  /// Utility to set up a multicast listener/broadcaster for a single interface.
  void setupMulticastListener(const udp_socket_ptr& socket, const string& bindAddress, unsigned short port, bool outboundInterface = false)
  {
    // Create the server socket.
    ip::udp::endpoint listenEndpoint(ip::address::from_string(bindAddress), port);
    socket->open(listenEndpoint.protocol());
    
    // Bind.
    try { socket->bind(listenEndpoint); }
    catch (std::exception& ex) { eprintf("NetworkService: Couldn't bind to port %d: %s", port, ex.what()); }
    
    // Reuse.
    try { socket->set_option(ip::udp::socket::reuse_address(true)); }
    catch (std::exception& ex) { eprintf("NetworkService: Couldn't reuse address: %s", ex.what()); }
    
    // Enable loopback.
    socket->set_option(ip::multicast::enable_loopback(true));
    
    // Join the multicast group after leaving it (just in case).
    try { socket->set_option(ip::multicast::leave_group(NS_BROADCAST_ADDR)); } 
    catch (std::exception& ex) { }
    try { socket->set_option(ip::multicast::join_group(NS_BROADCAST_ADDR)); }
    catch (std::exception& ex) { eprintf("NetworkService: Couldn't join multicast group: %s", ex.what()); }
    
    if (outboundInterface)
    {
      // Send out multicast packets on the specified interface.
      ip::address_v4 localInterface = ip::address_v4::from_string(bindAddress);
      ip::multicast::outbound_interface option(localInterface);
      try { socket->set_option(option); }
      catch (std::exception& ex) { eprintf("NetworkService: Unable to set option on socket."); }
    }
  }

  /// For subclasses to fill in.
  virtual void handleNetworkChange(const vector<NetworkInterface>& interfaces) = 0;

  boost::asio::io_service& m_ioService;
  deadline_timer m_timer;
  bool m_firstChange;

 private:

  void onNetworkChanged(const vector<NetworkInterface>& interfaces)
  {
    if (m_firstChange)
    {
      // Dispatch the notification in an ASIO thread's context.
      dprintf("NetworkService: Quick dispatch of network change.");
      m_ioService.dispatch(boost::bind(&NetworkServiceBase::handleNetworkChange, this, interfaces));
      m_firstChange = false;
    }
    else
    {
      // Dispatch the change with a two second delay.
      dprintf("NetworkService: Dispatch network change after two second delay.");
      m_timer.expires_from_now(posix_time::seconds(2));
      m_timer.async_wait(boost::bind(&NetworkServiceBase::handleNetworkChange, this, interfaces));
    }
  }
};
