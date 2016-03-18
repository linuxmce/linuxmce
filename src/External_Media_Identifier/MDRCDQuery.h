/**
 * MDRCDQuery - Query Microsoft Windows Media Server services for a CD 
 * given a run of TOC offsets calculated for a WM/MCDI ID
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef MDRCDQUERY_H
#define MDRCDQUERY_H

#include <string>
#include <curl/curl.h>
#include <curl/easy.h>

using namespace std;

class MDRCDQuery
{
  string m_sCDId;
  
 protected:
  CURL* curl;
  string curlBuffer;
  
 public:
  MDRCDQuery(string sCDId);
  virtual ~MDRCDQuery();
  
  static int curlWriter(char *data, size_t size, size_t nmemb, string *buffer) {
    int result = 0;
    if (buffer != NULL) {
      buffer->append(data, size * nmemb);
      result = size * nmemb;
    }
    return result;
  }

  bool execute();

  string CDId_get() {return m_sCDId;}
  string Reply_get() { return curlBuffer; }
  
};

#endif
