#include <print>
#include <string>
#include "SystemProxy.h"

int main()
{
    auto proxy = SystemProxy::Builder().setMode(SystemProxy::Pac).setPacUrl("http://example.com/proxy.pac").build();
    proxy->apply();

    // 基于现有配置创建新配置
    auto newBuilder = proxy->getBuilder();
    auto modifiedProxy = newBuilder->setMode(SystemProxy::Manual).setProxy("localhost", "8080").build();

    return 0;
}
