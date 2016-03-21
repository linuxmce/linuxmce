/**
 * HTTPPicture - an object that grabs and returns a picture with format type
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @version 1.0
 */

#include "HTTPPicture.h"

HTTPPicture::HTTPPicture(string sURL)
{
  m_sURL=sURL;
  m_sData="";
}

HTTPPicture::~HTTPPicture()
{
  m_sURL="";
  m_sData="";
}

bool HTTPPicture::get()
{
  CURLcode res;
  struct curl_slist *headerList= NULL;
  string sHeader="";

  curl = curl_easy_init();
  
  if (!curl)
    return false;
  
  curl_slist_append(headerList, sHeader.c_str());
  
  if (!curl)
    {
      return false;
    }
  
  curl_easy_setopt(curl, CURLOPT_URL, m_sURL.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_PORT, 80);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &HTTPPicture::curlWriter);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_sData);
  res = curl_easy_perform(curl);

  return true;
}

// int main(int argc, char** argv)
// {
//   HTTPPicture* p = new HTTPPicture("http://i.imgur.com/pBR94Z9.jpg");
//   if (p->get())
//     {
//       FileUtils::WriteTextFile("/tmp/foo.jpg",p->Data_get());
//     }
//   delete p;
// }
