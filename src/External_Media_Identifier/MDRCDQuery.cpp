/**
 * MDRCDQuery - Query Microsoft Windows Media Server services for a CD 
 * given a run of TOC values including leadout. (aka WM/MCDI ID)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "MDRCDQuery.h"
#include <iostream>

MDRCDQuery::MDRCDQuery(string sCDId)
{
  m_sCDId=sCDId;
}

MDRCDQuery::~MDRCDQuery()
{
}

bool MDRCDQuery::execute()
{
  string sHeader = "User-Agent: Windows-Media-Player/12.0.7601.18741";
  string sURL = "http://toc.music.metaservices.microsoft.com/cdinfo/GetMDRCD.aspx?locale=409&geoid=f4&version=12.0.10586.162&userlocale=409&CD="+CDId_get();
  CURLcode res;
  struct curl_slist *headerList= NULL;
  
  curl = curl_easy_init();

  if (!curl)
    return false;

  curl_slist_append(headerList, sHeader.c_str());
  
  if (!curl)
    {
      return false;
    }

  curl_easy_setopt(curl, CURLOPT_URL, sURL.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_PORT, 80);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &MDRCDQuery::curlWriter);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);
  res = curl_easy_perform(curl);

  return true;
}

// int main(int argc, char** argv)
// {
//   MDRCDQuery* q = new MDRCDQuery("E+96+3EB4+7401+A771+DA48+11596+14930+1804C+1BABF+20CD8+240EF+27B8A+2B160+2EB4C+326EE");
  
//   if (!q->execute())
//     {
//       cout << "Something Failed!";
//       return 1;
//     }

//   cout << "Data returned is: " << q->Reply_get() << endl;

//   return 0;
  
// }

