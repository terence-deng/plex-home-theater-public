#include "log.h"
#include "PlexMediaServerQueue.h"
#include "FileSystem/FileCurl.h"

using namespace XFILE;

PlexMediaServerQueue PlexMediaServerQueue::g_plexMediaServerQueue;

/////////////////////////////////////////////////////////////////////////////
PlexMediaServerQueue::PlexMediaServerQueue()
  : m_allowScrobble(true)
{
  Create();
}

/////////////////////////////////////////////////////////////////////////////
void PlexMediaServerQueue::Process()
{
  while (m_bStop == false)
  {
    // Wait for be signalled.
    m_mutex.lock();
    m_condition.wait(m_mutex);
    
    // OK, now process our queue.
    while (m_queue.size() > 0)
    {
      // Get a URL.
      string url = m_queue.front();
      
      // Hit the Plex Media Server.
      CFileCurl http;
      CStdString reply;
      http.Get(url, reply);
      CLog::Log(LOGNOTICE, "Plex Media Server Queue: %s", url.c_str());
      
      // That's it, pop it off the queue.
      m_queue.pop();
    }
    
    m_mutex.unlock();
  }
  
  printf("Exiting Plex Media Server queue...\n");
}

/////////////////////////////////////////////////////////////////////////////
void PlexMediaServerQueue::StopThread()
{
  if (m_bStop)
    return;
  
  // Signal the condition.
  m_mutex.lock();
  m_bStop = true;
  m_condition.notify_one();
  m_mutex.unlock();
  
  CThread::StopThread();
}
