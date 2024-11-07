#include "SystemProxyConfig.h"

int main()
{
    SystemProxyConfig proxy;

    // query proxy
    proxy.getServerAddress();
    proxy.getBypass();
    proxy.proxyEnabled();

    // setting proxy
    proxy.setServerAddress("127.0.0.1:1080");
    proxy.setBypass("localhost;127.*;192.168.*");
    proxy.setProxyEnabled(true);
    proxy.apply();
    return 0;
}
