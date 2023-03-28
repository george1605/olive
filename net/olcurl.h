#ifndef __OLCURL_H__
#define __OLCURL_H__
#include <curl/curl.h>

CURLcode ol_httpget(CURL* curl, const char* link)
{
    curl_easy_setopt(curl, CURLOPT_URL, link);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    return curl_easy_perform(curl);
}

CURLcode ol_getmail(CURL* curl, char* server, char* name, char* pass)
{
    curl_easy_setopt(curl, CURLOPT_USERNAME, "user");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "secret");
 
    curl_easy_setopt(curl, CURLOPT_URL, "pop3://pop.example.com/1");
    return curl_easy_perform(curl);
}

CURL* ol_curlinit(void(*error)())
{
    CURL* p = curl_easy_init();
    if(!p)
        if(error == NULL)
            puts("CURL Failed.\n");
        else
            error();
    return p;
}

void ol_curl_cookies(CURL* curl, struct curl_slist *cookies)
{
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
}

long long ol_download_size(CURL* curl)
{
    long long k;
    curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &k);
    return k; 
}

#endif