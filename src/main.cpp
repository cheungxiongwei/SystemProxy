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
