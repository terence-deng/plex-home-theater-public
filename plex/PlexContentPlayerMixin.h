/*
 *  Copyright (C) 2011 Plex, Inc.   
 *      Author: Elan Feingold
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include "Application.h"
#include "FileItem.h"
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
          g_windowManager.ActivateWindow(WINDOW_VIDEO_FILES, file->m_strPath + ",return");
        }
        else if (type == "artist" || type == "album")
        {
          g_windowManager.ActivateWindow(WINDOW_MUSIC_FILES, file->m_strPath + ",return");
        }
        else if (type == "track")
        {
          // Get album.
          CFileItemList  fileItems;
          CPlexDirectory plexDir;
          plexDir.GetDirectory(file->GetProperty("parentPath"), fileItems);
          int itemIndex = -1;
          
          for (int i=0; i < fileItems.Size(); ++i)
          {
            CFileItemPtr fileItem = fileItems[i];
            if (fileItem->GetProperty("unprocessedKey") == file->GetProperty("unprocessedKey"))
            {
              itemIndex = i;
              break;
            }
          }
          
          g_playlistPlayer.ClearPlaylist(PLAYLIST_MUSIC);
          g_playlistPlayer.Reset();
          g_playlistPlayer.Add(PLAYLIST_MUSIC, fileItems);
          g_playlistPlayer.SetCurrentPlaylist(PLAYLIST_MUSIC);
          g_playlistPlayer.Play(itemIndex);
        }
        else
        {
          bool resumeItem = false;
          
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
            if (!retVal)
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
};
