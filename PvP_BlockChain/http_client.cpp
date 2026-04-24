#include "http_client.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#endif

#include <sstream>
#include <thread>

namespace {

#ifdef _WIN32
void winHttpRequest(
    const std::wstring& method,
    const std::string& url,
    const std::string* jsonBody,
    std::function<void(bool, std::string)> callback
) {
    int wideLength = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, nullptr, 0);
    if (wideLength <= 0) {
        callback(false, "Failed to decode URL");
        return;
    }

    std::wstring wideUrl(wideLength - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, wideUrl.data(), wideLength);

    URL_COMPONENTS components{};
    components.dwStructSize = sizeof(components);
    components.dwSchemeLength = static_cast<DWORD>(-1);
    components.dwHostNameLength = static_cast<DWORD>(-1);
    components.dwUrlPathLength = static_cast<DWORD>(-1);
    components.dwExtraInfoLength = static_cast<DWORD>(-1);

    if (!WinHttpCrackUrl(wideUrl.c_str(), 0, 0, &components)) {
        callback(false, "Invalid URL");
        return;
    }

    std::wstring host(components.lpszHostName, components.dwHostNameLength);
    std::wstring path(components.lpszUrlPath, components.dwUrlPathLength);
    if (components.dwExtraInfoLength > 0) {
        path += std::wstring(components.lpszExtraInfo, components.dwExtraInfoLength);
    }

    const bool secure = components.nScheme == INTERNET_SCHEME_HTTPS;
    HINTERNET session = WinHttpOpen(L"GladiatorArena/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        callback(false, "WinHTTP session init failed");
        return;
    }

    HINTERNET connect = WinHttpConnect(session, host.c_str(), components.nPort, 0);
    if (!connect) {
        WinHttpCloseHandle(session);
        callback(false, "WinHTTP connect failed");
        return;
    }

    HINTERNET request = WinHttpOpenRequest(connect, method.c_str(), path.c_str(), nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, secure ? WINHTTP_FLAG_SECURE : 0);
    if (!request) {
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        callback(false, "WinHTTP request creation failed");
        return;
    }

    WinHttpSetTimeouts(request, 5000, 5000, 20000, 20000);

    BOOL sent = FALSE;
    if (jsonBody) {
        const wchar_t* headers = L"Content-Type: application/json\r\n";
        sent = WinHttpSendRequest(
            request,
            headers,
            static_cast<DWORD>(-1L),
            (LPVOID)jsonBody->data(),
            static_cast<DWORD>(jsonBody->size()),
            static_cast<DWORD>(jsonBody->size()),
            0
        );
    } else {
        sent = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    }

    if (!sent || !WinHttpReceiveResponse(request, nullptr)) {
        const DWORD error = GetLastError();
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        callback(false, "WinHTTP request failed: " + std::to_string(error));
        return;
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX);

    std::string response;
    DWORD available = 0;
    do {
        available = 0;
        if (!WinHttpQueryDataAvailable(request, &available)) {
            break;
        }
        if (available == 0) {
            break;
        }

        std::string buffer(available, '\0');
        DWORD downloaded = 0;
        if (!WinHttpReadData(request, buffer.data(), available, &downloaded)) {
            break;
        }
        buffer.resize(downloaded);
        response += buffer;
    } while (available > 0);

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);

    if (statusCode >= 200 && statusCode < 300) {
        callback(true, response);
    } else {
        std::ostringstream oss;
        oss << "HTTP " << statusCode << ": " << response;
        callback(false, oss.str());
    }
}
#endif

}  // namespace

void getUrlAsync(const std::string& url, std::function<void(bool, std::string)> callback) {
    std::thread([url, callback]() {
#ifdef _WIN32
        winHttpRequest(L"GET", url, nullptr, std::move(callback));
#else
        callback(false, "HTTP client is only implemented for Windows builds");
#endif
    }).detach();
}

void postJsonAsync(const std::string& url, const std::string& jsonBody, std::function<void(bool, std::string)> callback) {
    std::thread([url, jsonBody, callback]() {
#ifdef _WIN32
        winHttpRequest(L"POST", url, &jsonBody, std::move(callback));
#else
        callback(false, "HTTP client is only implemented for Windows builds");
#endif
    }).detach();
}
