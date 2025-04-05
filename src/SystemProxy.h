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

#include <memory>
#include <string>
#include <vector>

class SystemProxy
{
public:
    class Builder;

    // 公共枚举定义，方便外部使用
    enum Mode : uint8_t
    {
        NoProxy = 0,
        Auto = 1 << 0,
        Pac = 1 << 1,
        Manual = 1 << 2
    };

    // 枚举位运算操作符
    friend inline Mode operator|(Mode a, Mode b)
    {
        return static_cast<Mode>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }
    friend inline Mode& operator|=(Mode& a, Mode b) { return a = a | b; }
    friend inline bool operator&(Mode a, Mode b) { return static_cast<uint8_t>(a) & static_cast<uint8_t>(b); }

    struct Config
    {
        Mode mode{NoProxy};  // 代理模式
        std::string pac_url;  // PAC 脚本 URL
        std::string host;  // 代理服务器地址
        std::string port;  // 代理服务器端口
        std::vector<std::string> exceptions;  // 例外列表（不使用代理的地址）
    };

    // 禁用复制构造与复制赋值
    SystemProxy(const SystemProxy&) = delete;
    SystemProxy& operator=(const SystemProxy&) = delete;
    ~SystemProxy() = default;

    /**
     * @brief 将当前配置导出为JSON字符串
     * @return JSON字符串
     */
    std::string ToJson() const { return ""; }

    /**
     * @brief 从JSON字符串导入配置
     * @param jsonStr JSON字符串
     * @return 是否成功导入
     */
    bool FromJson(const std::string& jsonStr) { return false; }

    /**
     * @brief 应用当前配置到系统
     * @return 是否成功应用
     */
    bool ApplyConfig();

    /**
     * @brief 创建一个新的Builder对象来修改现有配置
     * @return 指向新Builder的唯一指针
     */
    std::unique_ptr<Builder> GetBuilder();

private:
    std::unique_ptr<Config> m_config;

    // 私有构造函数，只允许Builder构建
    explicit SystemProxy(std::unique_ptr<Config> config) : m_config(std::move(config)) {}

    friend class Builder;
};

class SystemProxy::Builder
{
public:
    // 默认构造函数，创建新的配置
    Builder() : m_config(std::make_unique<Config>()) {}

    // 从现有SystemProxy创建构建器的构造函数
    explicit Builder(const SystemProxy* parent)
    {
        if (parent && parent->m_config)
        {
            m_config = std::make_unique<Config>(*parent->m_config);
        }
        else
        {
            m_config = std::make_unique<Config>();
        }
    }

    Builder(const Builder&) = delete;
    Builder& operator=(const Builder&) = delete;
    ~Builder() = default;

    Builder& SetMode(Mode mode)
    {
        m_config->mode = mode;
        return *this;
    }

    Builder& SetPacUrl(std::string url)
    {
        m_config->pac_url = std::move(url);
        return *this;
    }

    Builder& SetProxy(std::string host, std::string port)
    {
        m_config->host = std::move(host);
        m_config->port = std::move(port);
        return *this;
    }

    Builder& SetExceptions(std::vector<std::string> exceptions)
    {
        m_config->exceptions = std::move(exceptions);
        return *this;
    }

    Builder& AddException(std::string exception)
    {
        m_config->exceptions.emplace_back(std::move(exception));
        return *this;
    }

    Builder& RemoveException(const std::string& exception)
    {
        auto& exceptions = m_config->exceptions;
        auto iter = std::find(exceptions.begin(), exceptions.end(), exception);
        if (iter != exceptions.end())
            exceptions.erase(iter);
        return *this;
    }

    std::unique_ptr<SystemProxy> Build() { return std::unique_ptr<SystemProxy>(new SystemProxy(std::move(m_config))); }

private:
    std::unique_ptr<Config> m_config;
};

// 定义在类外部的成员函数实现
inline std::unique_ptr<SystemProxy::Builder> SystemProxy::GetBuilder()
{
    return std::make_unique<Builder>(this);
}
