/*
 *  Copyright (C) 2011 Plex, Inc.   
 *      Author: Elan Feingold
 */

#pragma once

#include <map>
#include <string>

#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include "FileItem.h"
#include "GUIUserMessages.h"
#include "GUIWindowManager.h"
#include "PlexDirectory.h"

using namespace std;
using namespace boost;

class PlexContentWorker;
typedef boost::shared_ptr<PlexContentWorker> PlexContentWorkerPtr;

/////////////////////////////////////////////////////////////////////////////////////////
class PlexContentWorkerManager
{
 public:
  
  /// Constructor.
  PlexContentWorkerManager()
    : m_workerID(0) {}
  
  /// Number of pending workers.
  int pendingWorkers()
  {
    return m_pendingWorkers.size();
  }

  /// Queue a new worker.
  PlexContentWorkerPtr enqueue(int targetWindow, const string& url, int contextID);
  
  /// Find by ID.
  PlexContentWorkerPtr find(int id)
  {
    recursive_mutex::scoped_lock lk(m_mutex);

    if (m_pendingWorkers.find(id) != m_pendingWorkers.end())
      return m_pendingWorkers[id];

    return PlexContentWorkerPtr();
  }

  /// Destroy by ID.
  void destroy(int id)
  {
    recursive_mutex::scoped_lock lk(m_mutex);

    if (m_pendingWorkers.find(id) != m_pendingWorkers.end())
      m_pendingWorkers.erase(id);
  }

  /// Cancel all pending workers.
  void cancelPending();

 private:
  
  /// Keeps track of the last worker ID.
  int m_workerID;

  /// Keeps track of pending workers.
  map<int, PlexContentWorkerPtr> m_pendingWorkers;
  
  /// Protects the map.
  recursive_mutex m_mutex;
};

/////////////////////////////////////////////////////////////////////////////////////////
class PlexContentWorker
{
  friend class PlexContentWorkerManager;
  
 public:

  void run()
  {
    printf("Processing content request in thread [%s]\n", m_url.c_str());

    if (m_cancelled == false)
    {
      // Get the results.
      CPlexDirectory dir;
      dir.GetDirectory(m_url, *m_results.get());
    }

    // If we haven't been canceled, send them back.
    if (m_cancelled == false)
    {
      // Notify the main menu.
      CGUIMessage msg(GUI_MSG_SEARCH_HELPER_COMPLETE, m_targetWindow, 0, m_id, m_contextID);
      g_windowManager.SendThreadMessage(msg);
    }
    else
    {
      // Get rid of myself.
      m_manager->destroy(m_id);
    }
  }

  void cancel() { m_cancelled = true; }
  CFileItemListPtr getResults() { return m_results; }
  int getID() { return m_id; }

 protected:

  PlexContentWorker(PlexContentWorkerManager* manager, int id, int targetWindow, const string& url, int contextID)
    : m_manager(manager)
    , m_id(id)
    , m_targetWindow(targetWindow)
    , m_url(url)
    , m_cancelled(false)
    , m_contextID(contextID)
    , m_results(new CFileItemList())
  {}

 private:

  PlexContentWorkerManager* m_manager;
  
  int              m_id;
  int              m_targetWindow;
  string           m_url;
  bool             m_cancelled;
  int              m_contextID;
  CFileItemListPtr m_results;
};
