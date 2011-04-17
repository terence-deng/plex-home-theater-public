#pragma once

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

#include <map>
#include <string>

#include "GUIWindow.h"
#include "GUIWindowPlexSearch.h"

// List IDs.
#define CONTENT_LIST_RECENTLY_ADDED    11000
#define CONTENT_LIST_ON_DECK           11001
#define CONTENT_LIST_RECENTLY_ACCESSED 11002

class CGUIWindowHome :
      public CGUIWindow
{
public:
  CGUIWindowHome(void);
  virtual ~CGUIWindowHome(void);
  
private:
  virtual bool OnAction(const CAction &action);
  virtual bool OnMessage(CGUIMessage& message);
  virtual bool OnPopupMenu();
  virtual bool CheckTimer(const CStdString& strExisting, const CStdString& strNew, int title, int line1, int line2);
  virtual void UpdateContentForSelectedItem(int itemID);

  int m_lastSelectedItem;
  int m_lastSelectedID;
  
  std::map<int, std::string> m_idToSectionUrlMap;
  std::map<int, int>         m_idToSectionTypeMap;
  std::map<int, Group>       m_contentLists;
};
