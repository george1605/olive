#ifndef __OLCURL_H__
#define __OLCURL_H__
#include <curl/curl.h>
#include <string.h>

typedef struct {
    char* ptr;
    size_t len;
} OlChunk;

CURLcode ol_httpget(CURL* curl, const char* link)
{
    curl_easy_setopt(curl, CURLOPT_URL, link);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    return curl_easy_perform(curl);
}

CURLcode ol_getmail(CURL* curl, char* server, char* name, char* pass)
{
    curl_easy_setopt(curl, CURLOPT_USERNAME, name);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, pass);
 
    curl_easy_setopt(curl, CURLOPT_URL, server);
    return curl_easy_perform(curl);
}

static int ol_writeit(void *ptr, size_t size, size_t nmemb, OlChunk *s)
{
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size * nmemb;
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

static size_t ol_write_file(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// download file from http address
void ol_curl_download(const char* url, const char* fname)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "Failed to initialize cURL.\n");
        return;
    }
    FILE* fp = fopen(fname, "wb");
    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ol_write_file);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "cURL failed: %s\n", curl_easy_strerror(res));
    }
    fclose(fp);
    curl_easy_cleanup(curl);
}

void ol_initchunk(OlChunk *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

OlChunk ol_curl_data(CURL* curl) {
    OlChunk p;
    ol_initchunk(&p);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ol_writeit);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &p);
    return p;
}

long long ol_download_size(CURL* curl)
{
    long long k;
    curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &k);
    return k; 
}

#endif