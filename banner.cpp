// Based on https://curl.se/libcurl/c/sepheaders.html
// For more information see: https://curl.se/libcurl/c/libcurl-easy.html

#include "banner.hpp"

// Callback function to write received body data into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Callback function to capture full response headers into a string
size_t WriteHeaderCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

void getBanner(const char * ipAddress) {
    if (curl) {

        // Strings to store the received data
        std::string headerData;
        std::string bodyData;

        // Set the address to be fetched
        curl_easy_setopt(curl, CURLOPT_URL, ipAddress);

        // Limitin response only to headers
        //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");

        // Turn on Follow redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Set a function to suppress the body of the response
        //curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

        // Set the CERTINFO option to get certificate details
        curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);

        if (verbose){
        // Enable VERBOSE mode to see detailed SSL/TLS info
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);}
        
        // Set the callback function to process received data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bodyData);
        
        // Headers be written to this file handle
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteHeaderCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerData);

        // Configure libcurl to verify ssl connection
        //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Perform the HTTP GET request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Print the curl responses
            std::cout << "\033[1;34m===== Host Response =====\033[0m\n";
            std::cout << "Header response:\n" << headerData << std::endl;
            //std::cout << "Body response:\n" << bodyData << std::endl;

            std::cout << "\033[1;34m===== TLS/SSL =====\033[0m\n";
            // Check if TLS/SSL is used
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            std::cout << "HTTP Status Code: " << http_code << std::endl;
            if (http_code == 200) {
                std::cout << "TLS/SSL is used.\n";
                const char* tls_version;
                curl_easy_getinfo(curl, CURLINFO_TLS_SSL_PTR, &tls_version);
                std::cout << "TLS/SSL Version: " << tls_version << "\n";
                // Extract certificate details
                struct curl_certinfo *certinfo;
                curl_easy_getinfo(curl, CURLINFO_CERTINFO, &certinfo);
                cout << "TLS/SSL Details: " << certinfo << "\n\n";
                // Iterate through certificate chain and print details
                for (int i = 0; i < certinfo->num_of_certs; i++) {
                    cout << "Certificate #" << i + 1 << " details:" << endl;
                    for (struct curl_slist* slist = certinfo->certinfo[i]; slist; slist = slist->next) {
                        cout << slist->data << endl;
                    }
                }

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
