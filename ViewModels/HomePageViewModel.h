#pragma once
#include "BindableBase.h"
#include "..\Models\Models.h"
#include "..\Services\Services.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <memory>

namespace winrt::MicrosoftDocsGallery::ViewModels
{
    using namespace Models;
    using namespace Services;

    // 首页 ViewModel
    struct HomePageViewModel : public BindableBase
    {
    public:
        HomePageViewModel();
        ~HomePageViewModel() = default;

        // 属性 - 这些属性会绑定到 XAML 中的控件
        
        // 特色主题集合
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> FeaturedTopics() const
        {
            return m_featuredTopics.GetVector();
        }

        // 最近浏览集合
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> RecentItems() const
        {
            return m_recentItems.GetVector();
        }

        // 加载状态
        bool IsLoading() const { return m_isLoading; }
        void IsLoading(bool value) 
        { 
            if (SetProperty(m_isLoading, value, L"IsLoading"))
            {
                // 通知相关命令状态可能发生变化
                if (m_refreshCommand)
                {
                    m_refreshCommand->RaiseCanExecuteChanged();
                }
            }
        }

        // 欢迎消息
        winrt::hstring WelcomeMessage() const { return m_welcomeMessage; }
        void WelcomeMessage(winrt::hstring const& value)
        {
            SetProperty(m_welcomeMessage, value, L"WelcomeMessage");
        }

        // 用户统计信息
        int TotalPagesViewed() const { return m_appStatistics.TotalPagesViewed; }
        int FavoritesCount() const { return m_appStatistics.FavoritesCount; }
        int DaysUsed() const { return m_appStatistics.DaysUsed; }

        // 命令 - 这些命令会绑定到按钮的 Command 属性
        winrt::Microsoft::UI::Xaml::Input::ICommand RefreshCommand() const { return *m_refreshCommand; }
        winrt::Microsoft::UI::Xaml::Input::ICommand NavigateToTopicCommand() const { return *m_navigateToTopicCommand; }
        winrt::Microsoft::UI::Xaml::Input::ICommand OpenQuickAccessCommand() const { return *m_openQuickAccessCommand; }

        // 公共方法
        winrt::Windows::Foundation::IAsyncAction InitializeAsync();
        winrt::Windows::Foundation::IAsyncAction RefreshDataAsync();
        void NavigateToTopic(winrt::hstring const& url);

        // 事件 - 用于与 View 通信
        winrt::event_token NavigationRequested(winrt::Windows::Foundation::EventHandler<winrt::hstring> const& handler);
        void NavigationRequested(winrt::event_token const& token) noexcept;

    private:
        // 私有字段
        ObservableVector<winrt::Windows::Foundation::IInspectable> m_featuredTopics;
        ObservableVector<winrt::Windows::Foundation::IInspectable> m_recentItems;
        
        bool m_isLoading{ false };
        winrt::hstring m_welcomeMessage{ L"欢迎使用 Microsoft 文档浏览器" };
        AppStatistics m_appStatistics;

        // 服务依赖
        std::shared_ptr<IDataService> m_dataService;
        std::shared_ptr<IFavoritesService> m_favoritesService;
        std::shared_ptr<ISettingsService> m_settingsService;

        // 命令
        std::shared_ptr<AsyncRelayCommand> m_refreshCommand;
        std::shared_ptr<RelayCommand> m_navigateToTopicCommand;
        std::shared_ptr<RelayCommand> m_openQuickAccessCommand;

        // 事件
        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::hstring>> m_navigationRequested;

        // 私有方法
        winrt::Windows::Foundation::IAsyncAction LoadFeaturedTopicsAsync();
        winrt::Windows::Foundation::IAsyncAction LoadRecentItemsAsync();
        winrt::Windows::Foundation::IAsyncAction LoadUserStatisticsAsync();
        void InitializeCommands();
        void UpdateWelcomeMessage();

        // 命令实现
        winrt::Windows::Foundation::IAsyncAction OnRefreshAsync(winrt::Windows::Foundation::IInspectable const& parameter);
        void OnNavigateToTopic(winrt::Windows::Foundation::IInspectable const& parameter);
        void OnOpenQuickAccess(winrt::Windows::Foundation::IInspectable const& parameter);
        bool CanRefresh(winrt::Windows::Foundation::IInspectable const& parameter) const;
    };

    // 基础页面 ViewModel 类 - 包含常见的页面功能
    struct BasePageViewModel : public BindableBase
    {
    public:
        BasePageViewModel();
        virtual ~BasePageViewModel() = default;

        // 通用属性
        winrt::hstring Title() const { return m_title; }
        void Title(winrt::hstring const& value) { SetProperty(m_title, value, L"Title"); }

        bool IsBusy() const { return m_isBusy; }
        void IsBusy(bool value) { SetProperty(m_isBusy, value, L"IsBusy"); }

        winrt::hstring ErrorMessage() const { return m_errorMessage; }
        void ErrorMessage(winrt::hstring const& value) { SetProperty(m_errorMessage, value, L"ErrorMessage"); }

        bool HasError() const { return !m_errorMessage.empty(); }

        // 通用命令
        winrt::Microsoft::UI::Xaml::Input::ICommand GoBackCommand() const { return *m_goBackCommand; }

        // 虚方法 - 子类可以重写
        virtual winrt::Windows::Foundation::IAsyncAction InitializeAsync() { co_return; }
        virtual winrt::Windows::Foundation::IAsyncAction RefreshAsync() { co_return; }

        // 事件
        winrt::event_token NavigationRequested(winrt::Windows::Foundation::EventHandler<winrt::hstring> const& handler);
        void NavigationRequested(winrt::event_token const& token) noexcept;

        winrt::event_token ErrorOccurred(winrt::Windows::Foundation::EventHandler<winrt::hstring> const& handler);
        void ErrorOccurred(winrt::event_token const& token) noexcept;

    protected:
        // 受保护的方法供子类使用
        void ClearError() { ErrorMessage(L""); }
        void SetError(winrt::hstring const& message);
        void RaiseNavigationRequested(winrt::hstring const& destination);

        // 异步操作包装器 - 自动处理 IsBusy 状态和错误
        template<typename TFunc>
        winrt::Windows::Foundation::IAsyncAction ExecuteAsync(TFunc func, winrt::hstring const& errorContext = L"")
        {
            if (m_isBusy) co_return;

            IsBusy(true);
            ClearError();

            try
            {
                co_await func();
            }
            catch (winrt::hresult_error const& ex)
            {
                winrt::hstring message = errorContext.empty() ? 
                    ex.message() : 
                    errorContext + L": " + ex.message();
                SetError(message);
            }
            catch (std::exception const& ex)
            {
                auto message = winrt::to_hstring(ex.what());
                SetError(errorContext.empty() ? message : errorContext + L": " + message);
            }
            catch (...)
            {
                SetError(errorContext.empty() ? L"发生未知错误" : errorContext + L": 发生未知错误");
            }

            IsBusy(false);
        }

    private:
        winrt::hstring m_title;
        bool m_isBusy{ false };
        winrt::hstring m_errorMessage;

        std::shared_ptr<RelayCommand> m_goBackCommand;

        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::hstring>> m_navigationRequested;
        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::hstring>> m_errorOccurred;

        void OnGoBack(winrt::Windows::Foundation::IInspectable const& parameter);
    };
}