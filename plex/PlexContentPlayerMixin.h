/*
 *  Copyright (C) 2011 Plex, Inc.   
 *      Author: Elan Feingold
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include "Application.h"
#include "FileItem.h"
#include "GUISettings.h"
#include "GUIBaseContainer.h"
#include "GUIDialogContextMenu.h"
#include "GUIWindowManager.h"
#include "LocalizeStrings.h"
#include "PlayListPlayer.h"
#include "PlexDirectory.h"
#include "StringUtils.h"

class PlexContentPlayerMixin
{
 protected:
  
  /// Filled in by subclasses if needed.
  virtual void SaveStateBeforePlay(CGUIBaseContainer* container) {}
  
  /// Play the item selected in the specified container.
  void PlayFileFromContainer(const CGUIControl* control)
  {
    if (control == 0)
      return;
    
    // Let's see if we're asked to play something.
    if (control->IsContainer())
    {
      CGUIBaseContainer* container = (CGUIBaseContainer* )control;
      CGUIListItemPtr item = container->GetListItem(0);

      if (item)
      {
        CFileItem* file = (CFileItem* )item.get();
    
        // Now see what to do with it.
        string type = file->GetProperty("type");
        if (type == "show" || type == "person")
        {
          ActivateWindow(WINDOW_VIDEO_FILES, file->m_strPath);
        }
        else if (type == "artist" || type == "album")
        {
          ActivateWindow(WINDOW_MUSIC_FILES, file->m_strPath);
        }
        else if (type == "track")
        {
          CFileItemList fileItems;
          int itemIndex = 0;

          if (file->HasProperty("parentPath"))
          {
            // Get album.
            CPlexDirectory plexDir;
            plexDir.GetDirectory(file->GetProperty("parentPath"), fileItems);
            
            for (int i=0; i < fileItems.Size(); ++i)
            {
              CFileItemPtr fileItem = fileItems[i];
              if (fileItem->GetProperty("unprocessedKey") == file->GetProperty("unprocessedKey"))
              {
                itemIndex = i;
                break;
              }
            }
          }
          else
          {
            // Just add the track.
            CFileItemPtr theTrack(new CFileItem(*file));
            fileItems.Add(theTrack);
          }
          
          g_playlistPlayer.ClearPlaylist(PLAYLIST_MUSIC);
          g_playlistPlayer.Reset();
          g_playlistPlayer.Add(PLAYLIST_MUSIC, fileItems);
          g_playlistPlayer.SetCurrentPlaylist(PLAYLIST_MUSIC);
          g_playlistPlayer.Play(itemIndex);
        }
        else if (type == "channel")
        {
          if (file->m_strPath.find("/video/") != string::npos)
            ActivateWindow(WINDOW_VIDEO_FILES, file->m_strPath);
          else if (file->m_strPath.find("/music/") != string::npos)
            ActivateWindow(WINDOW_MUSIC_FILES, file->m_strPath);
          else
            ActivateWindow(WINDOW_PICTURES, file->m_strPath);
        }
        else
        {
          bool resumeItem = false;
          
          // If there is more than one media item, allow picking which one.
          if (file->m_mediaItems.size() > 1 && g_guiSettings.GetBool("videoplayer.alternatemedia") == true)
          {
            CFileItemList   fileItems;
            CContextButtons choices;
            CPlexDirectory  mediaChoices;
            
            for (size_t i=0; i < file->m_mediaItems.size(); i++)
            {
              CFileItemPtr item = file->m_mediaItems[i];
              
              CStdString label;
              CStdString videoCodec = item->GetProperty("mediaTag-videoCodec").ToUpper();
              CStdString videoRes = item->GetProperty("mediaTag-videoResolution").ToUpper();
              
              if (videoCodec.size() == 0 && videoRes.size() == 0)
              {
                label = "Unknown";
              }
              else
              {
                if (isdigit(videoRes[0]))
                  videoRes += "p";
              
                label += videoRes;
                label += " " + videoCodec;
              }
              
              choices.Add(i, label);
            }
            
            int choice = CGUIDialogContextMenu::ShowAndGetChoice(choices);
            if (choice >= 0)
              file->m_strPath = file->m_mediaItems[choice]->m_strPath;
            else
              return;
          }
          
          if (!file->m_bIsFolder && file->HasProperty("viewOffset")) 
          {
            // Oh my god. Copy and paste code. We need a superclass which manages media.
            float seconds = boost::lexical_cast<int>(file->GetProperty("viewOffset")) / 1000.0f;

            CContextButtons choices;
            CStdString resumeString;
            CStdString time = StringUtils::SecondsToTimeString(seconds);
            resumeString.Format(g_localizeStrings.Get(12022).c_str(), time.c_str());
            choices.Add(1, resumeString);
            choices.Add(2, g_localizeStrings.Get(12021));
            int retVal = CGUIDialogContextMenu::ShowAndGetChoice(choices);
            if (retVal == -1)
              return;

            resumeItem = (retVal == 1);
          }

          if (resumeItem)
            file->m_lStartOffset = STARTOFFSET_RESUME;
          else
            file->m_lStartOffset = 0;

          // Allow class to save state.
          SaveStateBeforePlay(container);
          
          // Play it.
          g_application.PlayFile(*file);
        }
      }
    }
  }
  
 private:
  
  void ActivateWindow(int window, const CStdString& path)
  {
    CStdString strWindow = (window == WINDOW_VIDEO_FILES) ? "MyVideoFiles" : (window == WINDOW_MUSIC_FILES) ? "MyMusicFiles" : "MyPictures";
    CStdString cmd = "XBMC.ActivateWindow(MyMusicFiles," + path + ",return)";
    
    g_application.ExecuteXBMCAction(cmd);
  }
};
