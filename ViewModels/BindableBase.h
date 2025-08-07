#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <string>
#include <functional>
#include <unordered_map>

namespace winrt::MicrosoftDocsGallery::ViewModels
{
    // 属性变更通知基类
    // 这是 MVVM 模式中 ViewModel 的核心基类
    struct BindableBase : winrt::implements<BindableBase, winrt::Microsoft::UI::Xaml::Data::INotifyPropertyChanged>
    {
        // INotifyPropertyChanged 实现
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return m_propertyChanged.add(handler);
        }

        void PropertyChanged(winrt::event_token const& token) noexcept
        {
            m_propertyChanged.remove(token);
        }

    protected:
        // 触发属性变更通知
        void RaisePropertyChanged(winrt::hstring const& propertyName)
        {
            m_propertyChanged(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
        }

        // 设置属性值并触发通知（如果值发生变化）
        template<typename T>
        bool SetProperty(T& field, T const& newValue, winrt::hstring const& propertyName)
        {
            if (field != newValue)
            {
                field = newValue;
                RaisePropertyChanged(propertyName);
                return true;
            }
            return false;
        }

        // 带有自定义比较器的属性设置
        template<typename T, typename Comparer>
        bool SetProperty(T& field, T const& newValue, winrt::hstring const& propertyName, Comparer comparer)
        {
            if (!comparer(field, newValue))
            {
                field = newValue;
                RaisePropertyChanged(propertyName);
                return true;
            }
            return false;
        }

    private:
        winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };

    // 命令接口 - 用于绑定按钮点击等操作
    struct ICommand : winrt::implements<ICommand, winrt::Microsoft::UI::Xaml::Input::ICommand>
    {
        // ICommand 实现
        winrt::event_token CanExecuteChanged(winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable> const& handler)
        {
            return m_canExecuteChanged.add(handler);
        }

        void CanExecuteChanged(winrt::event_token const& token) noexcept
        {
            m_canExecuteChanged.remove(token);
        }

        bool CanExecute(winrt::Windows::Foundation::IInspectable const& parameter)
        {
            return CanExecuteImpl(parameter);
        }

        void Execute(winrt::Windows::Foundation::IInspectable const& parameter)
        {
            if (CanExecute(parameter))
            {
                ExecuteImpl(parameter);
            }
        }

        // 触发 CanExecute 状态变更
        void RaiseCanExecuteChanged()
        {
            m_canExecuteChanged(*this, nullptr);
        }

    protected:
        virtual bool CanExecuteImpl(winrt::Windows::Foundation::IInspectable const& parameter) = 0;
        virtual void ExecuteImpl(winrt::Windows::Foundation::IInspectable const& parameter) = 0;

    private:
        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>> m_canExecuteChanged;
    };

    // 简单命令实现
    class RelayCommand : public ICommand
    {
    public:
        using ExecuteFunc = std::function<void(winrt::Windows::Foundation::IInspectable const&)>;
        using CanExecuteFunc = std::function<bool(winrt::Windows::Foundation::IInspectable const&)>;

        RelayCommand(ExecuteFunc executeFunc, CanExecuteFunc canExecuteFunc = nullptr)
            : m_executeFunc(executeFunc), m_canExecuteFunc(canExecuteFunc)
        {
        }

    protected:
        bool CanExecuteImpl(winrt::Windows::Foundation::IInspectable const& parameter) override
        {
            return m_canExecuteFunc ? m_canExecuteFunc(parameter) : true;
        }

        void ExecuteImpl(winrt::Windows::Foundation::IInspectable const& parameter) override
        {
            if (m_executeFunc)
            {
                m_executeFunc(parameter);
            }
        }

    private:
        ExecuteFunc m_executeFunc;
        CanExecuteFunc m_canExecuteFunc;
    };

    // 异步命令实现
    class AsyncRelayCommand : public ICommand
    {
    public:
        using AsyncExecuteFunc = std::function<winrt::Windows::Foundation::IAsyncAction(winrt::Windows::Foundation::IInspectable const&)>;
        using CanExecuteFunc = std::function<bool(winrt::Windows::Foundation::IInspectable const&)>;

        AsyncRelayCommand(AsyncExecuteFunc executeFunc, CanExecuteFunc canExecuteFunc = nullptr)
            : m_executeFunc(executeFunc), m_canExecuteFunc(canExecuteFunc), m_isExecuting(false)
        {
        }

        bool IsExecuting() const { return m_isExecuting; }

    protected:
        bool CanExecuteImpl(winrt::Windows::Foundation::IInspectable const& parameter) override
        {
            if (m_isExecuting) return false;
            return m_canExecuteFunc ? m_canExecuteFunc(parameter) : true;
        }

        void ExecuteImpl(winrt::Windows::Foundation::IInspectable const& parameter) override
        {
            if (m_executeFunc && !m_isExecuting)
            {
                ExecuteAsync(parameter);
            }
        }

    private:
        AsyncExecuteFunc m_executeFunc;
        CanExecuteFunc m_canExecuteFunc;
        std::atomic<bool> m_isExecuting;

        winrt::fire_and_forget ExecuteAsync(winrt::Windows::Foundation::IInspectable const& parameter)
        {
            m_isExecuting = true;
            RaiseCanExecuteChanged();

            try
            {
                co_await m_executeFunc(parameter);
            }
            catch (...)
            {
                // 错误处理 - 在实际应用中应该有更好的错误处理机制
            }

            m_isExecuting = false;
            RaiseCanExecuteChanged();
        }
    };

    // 集合变更通知包装器
    template<typename T>
    class ObservableVector
    {
    public:
        ObservableVector()
        {
            m_vector = winrt::single_threaded_observable_vector<T>();
        }

        // 获取 WinRT 集合
        winrt::Windows::Foundation::Collections::IObservableVector<T> GetVector() const
        {
            return m_vector;
        }

        // 常用操作的包装
        void Append(T const& item)
        {
            m_vector.Append(item);
        }

        void Clear()
        {
            m_vector.Clear();
        }

        uint32_t Size() const
        {
            return m_vector.Size();
        }

        T GetAt(uint32_t index) const
        {
            return m_vector.GetAt(index);
        }

        void RemoveAt(uint32_t index)
        {
            m_vector.RemoveAt(index);
        }

        // 批量操作
        void ReplaceAll(std::vector<T> const& items)
        {
            m_vector.Clear();
            for (auto const& item : items)
            {
                m_vector.Append(item);
            }
        }

    private:
        winrt::Windows::Foundation::Collections::IObservableVector<T> m_vector;
    };
}