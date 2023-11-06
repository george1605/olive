#include "olcurl.h"

int main()
{
    ol_curl_download("http://example.com", "index.html");
    return 0;   
}