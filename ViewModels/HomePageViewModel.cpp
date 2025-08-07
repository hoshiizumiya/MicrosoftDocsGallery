#include "pch.h"
#include "HomePageViewModel.h"
#include <winrt/Windows.Globalization.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;

namespace winrt::MicrosoftDocsGallery::ViewModels
{
    // HomePageViewModel 实现
    HomePageViewModel::HomePageViewModel()
    {
        // 从服务定位器获取服务实例
        auto& serviceLocator = ServiceLocator::Instance();
        m_dataService = serviceLocator.GetDataService();
        m_favoritesService = serviceLocator.GetFavoritesService();
        m_settingsService = serviceLocator.GetSettingsService();

        // 初始化命令
        InitializeCommands();

        // 更新欢迎消息
        UpdateWelcomeMessage();
    }

    void HomePageViewModel::InitializeCommands()
    {
        // 刷新命令
        m_refreshCommand = std::make_shared<AsyncRelayCommand>(
            [this](auto const& param) { return OnRefreshAsync(param); },
            [this](auto const& param) { return CanRefresh(param); }
        );

        // 导航到主题命令
        m_navigateToTopicCommand = std::make_shared<RelayCommand>(
            [this](auto const& param) { OnNavigateToTopic(param); }
        );

        // 快速访问命令
        m_openQuickAccessCommand = std::make_shared<RelayCommand>(
            [this](auto const& param) { OnOpenQuickAccess(param); }
        );
    }

    IAsyncAction HomePageViewModel::InitializeAsync()
    {
        if (m_isLoading) co_return;

        IsLoading(true);

        try
        {
            // 并行加载所有数据
            auto loadFeatured = LoadFeaturedTopicsAsync();
            auto loadRecent = LoadRecentItemsAsync();
            auto loadStats = LoadUserStatisticsAsync();

            // 等待所有加载完成
            co_await loadFeatured;
            co_await loadRecent;
            co_await loadStats;
        }
        catch (...)
        {
            // 错误处理 - 在实际应用中应该有更详细的错误处理
        }

        IsLoading(false);
    }

    IAsyncAction HomePageViewModel::RefreshDataAsync()
    {
        co_await InitializeAsync();
    }

    IAsyncAction HomePageViewModel::LoadFeaturedTopicsAsync()
    {
        try
        {
            // 从数据服务获取特色主题
            auto topics = co_await m_dataService->GetTopicsAsync();
            
            // 清空当前集合
            m_featuredTopics.Clear();

            // 只取前6个作为特色主题
            size_t count = std::min(topics.size(), static_cast<size_t>(6));
            for (size_t i = 0; i < count; ++i)
            {
                // 将 LearningTopic 包装为 IInspectable
                m_featuredTopics.Append(winrt::box_value(topics[i].Title));
            }

            // 通知 UI 更新
            RaisePropertyChanged(L"FeaturedTopics");
        }
        catch (...)
        {
            // 错误处理
        }
    }

    IAsyncAction HomePageViewModel::LoadRecentItemsAsync()
    {
        try
        {
            // 模拟从本地存储加载最近浏览项目
            co_await winrt::resume_after(TimeSpan{ 1000000 }); // 100ms 延迟

            m_recentItems.Clear();
            
            // 添加模拟数据
            std::vector<winrt::hstring> recentItems = {
                L"WinUI 3 入门教程",
                L"C++/WinRT 编程指南",
                L"MVVM 模式最佳实践",
                L"异步编程技术",
                L"数据绑定深度解析"
            };

            for (auto const& item : recentItems)
            {
                m_recentItems.Append(winrt::box_value(item));
            }

            RaisePropertyChanged(L"RecentItems");
        }
        catch (...)
        {
            // 错误处理
        }
    }

    IAsyncAction HomePageViewModel::LoadUserStatisticsAsync()
    {
        try
        {
            // 从设置服务加载统计信息
            m_appStatistics = co_await m_settingsService->GetAppStatisticsAsync();
            
            // 通知相关属性更新
            RaisePropertyChanged(L"TotalPagesViewed");
            RaisePropertyChanged(L"FavoritesCount");
            RaisePropertyChanged(L"DaysUsed");
        }
        catch (...)
        {
            // 使用默认统计信息
            m_appStatistics = AppStatistics{};
        }
    }

    void HomePageViewModel::UpdateWelcomeMessage()
    {
        try
        {
            // 根据当前时间生成个性化欢迎消息
            auto calendar = winrt::Windows::Globalization::Calendar();
            calendar.SetToNow();
            
            auto hour = calendar.Hour();
            winrt::hstring greeting;

            if (hour < 12)
            {
                greeting = L"早上好";
            }
            else if (hour < 18)
            {
                greeting = L"下午好";
            }
            else
            {
                greeting = L"晚上好";
            }

            WelcomeMessage(greeting + L"，欢迎使用 Microsoft 文档浏览器！");
        }
        catch (...)
        {
            WelcomeMessage(L"欢迎使用 Microsoft 文档浏览器！");
        }
    }

    void HomePageViewModel::NavigateToTopic(winrt::hstring const& url)
    {
        if (!url.empty())
        {
            // 触发导航事件
            m_navigationRequested(*this, url);
        }
    }

    // 命令实现
    IAsyncAction HomePageViewModel::OnRefreshAsync(IInspectable const& /*parameter*/)
    {
        co_await RefreshDataAsync();
    }

    void HomePageViewModel::OnNavigateToTopic(IInspectable const& parameter)
    {
        if (auto url = parameter.try_as<winrt::hstring>())
        {
            NavigateToTopic(url.value());
        }
    }

    void HomePageViewModel::OnOpenQuickAccess(IInspectable const& parameter)
    {
        // 处理快速访问逻辑
        if (auto tag = parameter.try_as<winrt::hstring>())
        {
            winrt::hstring url;
            auto tagValue = tag.value();
            
            if (tagValue == L"winui")
            {
                url = L"https://learn.microsoft.com/windows/apps/winui/";
            }
            else if (tagValue == L"cppwinrt")
            {
                url = L"https://learn.microsoft.com/windows/uwp/cpp-and-winrt-apis/";
            }
            else if (tagValue == L"dotnet")
            {
                url = L"https://learn.microsoft.com/dotnet/";
            }

            if (!url.empty())
            {
                NavigateToTopic(url);
            }
        }
    }

    bool HomePageViewModel::CanRefresh(IInspectable const& /*parameter*/) const
    {
        return !m_isLoading;
    }

    // 事件实现
    winrt::event_token HomePageViewModel::NavigationRequested(
        winrt::Windows::Foundation::EventHandler<winrt::hstring> const& handler)
    {
        return m_navigationRequested.add(handler);
    }

    void HomePageViewModel::NavigationRequested(winrt::event_token const& token) noexcept
    {
        m_navigationRequested.remove(token);
    }

    // BasePageViewModel 实现
    BasePageViewModel::BasePageViewModel()
    {
        // 初始化返回命令
        m_goBackCommand = std::make_shared<RelayCommand>(
            [this](auto const& param) { OnGoBack(param); }
        );
    }

    void BasePageViewModel::SetError(winrt::hstring const& message)
    {
        ErrorMessage(message);
        RaisePropertyChanged(L"HasError");
        m_errorOccurred(*this, message);
    }

    void BasePageViewModel::RaiseNavigationRequested(winrt::hstring const& destination)
    {
        m_navigationRequested(*this, destination);
    }

    void BasePageViewModel::OnGoBack(IInspectable const& /*parameter*/)
    {
        RaiseNavigationRequested(L"GoBack");
    }

    winrt::event_token BasePageViewModel::NavigationRequested(
        winrt::Windows::Foundation::EventHandler<winrt::hstring> const& handler)
    {
        return m_navigationRequested.add(handler);
    }

    void BasePageViewModel::NavigationRequested(winrt::event_token const& token) noexcept
    {
        m_navigationRequested.remove(token);
    }

    winrt::event_token BasePageViewModel::ErrorOccurred(
        winrt::Windows::Foundation::EventHandler<winrt::hstring> const& handler)
    {
        return m_errorOccurred.add(handler);
    }

    void BasePageViewModel::ErrorOccurred(winrt::event_token const& token) noexcept
    {
        m_errorOccurred.remove(token);
    }
}