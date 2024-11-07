#pragma once
// 版权所有 (C) [2024] [cheungxiongwei]
//
// 本软件根据Apache许可证，版本2.0（“许可证”）进行许可；
// 除非符合许可证，否则不得使用此文件。
// 您可以在以下网址获取许可证副本：
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// 除非适用法律要求或书面同意，按照许可证分发的软件
// 将按“原样”提供，不含任何明示或暗示的担保或条件。
// 请参阅许可证以了解有关许可权限和限制的具体语言。
//
// Copyright (C) [2024] [cheungxiongwei]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

class ISystemProxyConfig
{
public:
    virtual ~ISystemProxyConfig() = default;
    virtual const char *getServerAddress() = 0;
    virtual void setServerAddress(const char *server /*127.0.0.1:1080*/) = 0;
    virtual const char *getBypass() = 0;
    virtual void setBypass(const char *bypass) = 0;
    virtual void setProxyEnabled(bool enable) = 0;
    virtual bool proxyEnabled() = 0;
    virtual void apply() = 0;
};
