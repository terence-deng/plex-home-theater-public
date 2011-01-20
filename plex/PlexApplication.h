/*
 *  PlexApplication.h
 *  XBMC
 *
 *  Created by Jamie Kirkpatrick on 20/01/2011.
 *  Copyright 2011 Plex Inc. All rights reserved.
 *
 */

#include <boost/shared_ptr.hpp>

class PlexApplication;
typedef boost::shared_ptr<PlexApplication> PlexApplicationPtr;
class PlexServiceListener;
typedef boost::shared_ptr<PlexServiceListener> PlexServiceListenerPtr;

///
/// The hub of all Plex goodness.
///
class PlexApplication
{
public:
  static PlexApplicationPtr Create();
    
private:
  /// Default constructor.
  PlexApplication();
  
  /// Members
  PlexServiceListenerPtr m_serviceListener;
};
