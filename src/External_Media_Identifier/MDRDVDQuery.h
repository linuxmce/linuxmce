/**
 * MDRDVDQuery - Query Microsoft Windows Media Server services for a DVD 
 * given a CRC64 calculated by dvdid
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef MDRDVDQUERY_H
#define MDRDVDQUERY_H

#include <string>
#include <curl/curl.h>
#include <curl/easy.h>

using namespace std;

class MDRDVDQuery
{
  string m_sDVDId;
  
 protected:
  CURL* curl;
  string curlBuffer;
  
 public:
  MDRDVDQuery(string sDVDId);
  virtual ~MDRDVDQuery();
  
  static int curlWriter(char *data, size_t size, size_t nmemb, string *buffer) {
    int result = 0;
    if (buffer != NULL) {
      buffer->append(data, size * nmemb);
      result = size * nmemb;
    }
    return result;
  }

  bool execute();

  string DVDId_get() {return m_sDVDId;}
  string Reply_get() { return curlBuffer; }
  
};

#endif
