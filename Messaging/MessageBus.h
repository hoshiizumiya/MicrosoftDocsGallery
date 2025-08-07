#pragma once
#include <winrt/Windows.Foundation.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeinfo>
#include <typeindex>

namespace winrt::MicrosoftDocsGallery::Messaging
{
    // 消息基类
    struct IMessage
    {
        virtual ~IMessage() = default;
    };

    // 具体消息类型
    struct NavigationMessage : public IMessage
    {
        winrt::hstring Destination;
        winrt::Windows::Foundation::IInspectable Parameter;

        NavigationMessage(winrt::hstring destination, winrt::Windows::Foundation::IInspectable parameter = nullptr)
            : Destination(destination), Parameter(parameter) {}
    };

    struct DataUpdatedMessage : public IMessage
    {
        winrt::hstring DataType;
        winrt::Windows::Foundation::IInspectable Data;

        DataUpdatedMessage(winrt::hstring dataType, winrt::Windows::Foundation::IInspectable data)
            : DataType(dataType), Data(data) {}
    };

    struct ErrorMessage : public IMessage
    {
        winrt::hstring Title;
        winrt::hstring Description;
        winrt::hstring Source;

        ErrorMessage(winrt::hstring title, winrt::hstring description, winrt::hstring source = L"")
            : Title(title), Description(description), Source(source) {}
    };

    struct FavoriteChangedMessage : public IMessage
    {
        winrt::hstring Url;
        winrt::hstring Title;
        bool IsAdded;

        FavoriteChangedMessage(winrt::hstring url, winrt::hstring title, bool isAdded)
            : Url(url), Title(title), IsAdded(isAdded) {}
    };

    // 消息处理器类型
    template<typename TMessage>
    using MessageHandler = std::function<void(std::shared_ptr<TMessage>)>;

    // 消息总线
    class MessageBus
    {
    public:
        static MessageBus& Instance()
        {
            static MessageBus instance;
            return instance;
        }

        // 订阅消息
        template<typename TMessage>
        void Subscribe(MessageHandler<TMessage> handler)
        {
            std::type_index typeIndex(typeid(TMessage));
            auto wrapper = [handler](std::shared_ptr<IMessage> message) {
                if (auto typedMessage = std::dynamic_pointer_cast<TMessage>(message))
                {
                    handler(typedMessage);
                }
            };
            
            m_handlers[typeIndex].push_back(wrapper);
        }

        // 发布消息
        template<typename TMessage>
        void Publish(std::shared_ptr<TMessage> message)
        {
            std::type_index typeIndex(typeid(TMessage));
            auto it = m_handlers.find(typeIndex);
            if (it != m_handlers.end())
            {
                for (auto& handler : it->second)
                {
                    try
                    {
                        handler(message);
                    }
                    catch (...)
                    {
                        // 记录错误但继续处理其他处理器
                    }
                }
            }
        }

        // 便利方法 - 直接创建并发布消息
        template<typename TMessage, typename... Args>
        void Publish(Args&&... args)
        {
            auto message = std::make_shared<TMessage>(std::forward<Args>(args)...);
            Publish(message);
        }

        // 清理所有订阅
        void Clear()
        {
            m_handlers.clear();
        }

    private:
        MessageBus() = default;
        
        using HandlerWrapper = std::function<void(std::shared_ptr<IMessage>)>;
        std::unordered_map<std::type_index, std::vector<HandlerWrapper>> m_handlers;
    };

    // RAII 消息订阅管理器
    class MessageSubscription
    {
    public:
        MessageSubscription() = default;
        
        template<typename TMessage>
        MessageSubscription(MessageHandler<TMessage> handler)
        {
            Subscribe(handler);
        }

        ~MessageSubscription()
        {
            // 在实际实现中，这里应该取消订阅
            // 为了简化示例，这里省略了具体实现
        }

        template<typename TMessage>
        void Subscribe(MessageHandler<TMessage> handler)
        {
            MessageBus::Instance().Subscribe<TMessage>(handler);
        }

    private:
        // 存储订阅信息以便在析构时取消订阅
        // 这里为了简化示例省略了具体实现
    };
}