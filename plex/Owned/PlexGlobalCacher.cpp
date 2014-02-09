

#include "Owned/PlexGlobalCacher.h"
#include "FileSystem/PlexDirectory.h"
#include "Client/PlexServerDataLoader.h"
#include "PlexApplication.h"
#include "utils/Stopwatch.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "guilib/GUITextBox.h"
#include "guilib/GUIEditControl.h"
#include "dialogs/GUIDialogYesNo.h"
#include "utils/log.h"
#include "filesystem/File.h"
#include "TextureCache.h"

using namespace XFILE;

#define GLOBAL_CACHING_DESC "Precaching Metadata"

CPlexGlobalCacher* CPlexGlobalCacher::m_globalCacher = NULL;

CPlexGlobalCacher::CPlexGlobalCacher() : CPlexThumbCacher() , CThread("Plex Global Cacher")
{
    m_continue = true;
}

CPlexGlobalCacher::~CPlexGlobalCacher()
{
}


CPlexGlobalCacher* CPlexGlobalCacher::getGlobalCacher()
{
    if (!m_globalCacher)   // Only allow one instance of class to be generated.
        m_globalCacher = new CPlexGlobalCacher();
    return m_globalCacher;
}

void CPlexGlobalCacher::Continue(bool cont)
{
    m_continue = cont;
}

void CPlexGlobalCacher::Start()
{
	CLog::Log(LOGNOTICE,"Global Cache : Creating cacher thread");
	CThread::Create(true);
	CLog::Log(LOGNOTICE,"Global Cache : Cacher thread created");
}


void controlGlobalCache()
{
    CPlexGlobalCacher* cacher = CPlexGlobalCacher::getGlobalCacher();


    //virtual void StopThread(bool bWait = true); 
   // bool IsRunning() const;        
   
    if ( ! cacher->IsRunning() )
    {
        bool ok = CGUIDialogYesNo::ShowAndGetInput("Start global caching?",
        "This will cache all metadata and thumbnails automatically.",
        "Larger libraries will take longer to precache.",
        "Reboot for best performance once caching is complete.",
        "No!", "Yes");

        if ( ok )
        {
            cacher->Start();
            cacher->Continue(true);
        }
    }
    else if ( cacher ->IsRunning() )
    {
        bool ok = CGUIDialogYesNo::ShowAndGetInput("Stop global caching?",
        "A reboot is recommended after stopping ",
        "the global caching process.",
        "Stopping may not occur right away.",
        "No!", "Yes");

        if ( ok )
        {
            CLog::Log(LOGNOTICE,"Global Cache : Cacher thread stopped by user");
            cacher -> StopThread(false);
            cacher -> Continue(false);
        }
    }
}



void CPlexGlobalCacher::Process()
{
  CPlexDirectory dir;
  CFileItemList list;
  CStopWatch timer;
  CStopWatch looptimer;
  CStopWatch msgtimer;
  CStdString Message;
  int totalsize =0;

  CFileItemListPtr pAllSharedSections = g_plexApplication.dataLoader->GetAllSharedSections();
  CLog::Log(LOGNOTICE,"Global Cache : found %d Shared Sections",pAllSharedSections->Size());

  // get all the sections names
  CFileItemListPtr pAllSections = g_plexApplication.dataLoader->GetAllSections();
  pAllSections->Append(*pAllSharedSections);

  CLog::Log(LOGNOTICE,"Global Cache : found %d Regular Sections",pAllSections->Size());

  timer.StartZero();
  msgtimer.StartZero();
  for (int iSection = 0; iSection < pAllSections->Size() && m_continue; iSection ++)
  {
    list.Clear();

    looptimer.StartZero();
    CFileItemPtr Section = pAllSections->Get(iSection);

    // Pop the notification
    Message.Format("Retrieving content from '%s'...", Section->GetLabel());
    CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, Message, GLOBAL_CACHING_DESC, 5000, false,500);

    // gets all the data from one section
    CURL url(Section->GetPath());
    PlexUtils::AppendPathToURL(url, "all");

    // store them into the list
    dir.GetDirectory(url, list);

    // Grab the server Name
    CStdString ServerName = "<unknown>";
    if (list.Size())
    {
      CPlexServerPtr  pServer = g_plexApplication.serverManager->FindFromItem(list.Get(0));
      if (pServer)
      {
        ServerName = pServer->GetName();
      }

      CLog::Log(LOGNOTICE,"Global Cache : Processed +%d items in '%s' on %s (total %d), took %f (total %f)",list.Size(),Section->GetLabel().c_str(),ServerName.c_str(), totalsize, looptimer.GetElapsedSeconds(),timer.GetElapsedSeconds());
    }

    totalsize += list.Size();

    // Here we have the file list, just process the items
    for (int i = 0; i < list.Size() && m_continue; i++)
    {
      CFileItemPtr item = list.Get(i);
      if (item->IsPlexMediaServer())
      {
          // Pop the notification window each sec only in order to avoid queuing too many messages
          if (msgtimer.GetElapsedSeconds() > 1)
          {
            CStdString Message,Caption;

            msgtimer.StartZero();
            Message.Format("(%d / %d) - Processing %0.0f%%..." ,iSection+1,pAllSections->Size(),float(i * 100.0f) / (float)list.Size());
            Caption.Format("Precaching '%s' on %s",Section->GetLabel(),ServerName);

            CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, Message, Caption, 2000, false,10);
          }

          // do the caching work
          if (item->HasArt("thumb") &&
              !CTextureCache::Get().HasCachedImage(item->GetArt("thumb")))
            CTextureCache::Get().CacheImage(item->GetArt("thumb"));

          if (item->HasArt("fanart") &&
              !CTextureCache::Get().HasCachedImage(item->GetArt("fanart")))
            CTextureCache::Get().CacheImage(item->GetArt("fanart"));


          if (item->HasArt("grandParentThumb") &&
              !CTextureCache::Get().HasCachedImage(item->GetArt("grandParentThumb")))
            CTextureCache::Get().CacheImage(item->GetArt("grandParentThumb"));


          if (item->HasArt("bigPoster") &&
              !CTextureCache::Get().HasCachedImage(item->GetArt("bigPoster")))
            CTextureCache::Get().CacheImage(item->GetArt("bigPoster"));
      }

    }

    CLog::Log(LOGNOTICE,"Global Cache : Processing section %s took %f",Section->GetLabel().c_str(),timer.GetElapsedSeconds());
  }

  CLog::Log(LOGNOTICE,"Global Cache : Full operation took %f",timer.GetElapsedSeconds());

  CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, "Processing Complete.", GLOBAL_CACHING_DESC, 5000, false,500);
}


void CPlexGlobalCacher::OnExit()
{

    CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, "Precaching stopped.", GLOBAL_CACHING_DESC, 5000, false,500);
    m_globalCacher = NULL;

}
