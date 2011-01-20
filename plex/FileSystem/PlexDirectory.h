#pragma once

/*
 * PlexDirectory.h
 *
 *  Created on: Oct 4, 2008
 *      Author: Elan Feingold
 */
#include <string>

#include "FileCurl.h"
#include "IDirectory.h"
#include "Thread.h"
#include "SortFileItem.h"

class CURL;
class TiXmlElement;
using namespace std;
using namespace XFILE;

class CPlexDirectory : public IDirectory, 
                       public CThread
{
 public:
  CPlexDirectory(bool parseResults=true, bool displayDialog=true);
  virtual ~CPlexDirectory();
  
  virtual bool GetDirectory(const CStdString& strPath, CFileItemList &items);
  virtual DIR_CACHE_TYPE GetCacheType(const CStdString &strPath) const { return m_dirCacheType; };
  static std::string ProcessUrl(const std::string& parent, const std::string& url, bool isDirectory);
  virtual void SetTimeout(int timeout) { m_timeout = timeout; }
  
  std::string GetData() { return m_data; } 
  
  static std::string ProcessMediaElement(const std::string& parentPath, const char* mediaURL, int maxAge, bool local);
  static std::string BuildImageURL(const std::string& parentURL, const std::string& imageURL, bool local);
  
 protected:
  
  virtual void Process();
  virtual void OnExit();
  virtual void StopThread();
  
  void Parse(const CURL& url, TiXmlElement* root, CFileItemList &items, std::string& strFileLabel, std::string& strSecondFileLabel, std::string& strDirLabel, std::string& strSecondDirLabel, bool isLocal);
  void ParseTags(TiXmlElement* element, const CFileItemPtr& item, const std::string& name);
  
  CEvent     m_downloadEvent;
  bool       m_bStop;
  
  CStdString m_url;
  CStdString m_data;
  bool       m_bSuccess;
  bool       m_bParseResults;
  int        m_timeout;
  CFileCurl  m_http;
  DIR_CACHE_TYPE m_dirCacheType;
};

