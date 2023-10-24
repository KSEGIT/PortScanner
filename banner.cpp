#include "banner.h"

// Callback function to write received data into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

static void getBanner(const char * ipAddress) {
    if (curl) {
        // Specify the URL (IP address or domain name)
        const char* url = ipAddress;  // Replace with the server you want to inspect

        // Set the URL to be fetched
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Create a string to store the received data
        std::string response_data;

        // Set the callback function to process received data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // Configure libcurl to use OpenSSL
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Perform the HTTP GET request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Print the banner response
            std::cout << "Banner response:\n" << response_data << std::endl;

            // Check if TLS/SSL is used
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            if (http_code == 200) {
                std::cout << "TLS/SSL is used.\n";
                const char* tls_version;
                curl_easy_getinfo(curl, CURLINFO_TLS_SSL_PTR, &tls_version);
                std::cout << "TLS/SSL Version: " << tls_version << "\n";
            } else {
                std::cout << "TLS/SSL is not used for this connection.\n";
            }
        }

        // Cleanup and release resources
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }

};

