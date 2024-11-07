#include "SystemProxyConfig.h"
#include <string>
struct SystemProxyConfigContext
{
    // ui str
    std::string proxy_server;
    std::string proxy_bypass;
    std::string autoconfig_url;

    // ui flag
    bool direct = false; // no set
    bool proxy = false;
    bool auto_proxy_url = false;
    bool auto_detect = false;
};

static SystemProxyConfigContext s_context;
static constexpr auto DEFAULT_PROXY_ADDRESS = "127.0.0.1";
static constexpr int DEFAULT_PROXY_PORT = 1080;
static constexpr auto DEFAULT_PROXY_BYPASS = "localhost;127.0.0.*;192.168.*;";

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

[[maybe_unused]] static std::string query_http_version()
{
    INTERNET_VERSION_INFO _version = {0};
    DWORD dwBufSize = sizeof(INTERNET_VERSION_INFO);
    InternetQueryOptionA(nullptr, INTERNET_OPTION_VERSION, &_version, &dwBufSize);
    return std::to_string(_version.dwMajorVersion) + "." + std::to_string(_version.dwMinorVersion);
}

static bool query_proxy_option(SystemProxyConfigContext &context)
{
    // Normally, querying 4 flag bits information is enough.
    INTERNET_PER_CONN_OPTIONA _option[10];
    _option[0].dwOption = INTERNET_PER_CONN_FLAGS; // value: Use proxy server; Proxy UI interface state combination
    _option[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER; // str: Proxy address and port
    _option[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS; // str: Proxy bypass addresses
    _option[3].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL; // str: Automatic Config URL (Script Address)
    _option[4].dwOption = INTERNET_PER_CONN_AUTODISCOVERY_FLAGS;
    _option[5].dwOption = INTERNET_PER_CONN_AUTOCONFIG_SECONDARY_URL;
    _option[6].dwOption = INTERNET_PER_CONN_AUTOCONFIG_RELOAD_DELAY_MINS;
    _option[7].dwOption = INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_TIME;
    _option[8].dwOption = INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_URL;
    _option[9].dwOption = INTERNET_PER_CONN_FLAGS_UI;

    INTERNET_PER_CONN_OPTION_LISTA _list;
    DWORD dwBufSize = sizeof(INTERNET_PER_CONN_OPTION_LISTA);

    _list.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
    _list.pszConnection = nullptr;
    _list.dwOptionCount = 10;
    _list.dwOptionError = 0;
    _list.pOptions = _option;

    bool ret;
    if (ret = InternetQueryOptionA(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &_list, &dwBufSize); ret)
    {
        auto get_str = [](LPSTR str, std::string &out)
        {
            out = str ? std::string(str) : std::string();
            if (str)
            {
                GlobalFree(str);
            }
        };
        get_str(_option[1].Value.pszValue, context.proxy_server);
        get_str(_option[2].Value.pszValue, context.proxy_bypass);
        get_str(_option[3].Value.pszValue, context.autoconfig_url);
    }

    context.direct = _option[9].Value.dwValue & PROXY_TYPE_DIRECT ? true : false;
    context.proxy = _option[9].Value.dwValue & PROXY_TYPE_PROXY ? true : false;
    context.auto_proxy_url = _option[9].Value.dwValue & PROXY_TYPE_AUTO_PROXY_URL ? true : false;
    context.auto_detect = _option[9].Value.dwValue & PROXY_TYPE_AUTO_DETECT ? true : false;

    return ret;
}

static bool set_proxy_option(SystemProxyConfigContext &context)
{
    INTERNET_PER_CONN_OPTIONA _option[4];
    _option[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER; // str Proxy address and port
    _option[1].dwOption = INTERNET_PER_CONN_PROXY_BYPASS; // str Proxy bypass addresses
    _option[2].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL; // str Script address
    _option[3].dwOption = INTERNET_PER_CONN_FLAGS_UI; // Win7 and later, value indicating proxy server usage and proxy
                                                      // UI interface state combination

    _option[0].Value.pszValue = context.proxy_server.data();
    _option[1].Value.pszValue = context.proxy_bypass.data();
    _option[2].Value.pszValue = context.autoconfig_url.data();

    _option[3].Value.dwValue = PROXY_TYPE_DIRECT;
    if (context.proxy)
    {
        _option[3].Value.dwValue |= PROXY_TYPE_PROXY;
    }
    if (context.auto_proxy_url)
    {
        _option[3].Value.dwValue |= PROXY_TYPE_AUTO_PROXY_URL;
    }
    if (context.auto_detect)
    {
        _option[3].Value.dwValue |= PROXY_TYPE_AUTO_DETECT;
    }

    INTERNET_PER_CONN_OPTION_LISTA _list;
    DWORD dwBufSize = sizeof(INTERNET_PER_CONN_OPTION_LISTA);

    _list.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
    _list.pszConnection = nullptr;
    _list.dwOptionCount = 4;
    _list.dwOptionError = 0;
    _list.pOptions = _option;

    if (!InternetSetOptionA(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &_list, dwBufSize))
    {
        if (GetLastError() == ERROR_INVALID_PARAMETER)
        {
            fprintf_s(stderr, "%s\r\n", "ERROR_INVALID_PARAMETER");
        }
        return false;
    }

    InternetSetOptionA(nullptr, INTERNET_OPTION_PROXY_SETTINGS_CHANGED, NULL, NULL);
    InternetSetOptionA(nullptr, INTERNET_OPTION_REFRESH, NULL, NULL);
    return true;
}

// win10/win11 proxy ui mapping
#if _HAS_CXX23
#include <print>
void SystemProxyConfigPrint(SystemProxyConfigContext &context)
{
    // ui str
    std::println("proxy_server: {}", context.proxy_server);
    std::println("proxy_bypass: {}", context.proxy_bypass);
    std::println("autoconfig_url: {}", context.autoconfig_url);

    // ui flag
    std::println("direct: {}", context.direct);
    std::println("proxy: {}", context.proxy);
    std::println("auto_proxy_url: {}", context.auto_proxy_url);
    std::println("auto_detect: {}", context.auto_detect);
}
#endif

SystemProxyConfig::SystemProxyConfig() { query_proxy_option(s_context); }

SystemProxyConfig::~SystemProxyConfig() = default;

const char *SystemProxyConfig::getServerAddress() { return s_context.proxy_server.data(); }

void SystemProxyConfig::setServerAddress(const char *address) { s_context.proxy_server = address; }

const char *SystemProxyConfig::getBypass() { return s_context.proxy_bypass.data(); }

void SystemProxyConfig::setBypass(const char *bypass) { s_context.proxy_bypass = bypass; }

void SystemProxyConfig::setProxyEnabled(bool enable)
{
    s_context.proxy = enable;
    s_context.auto_proxy_url = false;
    s_context.auto_detect = false;
}

bool SystemProxyConfig::proxyEnabled() { return s_context.proxy; }

void SystemProxyConfig::apply() { set_proxy_option(s_context); }

#elif defined(__APPLE__)
#warning “Running on macOS OS.”
#elif defined(__linux__)
#warning “Running on Linux OS.”
#else
#warning “Running on unknown OS.”
#endif