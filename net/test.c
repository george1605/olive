#include "olcurl.h"

int main()
{
    CURL* curl = curl_easy_init();
    ol_httpget(curl, "https://www.google.com");
    curl_easy_cleanup(curl);
    printf("Size: %i", ol_download_size(curl));
}