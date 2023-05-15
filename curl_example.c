#include <stdio.h>
#include <curl/curl.h>
#include <time.h>

int main(void)
{
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    char url[] = "http://localhost:8000/distance_data"; 

    double distance = 10.5; // replace with the appropriate distance value

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the POST headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        time_t timestamp = time(NULL); // Get now timestamp to send the server
        // THIS how to get data inside string using sprintf
        // #####################
        char post_data[100];
        sprintf(post_data, "{\"distance\": %.2f, \"timestamp\": %ld}", distance, timestamp);
        // #####################
        /*
        In case of wanting to send the ph data use this line instead
        sprintf(post_data, "{\"ph\": %.2f, \"timestamp\": %ld}", phValue, timestamp);
        */

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

        // Set the URL and headers
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl);

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
    }

    return 0;
}