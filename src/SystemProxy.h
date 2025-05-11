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
#include <utility>
#include <vector>

class SystemProxy
{
public:
    enum Mode
    {
        NoProxy = 0,
        Auto = 1 << 0,
        Pac = 1 << 1,
        Manual = 1 << 2
    };
    friend inline Mode operator|(Mode a, Mode b)
    {
        return static_cast<Mode>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }
    friend inline Mode& operator|=(Mode& a, Mode b) { return a = a | b; }
    friend inline bool operator&(Mode a, Mode b) { return static_cast<uint8_t>(a) & static_cast<uint8_t>(b); }

    struct Config
    {
        Mode mode{NoProxy};  // 代理模式
        std::string pacUrl;  // PAC 脚本 URL
        std::string host{"127.0.0.1"};  // 代理服务器地址
        std::string port{"1080"};  // 代理服务器端口
        std::vector<std::string> exceptions;  // 例外列表（不使用代理的地址）
    };
    class Builder;

    bool apply();

    std::unique_ptr<Builder> getBuilder();
private:
    Config config_;

    explicit SystemProxy() = default;
    explicit SystemProxy(Config config):config_(std::move(config)){}

    static std::unique_ptr<SystemProxy> create() noexcept {
        return std::unique_ptr<SystemProxy>(new(std::nothrow) SystemProxy());
    }

    static std::unique_ptr<SystemProxy> create(const Config& config) noexcept {
        return std::unique_ptr<SystemProxy>(new(std::nothrow) SystemProxy(config));
    }
};

class SystemProxy::Builder
{
public:
    explicit Builder() noexcept : context_(create()) {}
    explicit Builder(const Config& config) noexcept : context_(create(config)) {}

    Builder& setMode(Mode mode)
    {
        context_->config_.mode = mode;
        return *this;
    }

    Builder& setPacUrl(std::string url)
    {
        context_->config_.pacUrl = std::move(url);
        return *this;
    }

    Builder& setProxy(std::string host, std::string port)
    {
        context_->config_.host = std::move(host);
        context_->config_.port = std::move(port);
        return *this;
    }

    Builder& setExceptions(std::vector<std::string> exceptions)
    {
        context_->config_.exceptions = std::move(exceptions);
        return *this;
    }

    Builder& addException(std::string exception)
    {
        context_->config_.exceptions.emplace_back(std::move(exception));
        return *this;
    }

    Builder& removeException(const std::string& exception)
    {
        auto& exceptions = context_->config_.exceptions;
        auto iter = std::find(exceptions.begin(), exceptions.end(), exception);
        if (iter != exceptions.end())
            exceptions.erase(iter);
        return *this;
    }

    std::unique_ptr<SystemProxy> build() { return std::move(context_); }
private:
    std::unique_ptr<SystemProxy> context_;
};

inline std::unique_ptr<SystemProxy::Builder> SystemProxy::getBuilder()
{
    return std::make_unique<Builder>(this->config_);
}
