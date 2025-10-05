#include "http_client.h"
#include <thread>
#include <curl/curl.h>
#include <sstream>

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realSize = size * nmemb;
    std::string* mem = static_cast<std::string*>(userp);
    mem->append(static_cast<char*>(contents), realSize);
    return realSize;
}

void postJsonAsync(const std::string& url, const std::string& jsonBody, std::function<void(bool,std::string)> callback) {
    std::thread([url, jsonBody, callback]() {
        CURL* curl = curl_easy_init();
        if (!curl) {
            callback(false, "Failed to init CURL");
            return;
        }

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)jsonBody.size());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Optional: set timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (res != CURLE_OK) {
            std::string err = std::string("CURL error: ") + curl_easy_strerror(res);
            callback(false, err);
        } else if (http_code >= 200 && http_code < 300) {
            callback(true, response);
        } else {
            std::ostringstream oss;
            oss << "HTTP " << http_code << ": " << response;
            callback(false, oss.str());
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }).detach();
}
