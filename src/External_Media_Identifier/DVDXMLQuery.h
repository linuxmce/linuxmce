/**
 * DVDXMLQuery - a class which attempts to retrieve metadata for a DVD given a DVDID.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef DVDXMLQUERY_H
#define DVDXMLQUERY_H

#define DVDXML_API_USER "tschak909"
#define DVDXML_API_PASSWORD "hal9000"

#include <string>
#include <curl/curl.h>
#include <curl/easy.h>

using namespace std;

class DVDXMLQuery
{
  string m_sDVDId;

 protected:
  CURL* curl;
  string curlBuffer;

 public:
  DVDXMLQuery(string sDVDId);
  virtual ~DVDXMLQuery();

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
