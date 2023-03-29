#include "olcurl.h"

int main()
{
    CURL* curl = curl_easy_init();
    ol_httpget(curl, "https://www.google.com");
    OlChunk chunk = ol_curl_data(curl);
    printf("Data: %i", chunk.len);
    curl_easy_cleanup(curl);
}