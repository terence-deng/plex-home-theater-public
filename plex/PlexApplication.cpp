/*
 *  PlexApplication.cpp
 *  XBMC
 *
 *  Created by Jamie Kirkpatrick on 20/01/2011.
 *  Copyright 2011 Plex Inc. All rights reserved.
 *
 */

#include "PlexNetworkServices.h"
#include "PlexApplication.h"
#include "BackgroundMusicPlayer.h"
#include "GUIUserMessages.h"

////////////////////////////////////////////////////////////////////////////////
PlexApplicationPtr PlexApplication::Create()
{
  return PlexApplicationPtr( new PlexApplication() );
}

////////////////////////////////////////////////////////////////////////////////
PlexApplication::PlexApplication()
{
  m_serviceListener = PlexServiceListener::Create();
  m_bgMusicPlayer = BackgroundMusicPlayer::Create();
}

////////////////////////////////////////////////////////////////////////////////
bool PlexApplication::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
    case GUI_MSG_APP_ACTIVATED:
    case GUI_MSG_APP_DEACTIVATED:
    {
      CLog::Log(LOGDEBUG,"Plex Application: Handling message %d", message.GetMessage());
      return true;
    }
    case GUI_MSG_BG_MUSIC_SETTINGS_UPDATED:
    {
      m_bgMusicPlayer->SyncSettings();
      return true;
    }
    case GUI_MSG_BG_MUSIC_THEME_UPDATED:
    {
			m_bgMusicPlayer->SetTheme(message.GetStringParam());
      return true;
    }
  }
  
  return false;
}

////////////////////////////////////////////////////////////////////////////////
void PlexApplication::SetGlobalVolume(int volume)
{
	m_bgMusicPlayer->SetGlobalVolumeAsPercent(volume);
}