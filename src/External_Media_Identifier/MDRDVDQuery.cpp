/**
 * MDRDVDQuery - Query Microsoft Windows Media Server services for a DVD 
 * given a CRC64 calculated by dvdid
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "MDRDVDQuery.h"
#include <iostream>

MDRDVDQuery::MDRDVDQuery(string sDVDId)
{
  m_sDVDId=sDVDId;
}

MDRDVDQuery::~MDRDVDQuery()
{
}

bool MDRDVDQuery::execute()
{
  string sHeader = "User-Agent: Windows-Media-Player/12.0.7601.18741";
  string sURL = "http://metaservices.windowsmedia.com/pas_dvd_B/template/GetMDRDVDByCRC.xml?CRC="+DVDId_get();
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
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &MDRDVDQuery::curlWriter);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);
  res = curl_easy_perform(curl);

  return true;
}

// int main(int argc, char** argv)
// {
//   MDRDVDQuery* q = new MDRDVDQuery("3B509BE3243449DA");
  
//   if (!q->execute())
//     {
//       cout << "Something Failed!";
//       return 1;
//     }

//   cout << "Data returned is: " << q->Reply_get() << endl;

//   return 0;
  
// }

