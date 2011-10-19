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

#include "../../xbmc/AdvancedSettings.h"
#include "../../xbmc/SystemGlobals.h"
#include "../../xbmc/utils/log.h"
#include "WIN32Util.h"
#include "shellapi.h"
#include "dbghelp.h"

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                        CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

CSystemGlobals g_SystemGlobals;

// Minidump creation function 
LONG WINAPI CreateMiniDump( EXCEPTION_POINTERS* pEp ) 
{
  // Create the dump file where the xbmc.exe resides
  CStdString errorMsg;
  CStdString dumpFile;
  CDateTime now(CDateTime::GetCurrentDateTime());
  dumpFile.Format("%s\\Plex\\plex_crashlog-%04i%02i%02i-%02i%02i%02i.dmp", CWIN32Util::GetProfilePath().c_str(), now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond());
  HANDLE hFile = CreateFile(dumpFile.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 

  // Call MiniDumpWriteDump api with the dump file
  if ( hFile && ( hFile != INVALID_HANDLE_VALUE ) ) 
  {
    // Load the DBGHELP DLL
    HMODULE hDbgHelpDll = ::LoadLibrary("DBGHELP.DLL");       
    if (hDbgHelpDll)
    {
      MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDbgHelpDll, "MiniDumpWriteDump");
      if (pDump)
      {
        // Initialize minidump structure
        MINIDUMP_EXCEPTION_INFORMATION mdei; 
        mdei.ThreadId           = CThread::GetCurrentThreadId();
        mdei.ExceptionPointers  = pEp; 
        mdei.ClientPointers     = FALSE; 

        // Call the minidump api with normal dumping
        // We can get more detail information by using other minidump types but the dump file will be
        // extermely large.
        BOOL rv = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mdei, 0, NULL); 
        if( !rv ) 
        {
          errorMsg.Format("MiniDumpWriteDump failed with error id %d", GetLastError());
          MessageBox(NULL, errorMsg.c_str(), "Plex: Error", MB_OK|MB_ICONERROR); 
        } 
      }
      else
      {
        errorMsg.Format("MiniDumpWriteDump failed to load with error id %d", GetLastError());
        MessageBox(NULL, errorMsg.c_str(), "Plex: Error", MB_OK|MB_ICONERROR);
      }
      
      // Close the DLL
      FreeLibrary(hDbgHelpDll);
    }
    else
    {
      errorMsg.Format("LoadLibrary 'DBGHELP.DLL' failed with error id %d", GetLastError());
      MessageBox(NULL, errorMsg.c_str(), "Plex: Error", MB_OK|MB_ICONERROR);
    }

    // Close the file 
    CloseHandle( hFile ); 
  }
  else 
  {
    errorMsg.Format("CreateFile '%s' failed with error id %d", dumpFile.c_str(), GetLastError());
    MessageBox(NULL, errorMsg.c_str(), "Plex: Error", MB_OK|MB_ICONERROR); 
  }

  return pEp->ExceptionRecord->ExceptionCode;;
}

void CRT_invalid_parameter(const wchar_t* expression, const wchar_t* function, const wchar_t* file, 
                           unsigned int line, uintptr_t pReserved)
{
  if (function && file)
  {
    CStdString strUTF8function, strUTF8file;
    g_charsetConverter.wToUTF8(CStdStringW(function), strUTF8function);
    g_charsetConverter.wToUTF8(CStdStringW(file), strUTF8file);

    CLog::Log(LOGSEVERE,"CRT_invalid_parameter: function %s. File: %s Line: %d", strUTF8function.c_str(), strUTF8file.c_str(), line);

    if (expression)
    {
      CStdString strUTF8expression;
      g_charsetConverter.wToUTF8(CStdStringW(expression), strUTF8expression);
      CLog::Log(LOGSEVERE,"CRT_invalid_parameter: expression: %s", strUTF8expression.c_str());
    }
  }
  else
  {
    CLog::Log(LOGSEVERE,"CRT_invalid_parameter: no function or file provided");
  }
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR commandLine, INT )
{
  //this can't be set from CAdvancedSettings::Initialize() because it will overwrite
  //the loglevel set with the --debug flag
#ifdef _DEBUG
  g_advancedSettings.m_logLevel     = LOG_LEVEL_DEBUG;
  g_advancedSettings.m_logLevelHint = LOG_LEVEL_DEBUG;
#else
  g_advancedSettings.m_logLevel     = LOG_LEVEL_NORMAL;
  g_advancedSettings.m_logLevelHint = LOG_LEVEL_NORMAL;
#endif

  // Initializes CreateMiniDump to handle exceptions.
  // SetUnhandledExceptionFilter(CreateMiniDump);
  //
  // We don't do this anymore...
  // It's generally a bad idea for a process to continue executing when an unhandled exception is thrown.
  // It's generally much better to simply let the system handle creating dumps as per system policy.
  // A classic example is heap corruption. CreateMiniDump could crash again allocating strings, thus obscuring the original issue.

  // we don't want to see the "no disc in drive" windows message box
  SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

  // Turn off CRT auto-termination behavior
  _set_invalid_parameter_handler(CRT_invalid_parameter);

  // Check if XBMC is already running
  // The mutex will be closed when the process exits
  HANDLE hMutex = CreateMutex(NULL, FALSE, "9034bd65-6bec-49be-bb42-4943c9776295");
  if (hMutex == NULL)
  {
    CLog::FatalError("CreateMutex failed: 0x%x", GetLastError());
  }
  if(GetLastError() == ERROR_ALREADY_EXISTS)
  {
    HWND m_hwnd = FindWindow("XBMC","Plex");
    if(m_hwnd != NULL)
    {
      // switch to the running instance
      ShowWindow(m_hwnd,SW_RESTORE);
      SetForegroundWindow(m_hwnd);
    }
    return 0;
  }

#ifndef HAS_DX
  if(CWIN32Util::GetDesktopColorDepth() < 32)
  {
    //FIXME: replace it by a SDL window for all ports
    MessageBox(NULL, "Desktop Color Depth isn't 32Bit", "XBMC: Fatal Error", MB_OK|MB_ICONERROR);
    return 0;
  }
#endif

  //Initialize COM
  HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (FAILED(hr))
  {
    CLog::FatalError("CoInitializeEx failed: 0x%x", hr);
  }

  // parse the command line
  CStdStringW strcl(commandLine);
  LPWSTR *szArglist;
  int nArgs;

  setlocale(LC_NUMERIC, "C");
  g_advancedSettings.Initialize();
  szArglist = CommandLineToArgvW(strcl.c_str(), &nArgs);
  if(szArglist != NULL)
  {
    for(int i=0;i<nArgs;i++)
    {
      CStdStringW strArgW(szArglist[i]);
      if(strArgW.Equals(L"-fs"))
        g_advancedSettings.m_startFullScreen = true;
      else if(strArgW.Equals(L"-p") || strArgW.Equals(L"--portable"))
        g_application.EnablePlatformDirectories(false);
      else if(strArgW.Equals(L"-d"))
      {
        if(++i < nArgs)
        {
          int iSleep = _wtoi(szArglist[i]);
          if(iSleep > 0 && iSleep < 360)
            Sleep(iSleep*1000);
          else
            --i;
        }
      }
      else if(strArgW.Equals(L"--debug"))
      {
        g_advancedSettings.m_logLevel     = LOG_LEVEL_DEBUG;
        g_advancedSettings.m_logLevelHint = LOG_LEVEL_DEBUG;
      }
    }
    LocalFree(szArglist);
  }

  WSADATA wd;
  int wsaret = WSAStartup(MAKEWORD(2,2), &wd);
  if (wsaret != 0)
  {
    CLog::FatalError("WSAStartup failed: 0x%x", wsaret);
  }

  // Create and run the app
  if(!g_application.Create())
  {
    CStdString errorMsg;
    errorMsg.Format("CApplication::Create() failed - check log file and that it is writable");
    MessageBox(NULL, errorMsg.c_str(), "Plex: Error", MB_OK|MB_ICONERROR);
    return 0;
  }

  g_application.Run();
  
  // put everything in CApplication::Cleanup() since this point is never reached

  return 0;
}
