/**
 * HTTPPicture - an object that grabs and returns a picture with format type
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @version 1.0
 */

#ifndef HTTPPICTURE_H
#define HTTPPICTURE_H

#include <string>
#include <curl/curl.h>
#include <curl/easy.h>

using namespace std;

class HTTPPicture
{
 private:
  string m_sURL;

 protected:
  CURL* curl;
  string m_sData;

 public:
  HTTPPicture(string sURL);
  virtual ~HTTPPicture();

  static int curlWriter(char *data, size_t size, size_t nmemb, string *buffer) {
    int result = 0;
    if (buffer != NULL) {
      buffer->append(data, size * nmemb);
      result = size * nmemb;
    }
    return result;
  }

  bool get(); 
  string URL_get() {return m_sURL;}
  string Data_get() {return m_sData;}
};

#endif /* HTTPICTURE_H */
