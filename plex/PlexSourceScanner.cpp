/*
 *  PlexSourceScanner.cpp
 *  Plex
 *
 *  Created by Elan Feingold & James Clarke on 13/11/2009.
 *  Copyright 2009 Plex Development Team. All rights reserved.
 *
 */
#include "Log.h"
#include "File.h"
#include "FileItem.h"
#include "Key.h"
#include "Picture.h"
#include "PlexDirectory.h"
#include "PlexUtils.h"
#include "PlexSourceScanner.h"
#include "Util.h"
#include "GUIWindowManager.h"
#include "GUIUserMessages.h"
#include "CocoaUtilsPlus.h"

map<std::string, HostSourcesPtr> CPlexSourceScanner::g_hostSourcesMap;
CCriticalSection CPlexSourceScanner::g_lock;
int CPlexSourceScanner::g_activeScannerCount = 0;

using namespace XFILE; 

void CPlexSourceScanner::Process()
{
  CStdString path;
  
  CLog::Log(LOGNOTICE, "Plex Source Scanner starting...(%s)", m_host.c_str());
  
  { // Make sure any existing entry is removed.
    CSingleLock lock(g_lock);
    g_hostSourcesMap.erase(m_host);
    g_activeScannerCount++;
  }
  
  if (m_host.find("members.mac.com") != std::string::npos)
  {
    CLog::Log(LOGWARNING, "Skipping MobileMe address: %s", m_host.c_str());
  }
  else
  {
    // Compute the real host label (empty for local server).
    std::string realHostLabel = m_hostLabel;
    bool onlyShared = false;

    // Act a bit differently if we're talking to a local server.
    if (Cocoa_IsHostLocal(m_host) == true)
    {
      realHostLabel = "";
      onlyShared = false;
    }
    
    // Create a new entry.
    HostSourcesPtr sources = HostSourcesPtr(new HostSources());
    CLog::Log(LOGNOTICE, "Scanning remote server: %s", m_host.c_str());
    
    // Scan the server.
    path.Format("%s/music/", m_url);
    AutodetectPlexSources(path, sources->musicSources, realHostLabel, onlyShared);
    
    path.Format("%s/video/", m_url);
    AutodetectPlexSources(path, sources->videoSources, realHostLabel, onlyShared);
    
    path.Format("%s/photos/", m_url);
    AutodetectPlexSources(path, sources->pictureSources, realHostLabel, onlyShared);

    // Library sections.
    path.Format("%s/library/sections", m_url);
    CPlexDirectory plexDir(true, false);
    plexDir.SetTimeout(5);
    sources->librarySections.ClearItems();
    plexDir.GetDirectory(path, sources->librarySections);
    
    // Edit for friendly name.
    for (int i=0; i<sources->librarySections.Size(); i++)
    {
      CFileItemPtr item = sources->librarySections[i];
      item->SetLabel2(m_hostLabel);
      CLog::Log(LOGNOTICE, " -> Section '%s' found.", item->GetLabel().c_str());
    }
    
    { // Add the entry to the map.
      CSingleLock lock(g_lock);
      g_hostSourcesMap[m_host] = sources;
    }
    
    // Notify the UI.
    CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE_REMOTE_SOURCES);
    g_windowManager.SendThreadMessage(msg);
    
    // Notify the main menu.
    CGUIMessage msg2(GUI_MSG_UPDATE_MAIN_MENU, WINDOW_HOME, 300);
    g_windowManager.SendThreadMessage(msg2);
  
    CLog::Log(LOGNOTICE, "Scanning host %s is complete.", m_host.c_str());
  }
  
  CSingleLock lock(g_lock);
  g_activeScannerCount--;
  
  CLog::Log(LOGNOTICE, "Plex Source Scanner finished for host %s (%d left)", m_host.c_str(), g_activeScannerCount);
}

void CPlexSourceScanner::ScanHost(const std::string& host, const std::string& hostLabel, const std::string& url)
{
  new CPlexSourceScanner(host, hostLabel, url);
}

void CPlexSourceScanner::RemoveHost(const std::string& host)
{
  { // Remove the entry from the map in case it was remote.
    CSingleLock lock(g_lock);
    g_hostSourcesMap.erase(host);
  }
  
  // Notify the UI.
  CLog::Log(LOGNOTICE, "Notifying remote remove host on %s", host.c_str());
  CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE_REMOTE_SOURCES);
  g_windowManager.SendThreadMessage(msg);
  
  CGUIMessage msg2(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE_SOURCES);
  g_windowManager.SendThreadMessage(msg2);
  
  CGUIMessage msg3(GUI_MSG_UPDATE_MAIN_MENU, WINDOW_HOME, 300);
  g_windowManager.SendThreadMessage(msg3);
}

void CPlexSourceScanner::MergeSourcesForWindow(int windowId)
{
  CSingleLock lock(g_lock);
  switch (windowId) 
  {
    case WINDOW_MUSIC_FILES:
      BOOST_FOREACH(StringSourcesPair pair, g_hostSourcesMap)
        MergeSource(g_settings.m_musicSources, pair.second->musicSources);
      CheckForRemovedSources(g_settings.m_musicSources, windowId);
      break;
      
    case WINDOW_PICTURES:
      BOOST_FOREACH(StringSourcesPair pair, g_hostSourcesMap)
        MergeSource(g_settings.m_pictureSources, pair.second->pictureSources);
      CheckForRemovedSources(g_settings.m_pictureSources, windowId);
      break;
      
    case WINDOW_VIDEO_FILES:
      BOOST_FOREACH(StringSourcesPair pair, g_hostSourcesMap)
        MergeSource(g_settings.m_videoSources, pair.second->videoSources);
      CheckForRemovedSources(g_settings.m_videoSources, windowId);
      break;
      
    default:
      break;   
  }
}

void CPlexSourceScanner::MergeSource(VECSOURCES& sources, VECSOURCES& remoteSources)
{
  BOOST_FOREACH(CMediaSource source, remoteSources)
  {
    // If the source doesn't already exist, add it.
    bool bIsSourceName = true;
    if (CUtil::GetMatchingSource(source.strName, sources, bIsSourceName) < 0)
    {
      #pragma warning fix this
      //source.m_autoDetected = true;
      sources.push_back(source);
    }
  }
}

void CPlexSourceScanner::CheckForRemovedSources(VECSOURCES& sources, int windowId)
{
  VECSOURCES::iterator iterSources = sources.begin();
  while (iterSources != sources.end()) 
  {
    CMediaSource source = *iterSources;
    bool bFound = true;
    
    if (source.m_autoDetected)
    {
      bool bIsSourceName = true;
      bFound = false;
      BOOST_FOREACH(StringSourcesPair pair, g_hostSourcesMap)
      {
        VECSOURCES remoteSources;
        switch (windowId) 
        {
          case WINDOW_MUSIC_FILES:
            remoteSources = pair.second->musicSources;
            break;
          case WINDOW_PICTURES:
            remoteSources = pair.second->pictureSources;
            break;
          case WINDOW_VIDEO_FILES:
            remoteSources = pair.second->videoSources;
            break;
          default:
            return;
        }
        
        if (CUtil::GetMatchingSource(source.strName, remoteSources, bIsSourceName) >= 0)
          bFound = true;
      }
    }
    
    if (!bFound)
      sources.erase(iterSources);
    else
      ++iterSources;
  }  
}

void CPlexSourceScanner::AutodetectPlexSources(CStdString strPlexPath, VECSOURCES& dstSources, CStdString strLabel, bool onlyShared)
{
  bool bIsSourceName = true;
  bool bPerformRemove = true;
  
  // Auto-add PMS sources
  VECSOURCES pmsSources;
  CFileItemList* fileItems = new CFileItemList();
  CPlexDirectory plexDir;
  plexDir.SetTimeout(2);
  
  CUtil::AddSlashAtEnd(strPlexPath);
  if (plexDir.GetDirectory(strPlexPath, *fileItems))
  {
    // Make sure all items in the PlexDirectory are added as sources
    for ( int i = 0; i < fileItems->Size(); i++ )
    {
      CFileItemPtr item = fileItems->Get(i);
      if ((!onlyShared) || item->HasProperty("share"))
      {
        CMediaSource share;
        share.strName = item->GetLabel();
        
        // Add the label (if provided
        if (strLabel != "")
          share.strName.Format("%s (%s)", share.strName, strLabel);
        
        // Get special attributes for PMS sources
        if (item->HasProperty("hasPrefs"))
          share.hasPrefs = item->GetPropertyBOOL("hasPrefs");
        
        if (item->HasProperty("pluginIdentifer"))
          share.strPluginIdentifer = item->GetProperty("pluginIdentifier");
        
        if (item->HasProperty("hasStoreServices"))
          share.hasStoreServices = item->GetPropertyBOOL("hasStoreServices");
        
        share.strPath = item->m_strPath;
        share.m_strFanartUrl = item->GetQuickFanart();
        share.m_ignore = true;
        
        // Download thumbnail if needed.
        CStdString cachedThumb(item->GetCachedPlexMediaServerThumb());
        CStdString thumb(item->GetThumbnailImage());
        
        if (CFile::Exists(cachedThumb))
        {
          item->SetThumbnailImage(cachedThumb);
        }
        else
        {
          CPicture pic;
          if(pic.CreateThumbnail(thumb, cachedThumb))
            item->SetThumbnailImage(cachedThumb);
          else
            item->SetThumbnailImage("");
        }
        
        share.m_strThumbnailImage = cachedThumb;
        
        // Fanart.
        if (!item->HasProperty("fanart_image"))
        {
          item->CacheLocalFanart();
          if (CFile::Exists(item->GetCachedProgramFanart()))
            item->SetProperty("fanart_image", item->GetCachedProgramFanart());
        }
        
        pmsSources.push_back(share);
        if (CUtil::GetMatchingSource(share.strName, dstSources, bIsSourceName) < 0)
          dstSources.push_back(share);
      }
    }
    delete fileItems;
    
    // Remove any local PMS sources that don't exist in the PlexDirectory
    for (int i = dstSources.size() - 1; i >= 0; i--)
    {
      CMediaSource share = dstSources.at(i);
      if ((share.strPath.find(strPlexPath) != string::npos) && (share.strPath.find("/", strPlexPath.length()) == share.strPath.length()-1))
      {
        if (CUtil::GetMatchingSource(dstSources.at(i).strName, pmsSources, bIsSourceName) < 0)
          dstSources.erase(dstSources.begin()+i);
      }
    }
    
    // Everything ran successfully - don't remove PMS sources
    bPerformRemove = false;
  }
  
  // If there was a problem connecting to the local PMS, remove local root sources
  if (bPerformRemove)
  {
    RemovePlexSources(strPlexPath, dstSources);
  }
}

void CPlexSourceScanner::RemovePlexSources(CStdString strPlexPath, VECSOURCES& dstSources)
{
  for ( int i = dstSources.size() - 1; i >= 0; i--)
  {
    CMediaSource share = dstSources.at(i);
    if ((share.strPath.find(strPlexPath) != string::npos) && (share.strPath.find("/", strPlexPath.length()) == share.strPath.length()-1))
      dstSources.erase(dstSources.begin()+i);
  }
}

