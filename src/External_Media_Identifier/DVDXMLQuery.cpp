/**
 * DVDXMLQuery - a class which attempts to retrieve metadata for a DVD given a DVDID.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "DVDXMLQuery.h"

#include <iostream>

DVDXMLQuery::DVDXMLQuery(string sDVDId)
{
  m_sDVDId=sDVDId;
}

DVDXMLQuery::~DVDXMLQuery()
{
}

bool DVDXMLQuery::execute()
{
  string sRequest = "<dvdXml><authentication><user>" DVDXML_API_USER "</user><password>" DVDXML_API_PASSWORD "</password></authentication><requests><request type='information'><dvdId>"+m_sDVDId+"</dvdId></request></requests></dvdXml>";
  string sHeader = "Content-Type: text/xml";
  string sURL = "http://api.dvdxml.com/";
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
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sRequest.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1);
  curl_easy_setopt(curl, CURLOPT_PORT, 80);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &DVDXMLQuery::curlWriter);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);
  res = curl_easy_perform(curl);

  return true;
}

// int main(int argc, char** argv)
// {
//   DVDXMLQuery* q = new DVDXMLQuery("3B509BE3243449DA");
  
//   if (!q->execute())
//     {
//       cout << "Something Failed!";
//       return 1;
//     }

//   cout << "Data returned is: " << q->Reply_get() << endl;

//   return 0;
  
// }
