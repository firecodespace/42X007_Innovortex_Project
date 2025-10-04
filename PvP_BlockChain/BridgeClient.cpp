#include "BridgeClient.h"
#include <windows.h>
#include <winhttp.h>
#include <sstream>

#pragma comment(lib, "winhttp.lib")

namespace BridgeClient {

    static std::wstring s2ws(const std::string& s) {
        if (s.empty()) return {};
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &s[0], (int)s.size(), NULL, 0);
        std::wstring ws(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &s[0], (int)s.size(), &ws[0], size_needed);
        return ws;
    }

    bool postWin(const std::string& winnerAddress, const std::string& loserAddress, std::string& outResponse) {
        bool result = false;
        outResponse.clear();

        std::string host = "localhost";
        std::string path = "/win";
        std::string payload = "{ \"winnerAddress\": \"" + winnerAddress + "\", \"loserAddress\": \"" + loserAddress + "\" }";

        HINTERNET hSession = WinHttpOpen(L"PvPBridgeClient/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);

        if (!hSession) return false;

        HINTERNET hConnect = WinHttpConnect(hSession, s2ws(host).c_str(), 3000, 0);
        if (!hConnect) { WinHttpCloseHandle(hSession); return false; }

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST",
            s2ws(path).c_str(), NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

        // Set headers (Content-Type) and include correct content length
        std::wstring headers = L"Content-Type: application/json";
        BOOL addOk = WinHttpAddRequestHeaders(hRequest, headers.c_str(), (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD);
        DWORD contentLength = (DWORD)payload.size();
        BOOL ok = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            (LPVOID)payload.c_str(), contentLength, contentLength, 0);

        if (!ok) {
            WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
            return false;
        }

        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
            return false;
        }

        // Read response
        DWORD dwSize = 0;
        do {
            DWORD dwDownloaded = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;
            std::vector<char> buffer(dwSize + 1);
            if (WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded) && dwDownloaded) {
                buffer[dwDownloaded] = '\0';
                outResponse.append(buffer.data(), dwDownloaded);
            }
        } while (dwSize > 0);

        // Clean up
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        result = true;
        return result;
    }

}
