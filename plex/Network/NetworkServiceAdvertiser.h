/*
 *  Copyright (C) 2010 Plex, Inc.   
 *
 *  Created on: Dec 16, 2010
 *      Author: Elan Feingold
 */

#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>

#include "NetworkService.h"
#include "NetworkServiceBase.h"

#define CRLF "\r\n"

/////////////////////////////////////////////////////////////////////////////
class NetworkServiceAdvertiser : public NetworkServiceBase
{
 public:
  
  /// Constructor.
  NetworkServiceAdvertiser(boost::asio::io_service& ioService, unsigned short port)
   : NetworkServiceBase(ioService)
   , m_port(port)
  {
    // This is where we'll send notifications.
    m_notifyEndpoint = boost::asio::ip::udp::endpoint(NS_BROADCAST_ADDR, m_port+1);
  }
 
  /// Start advertising the service.
  void start()
  {
  }
  
  /// Stop advertising the service.
  void stop()
  {
    // Send out the BYE message synchronously and close the socket.
    if (m_socket)
    {
      string hello = "BYE * HTTP/1.0\r\n" + createReplyMessage();
      try { m_socket->send_to(boost::asio::buffer(hello), m_notifyEndpoint); } catch (...) {}
      m_socket->close();
    }
  }
  
  /// Advertise an update to the service.
  void update()
  {
    broadcastMessage(m_socket, "UPDATE");
  }
  
  /// For subclasses to fill in.
  virtual void createReply(map<string, string>& headers) {}
  
  /// For subclasses to fill in.
  virtual string getType() = 0;
  virtual string getResourceIdentifier() = 0;
  virtual string getBody() = 0;
  
 private:
  
  /// Handle network change.
  virtual void handleNetworkChange(const vector<NetworkInterface>& interfaces)
  {
    dprintf("Network change for advertiser.");

    // Close the old one.
    if (m_socket)
    {
      // Try to shutdown socket.
      try { m_socket->cancel(); } catch (...) {}
      try { m_socket->close();  } catch (...) {}
    }
   
    // Start up new socket.
    m_socket = udp_socket_ptr(new boost::asio::ip::udp::socket(m_ioService));

    // Listen, send out HELLO, and start reading data.
    setupMulticastListener(m_socket, "0.0.0.0", m_port);
    broadcastMessage(m_socket, "HELLO");

    m_socket->async_receive_from(boost::asio::buffer(m_data, NS_MAX_PACKET_SIZE), m_endpoint, boost::bind(&NetworkServiceAdvertiser::handleRead, this, m_socket, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
  }

  void broadcastMessage(const udp_socket_ptr& socket, const string& action)
  {
    // Send out the message.
    if (socket)
    {
      string hello = action + " * HTTP/1.0\r\n" + createReplyMessage();
      try { socket->send_to(boost::asio::buffer(hello), m_notifyEndpoint); } catch (...) {}
    }
  }
  
  void handleRead(const udp_socket_ptr& socket, const boost::system::error_code& error, size_t bytes)
  {
    if (!error)
    {
      //dprintf("Network Service: Received a advertisement request from %s", m_endpoint.address().to_string().c_str());

      // Create the reply, we don't actually look at the request for now.
      string reply = "HTTP/1.0 200 OK\r\n" + createReplyMessage();
      
      // Write the reply back to the client and wait for the next packet.
      try { socket->send_to(boost::asio::buffer(reply), m_endpoint); } 
      catch (...) { eprintf("Error replying to broadcast packet."); }
      
      socket->async_receive_from(boost::asio::buffer(m_data, NS_MAX_PACKET_SIZE), m_endpoint, boost::bind(&NetworkServiceAdvertiser::handleRead, this, socket, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      eprintf("Network Service: Error in advertiser handle read.");
    }
  }
  
  // Turn the parameter map into HTTP headers.
  string createReplyMessage()
  {
    string reply;

    map<string, string> params;
    createReply(params);

    reply = "Content-Type: " + getType() + CRLF;
    reply += "Resource-Identifier: " + getResourceIdentifier() + CRLF;
    BOOST_FOREACH(string_pair param, params)
      reply += param.first + ": " + param.second + CRLF;

    // See if there's a body.
    string body = getBody();
    if (body.empty() == false)
    {
      reply += "Content-Length: " + boost::lexical_cast<string>(body.size()) + CRLF;
    }
    
    reply += CRLF;
    reply += body;
    
    return reply;
  }
  
  udp_socket_ptr    m_socket;
  unsigned short    m_port;
  boost::asio::ip::udp::endpoint m_endpoint;
  boost::asio::ip::udp::endpoint m_notifyEndpoint;
  char              m_data[NS_MAX_PACKET_SIZE];
};
