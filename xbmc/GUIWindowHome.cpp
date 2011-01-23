/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <boost/foreach.hpp>
#include <list>
#include <vector>

#include "FileSystem/File.h"
#include "FileItem.h"
#include "GUIBaseContainer.h"
#include "GUIStaticItem.h"
#include "GUIWindowHome.h"
#include "GUIWindowManager.h"
#include "GUIUserMessages.h"
#include "Key.h"
#include "PlexSourceScanner.h"

using namespace std;
using namespace XFILE;

#define MAIN_MENU         9000 // THIS WAS 300 for Plex skin.

CGUIWindowHome::CGUIWindowHome(void) : CGUIWindow(WINDOW_HOME, "Home.xml")
  , m_lastSelectedItem(-1)
{
}

CGUIWindowHome::~CGUIWindowHome(void)
{
}

typedef pair<string, HostSourcesPtr> string_sources_pair;

static bool compare(CFileItemPtr first, CFileItemPtr second)
{
  return first->GetLabel() <= second->GetLabel();
}

bool CGUIWindowHome::OnMessage(CGUIMessage& message)
{
  if (message.GetMessage() ==  GUI_MSG_WINDOW_DEINIT)
  {
    CGUIBaseContainer* control = (CGUIBaseContainer* )GetControl(MAIN_MENU);
    if (control == 0)
      control = (CGUIBaseContainer* )GetControl(300);
    
    m_lastSelectedItem = control->GetSelectedItem();
  }

  bool ret = CGUIWindow::OnMessage(message);
  
  switch (message.GetMessage())
  {
  case GUI_MSG_WINDOW_INIT:
  case GUI_MSG_WINDOW_RESET:
  case GUI_MSG_UPDATE_MAIN_MENU:
  {
    // This will be our new list.
    vector<CGUIListItemPtr> newList;
    
    // Get the old list.
    CGUIBaseContainer* control = (CGUIBaseContainer* )GetControl(MAIN_MENU);
    if (control == 0)
      control = (CGUIBaseContainer* )GetControl(300);
    
    if (control)
    {
      vector<CGUIListItemPtr>& oldList = control->GetStaticItems();
      
      // First collect all the real items.
      BOOST_FOREACH(CGUIListItemPtr item, oldList)
      {
        if (item->HasProperty("plex") == false)
          newList.push_back(item);
      }
      
      // Now collect all the added items.
      CPlexSourceScanner::Lock();
      
      map<string, int> nameCounts;
      map<string, HostSourcesPtr>& map = CPlexSourceScanner::GetMap();
      list<CFileItemPtr> newItems;
      BOOST_FOREACH(string_sources_pair nameSource, map)
      {
        for (int i=0; i<nameSource.second->librarySections.Size(); i++)
        {
          newItems.push_back(nameSource.second->librarySections[i]);
          CStdString sectionName = nameSource.second->librarySections[i]->GetLabel();
          ++nameCounts[sectionName.ToLower()];
        }
      }

      CPlexSourceScanner::Unlock();
      
      // Now sort them according to name.
      newItems.sort(compare);

      // Now add the new ones.
      int id = 1000;
      BOOST_FOREACH(CFileItemPtr item, newItems)
      {
        CFileItemPtr newItem = CFileItemPtr(new CGUIStaticItem());
        newItem->SetLabel(item->GetLabel());
        newItem->SetProperty("plex", "1");
        
        CStdString sectionName = item->GetLabel();
        if (nameCounts[sectionName.ToLower()] > 1)
          newItem->SetLabel2(item->GetLabel2());
       
        if (item->GetProperty("type") == "artist")
          newItem->m_strPath = "XBMC.ActivateWindow(MyMusicFiles," + item->m_strPath + ",return)";
        else
          newItem->m_strPath = "XBMC.ActivateWindow(MyVideoFiles," + item->m_strPath + ",return)";
        
        newItem->m_idepth = 0;
        newItem->SetQuickFanart(item->GetQuickFanart());
        newItem->m_iprogramCount = id++;

        // Load and set fanart.
        //newItem->CacheFanart(); FIXME
        //if (CFile::Exists(newItem->GetCachedProgramFanart()))
        //  newItem->SetProperty("fanart_image", newItem->GetCachedProgramFanart());

        newList.push_back(newItem);
      }

      // Replace 'em.
      control->GetStaticItems().clear();
      control->GetStaticItems().assign(newList.begin(), newList.end());
      
      // See if we have a selected item and restore it if we do.
      if (m_lastSelectedItem >= 0)
      {
        CGUIMessage msg(GUI_MSG_SETFOCUS, GetID(), control->GetID(), m_lastSelectedItem+1, 0);
        g_windowManager.SendThreadMessage(msg);
      }
    }
  }
  break;
  }
  
  return ret;
}
