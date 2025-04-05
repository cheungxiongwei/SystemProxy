#include "SystemProxy.h"
#include <format>
#include <print>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

static bool windows_set_proxy_config(SystemProxy::Config* config)
{
    INTERNET_PER_CONN_OPTIONA option[4];
    option[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
    option[1].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
    option[2].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
    option[3].dwOption = INTERNET_PER_CONN_FLAGS_UI;  // Win7 and later, value indicating proxy server usage and proxy

    // UI interface state combination
    option[3].Value.dwValue = PROXY_TYPE_DIRECT;
    if (config->mode & SystemProxy::Manual)
    {
        static std::string proxy_server = std::format("{}:{}", config->host, config->port);
        static std::string exceptions_list;
        exceptions_list.clear();
        for (auto& v : config->exceptions)
        {
            exceptions_list += v + ";";
        }
        option[0].Value.pszValue = proxy_server.data();
        option[1].Value.pszValue = exceptions_list.data();
        option[3].Value.dwValue |= PROXY_TYPE_PROXY;
    }
    if (config->mode & SystemProxy::Pac)
    {
        option[2].Value.pszValue = config->pac_url.data();
        option[3].Value.dwValue |= PROXY_TYPE_AUTO_PROXY_URL;
    }
    if (config->mode & SystemProxy::Auto)
    {
        option[3].Value.dwValue |= PROXY_TYPE_AUTO_DETECT;
    }

    INTERNET_PER_CONN_OPTION_LISTA list;
    DWORD dwBufSize = sizeof(INTERNET_PER_CONN_OPTION_LISTA);

    list.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
    list.pszConnection = nullptr;
    list.dwOptionCount = 4;
    list.dwOptionError = 0;
    list.pOptions = option;


    if (InternetSetOptionA(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize) &&
        InternetSetOptionA(nullptr, INTERNET_OPTION_PROXY_SETTINGS_CHANGED, NULL, NULL) &&
        InternetSetOptionA(nullptr, INTERNET_OPTION_REFRESH, NULL, NULL))
    {
        return true;
    }

    if (GetLastError() == ERROR_INVALID_PARAMETER)
    {
        std::println("ERROR_INVALID_PARAMETER");
    }
    return false;
}

[[maybe_unused]] static bool windows_get_proxy_config(SystemProxy::Config* config)
{
    // Initialize INTERNET_PER_CONN_OPTIONA array with various proxy configuration options.
    INTERNET_PER_CONN_OPTIONA option[10];
    // Indicates the use of proxy server and Proxy UI interface state.
    option[0].dwOption = INTERNET_PER_CONN_FLAGS;
    // Proxy address and port as a string.
    option[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
    // List of addresses to bypass proxy as a string.
    option[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
    // Automatic configuration script URL as a string.
    option[3].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
    // Auto-discovery flags.
    option[4].dwOption = INTERNET_PER_CONN_AUTODISCOVERY_FLAGS;
    // Secondary configuration script URL.
    option[5].dwOption = INTERNET_PER_CONN_AUTOCONFIG_SECONDARY_URL;
    // Delay for reloading auto-config script in minutes.
    option[6].dwOption = INTERNET_PER_CONN_AUTOCONFIG_RELOAD_DELAY_MINS;
    // Last auto-detection time.
    option[7].dwOption = INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_TIME;
    // Last auto-detection URL.
    option[8].dwOption = INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_URL;
    // Flags representing UI-related proxy configurations.
    option[9].dwOption = INTERNET_PER_CONN_FLAGS_UI;

    // Initialize INTERNET_PER_CONN_OPTION_LISTA structure with the above array and other parameters.
    INTERNET_PER_CONN_OPTION_LISTA list;
    DWORD dwBufSize = sizeof(INTERNET_PER_CONN_OPTION_LISTA);
    list.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
    list.pszConnection = nullptr;
    list.dwOptionCount = sizeof(option) / sizeof(INTERNET_PER_CONN_OPTIONA);
    list.dwOptionError = NULL;
    list.pOptions = option;

    if (InternetQueryOptionA(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, &dwBufSize))
    {
        auto get_str = [](LPSTR str, std::string& out)
        {
            out = str ? std::string(str) : std::string();
            if (str)
            {
                GlobalFree(str);
            }
        };

        std::string proxy_server;
        std::string exceptions_list;
        std::string pac_url;

        get_str(option[1].Value.pszValue, proxy_server);
        get_str(option[2].Value.pszValue, exceptions_list);
        get_str(option[3].Value.pszValue, pac_url);

        SystemProxy::Mode mode{SystemProxy::NoProxy};
        if (option[9].Value.dwValue & PROXY_TYPE_DIRECT)
            mode = SystemProxy::NoProxy;
        if (option[9].Value.dwValue & PROXY_TYPE_PROXY)
            mode |= SystemProxy::Manual;
        if (option[9].Value.dwValue & PROXY_TYPE_AUTO_PROXY_URL)
            mode |= SystemProxy::Pac;
        if (option[9].Value.dwValue & PROXY_TYPE_AUTO_DETECT)
            mode |= SystemProxy::Auto;

        // Log the retrieved proxy configuration details.
        std::println("Proxy Configuration:");
        std::println("  Proxy Server: {}", proxy_server.empty() ? "None" : proxy_server);
        std::println("  PAC URL: {}", pac_url.empty() ? "None" : pac_url);
        std::println("  Exceptions List: {}", exceptions_list.empty() ? "None" : exceptions_list);
        std::println("  Mode:");
        if (mode & SystemProxy::NoProxy)
            std::println("    No Proxy");
        if (mode & SystemProxy::Manual)
            std::println("    Manual");
        if (mode & SystemProxy::Pac)
            std::println("    PAC");
        if (mode & SystemProxy::Auto)
            std::println("    Auto");

        return true;
    }
    return false;
}

#elif defined(__linux__)

#elif defined(__APPLE__)

static bool macos_set_proxy_config(SystemProxy::Config* config)
{
    if (!config)
        return false;

    // Query the primary service name
    char buffer[128];
    std::string service_name;
    FILE* pipe = popen(
        "/usr/sbin/networksetup -listnetworkserviceorder | grep 'Hardware Port' | awk -F'[(|)]' '{print $2}'", "r");
    if (pipe)
    {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            service_name += buffer;
        }
        pclose(pipe);

        // Trim trailing newline
        if (!service_name.empty() && service_name.back() == '\n')
            service_name.pop_back();
    }

    if (service_name.empty())
        return false;

    std::string command;

    if (config->mode & SystemProxy::Manual)
    {
        command = std::format(
            "/usr/sbin/networksetup -setwebproxy \"{}\" {} {} && /usr/sbin/networksetup -setsecurewebproxy \"{}\" {} "
            "{}",
            service_name, config->host, config->port, service_name, config->host, config->port);

        if (!config->exceptions.empty())
        {
            std::string exceptions_list;
            for (const auto& exception : config->exceptions)
            {
                exceptions_list += exception + ",";
            }
            // Remove trailing comma
            if (!exceptions_list.empty())
                exceptions_list.pop_back();

            command += std::format(" && /usr/sbin/networksetup -setproxybypassdomains \"{}\" {}", service_name,
                                   exceptions_list);
        }
    }
    else if (config->mode & SystemProxy::Pac)
    {
        command = std::format("/usr/sbin/networksetup -setautoproxyurl \"{}\" \"{}\"", service_name, config->pac_url);
    }
    else if (config->mode & SystemProxy::Auto)
    {
        command = std::format("/usr/sbin/networksetup -setautoproxystate \"{}\" on", service_name);
    }
    else
    {
        command = std::format(
            "/usr/sbin/networksetup -setwebproxystate \"{}\" off && /usr/sbin/networksetup -setsecurewebproxystate "
            "\"{}\" off && /usr/sbin/networksetup -setautoproxystate \"{}\" off",
            service_name, service_name, service_name);
    }

    return (std::system(command.c_str()) == 0);
}

#endif

// windows,linux,macos,ios,android
bool SystemProxy::ApplyConfig()
{
    if (!m_config)
        return false;

#if defined(_WIN32) || defined(_WIN64)
    return windows_set_proxy_config(m_config.get());
#elif defined(__linux__)
    // Implement Linux-specific proxy configuration setting
    return linux_set_proxy_config(m_config.get());
#elif defined(__APPLE__)
    // Implement macOS-specific proxy configuration setting
    return macos_set_proxy_config(m_config.get());
#else
    std::runtime_error("Unsupported platform");
    return false;  // Unsupported platform
#endif
}
