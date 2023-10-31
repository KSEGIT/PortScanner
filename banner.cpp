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

// TODO more detailed ssl information
// Callback function to capture SSL/TLS version information
/* size_t SSLEngineInfoCallback(void* ptr, size_t size, size_t nmemb, void* data) {
    const struct curl_tlssessioninfo* info = static_cast<const struct curl_tlssessioninfo*>(ptr);
    if (info && info->backend == CURLSSLBACKEND_OPENSSL) {
        SSL* ssl = static_cast<SSL*>(info->internals);
        const char* version = SSL_get_version(ssl);
        std::cout << "SSL/TLS Version: " << version << std::endl;
    }
    return size * nmemb;
} */

void getBanner(const char * ipAddress) {
    // Initialize libcurl
    CURL* curl = curl_easy_init();

    if (curl) {
        try {
            // Strings to store the received data
            std::string headerData;
            std::string bodyData;

            // Set the address to be fetched
            curl_easy_setopt(curl, CURLOPT_URL, ipAddress);

            // Set a 10-second timeout
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);  

            // Turn on Follow redirects
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            if (g_verbose){
            // Enable VERBOSE mode to see detailed SSL/TLS info
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            }
            
            // Headers be written to this file handle
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteHeaderCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerData);

            // Set the callback function to process received data
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bodyData);

            // Set True for getting certificate information
            curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);

            // Perform HTTP GET request 
            CURLcode res = curl_easy_perform(curl);
            
            // When request succesfull print information
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

                    /* // TODO more detailed ssl information
                    // Set the callback function to capture SSL/TLS version
                    curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L);
                    curl_easy_setopt(curl, CURLOPT_SSL_CTX_DATA, curl);
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, SSLEngineInfoCallback); 
                    */

                    // Get certificate information
                    std::cout << "Getting TLS/SSL information...\n";
                    
                    // Set the CERTINFO option to get certificate details
                    struct curl_certinfo *certinfo;
                    curl_easy_getinfo(curl, CURLINFO_CERTINFO, &certinfo);

                    // Check if there is certificate information available
                    if (certinfo) {
                        cout << "Number of certificates: " << certinfo->num_of_certs << "\n";

                        for (int i = 0; i < certinfo->num_of_certs; i++) {
                            cout << "Certificate #" << i + 1 << " details:" << endl;

                            struct curl_slist* slist = certinfo->certinfo[i];
                            while (slist) {
                                // Print each certificate detail in the list
                                cout << slist->data << endl;
                                slist = slist->next;
                            }
                        }
                    } else {
                        cout << "No certificate information available." << endl;
                    }
                } else {
                    std::cout << "Unable to connect to host\n";
                }
            }
            // Cleanup and release resources
            curl_easy_cleanup(curl);
        } 
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        } 
        catch (std::logic_error& e){
            std::cerr << "Error: " << e.what() << std::endl;
        } 
        catch (...) {
            std::current_exception();
        }
    } else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }

}
