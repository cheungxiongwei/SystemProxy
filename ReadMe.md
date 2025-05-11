# SystemProxy

`SystemProxy` is a Windows system proxy configuration utility that allows you to set system proxy settings
programmatically. It can be used in tools like v2ray, trojan-go, and other similar tools that require setting the
`System IE proxy`.

### Usage

```c++
#include <print>
#include <string>
#include "SystemProxy.h"

int main()
{
    auto proxy = SystemProxy::Builder().SetMode(SystemProxy::Pac).SetPacUrl("http://example.com/proxy.pac").Build();
    proxy->ApplyConfig();

    // 基于现有配置创建新配置
    auto newBuilder = proxy->GetBuilder();
    auto modifiedProxy = newBuilder->SetMode(SystemProxy::Manual).SetProxy("localhost", "8080").Build();

    return 0;
}
```

```macos
# ss_on.sh
/usr/sbin/networksetup -setwebproxystate Wi-Fi on;
/usr/sbin/networksetup -setsecurewebproxystate Wi-Fi on;
/usr/sbin/networksetup -setsocksfirewallproxystate Wi-Fi on;

/usr/sbin/networksetup -setwebproxy Wi-Fi 192.168.3.105 1080;
/usr/sbin/networksetup -setsecurewebproxy Wi-Fi 192.168.3.105 1080;
/usr/sbin/networksetup -setsocksfirewallproxy Wi-Fi 192.168.3.105 1080;

# ss_off.sh
/usr/sbin/networksetup -setwebproxystate Wi-Fi off;
/usr/sbin/networksetup -setsecurewebproxystate Wi-Fi off;
/usr/sbin/networksetup -setsocksfirewallproxystate Wi-Fi off;
```

```json

```