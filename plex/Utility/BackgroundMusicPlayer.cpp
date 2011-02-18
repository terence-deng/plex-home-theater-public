//
//  BackgroundMusicPlayer.cpp
//  Plex
//
//  Created by Jamie Kirkpatrick on 29/01/2011.
//  Copyright 2011 Kirk Consulting Limited. All rights reserved.
//

#include "BackgroundMusicPlayer.h"
#include "FileItem.h"
#include "GUISettings.h"
#include "GUIUserMessages.h"
#include "GUIWindowManager.h"
#include "Log.h"
#include "cores/playercorefactory/PlayerCoreFactory.h"

////////////////////////////////////////////////////////////////////////////////
BackgroundMusicPlayerPtr BackgroundMusicPlayer::Create()
{
  return BackgroundMusicPlayerPtr( new BackgroundMusicPlayer() );
}

////////////////////////////////////////////////////////////////////////////////
void BackgroundMusicPlayer::SendThemeChangeMessage(const CStdString& theme)
{
  CGUIMessage msg(GUI_MSG_BG_MUSIC_THEME_UPDATED, 0, 0);
  msg.SetStringParam(theme);
  g_windowManager.SendMessage(msg); 
}

////////////////////////////////////////////////////////////////////////////////
BackgroundMusicPlayer::BackgroundMusicPlayer()
{
  m_globalVolume = 100;
  m_player.reset(CPlayerCoreFactory::CreatePlayer(EPC_DVDPLAYER, *this));
  SyncSettings();
}

////////////////////////////////////////////////////////////////////////////////
void BackgroundMusicPlayer::SyncSettings()
{
  CLog::Log(LOGDEBUG,"Updating background music settings");
  
  m_isEnabled = g_guiSettings.GetBool("backgroundmusic.thememusicenabled");
  m_volume = g_guiSettings.GetInt("backgroundmusic.bgmusicvolume")/100.0f;
}

////////////////////////////////////////////////////////////////////////////////
void BackgroundMusicPlayer::SetGlobalVolumeAsPercent(int volume)
{
  return;
}

////////////////////////////////////////////////////////////////////////////////
void BackgroundMusicPlayer::SetTheme(const CStdString& theme)
{
  if (m_theme.Equals(theme))
    return;
  
  m_theme = theme;
  
  CLog::Log(LOGDEBUG,"Background music theme changed to: %s", theme.c_str());
  PlayCurrentTheme();
}

////////////////////////////////////////////////////////////////////////////////
void BackgroundMusicPlayer::OnPlayBackEnded()
{
  PlayCurrentTheme();
}

////////////////////////////////////////////////////////////////////////////////
void BackgroundMusicPlayer::PlayCurrentTheme()
{
  if (m_theme.size())
    m_player->OpenFile(CFileItem(m_theme, false), CPlayerOptions());
  else if (m_player->IsPlaying())
  	m_player->CloseFile();
}
