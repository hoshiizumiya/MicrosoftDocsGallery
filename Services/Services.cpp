#include "pch.h"
#include "Services.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;

namespace winrt::MicrosoftDocsGallery::Services
{
    // DataService 实现
    DataService::DataService()
    {
        m_lastCacheUpdate = Windows::Foundation::DateTime::clock::now();
        InitializeDefaultTopics();
    }

    IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> DataService::GetTopicTitlesAsync()
    {
        // 转换为 WinRT 集合
        auto result = winrt::single_threaded_vector<winrt::hstring>();
        for (auto const& title : m_topicTitles)
        {
            result.Append(title);
        }
        
        co_return result;
    }

    IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> DataService::GetTopicUrlsAsync()
    {
        // 转换为 WinRT 集合
        auto result = winrt::single_threaded_vector<winrt::hstring>();
        for (auto const& url : m_topicUrls)
        {
            result.Append(url);
        }
        
        co_return result;
    }

    IAsyncAction DataService::RefreshDataAsync()
    {
        InitializeDefaultTopics();
        m_lastCacheUpdate = Windows::Foundation::DateTime::clock::now();
        co_return;
    }

    void DataService::InitializeDefaultTopics()
    {
        m_topicTitles.clear();
        m_topicUrls.clear();
        
        // 添加默认主题
        m_topicTitles.push_back(L"WinUI 3 入门");
        m_topicUrls.push_back(L"https://learn.microsoft.com/windows/apps/winui/winui3/");
        
        m_topicTitles.push_back(L"XAML 平台");
        m_topicUrls.push_back(L"https://learn.microsoft.com/windows/apps/xaml-platform/");
        
        m_topicTitles.push_back(L".NET 基础");
        m_topicUrls.push_back(L"https://learn.microsoft.com/dotnet/fundamentals/");
        
        m_topicTitles.push_back(L"ASP.NET Core");
        m_topicUrls.push_back(L"https://learn.microsoft.com/aspnet/core/");
        
        m_topicTitles.push_back(L"C# 编程指南");
        m_topicUrls.push_back(L"https://learn.microsoft.com/dotnet/csharp/");
        
        m_topicTitles.push_back(L"异步编程");
        m_topicUrls.push_back(L"https://learn.microsoft.com/dotnet/csharp/asynchronous-programming/");
    }

    bool DataService::IsCacheValid() const
    {
        // 缓存10分钟有效
        auto now = Windows::Foundation::DateTime::clock::now();
        auto timeDiff = now.time_since_epoch() - m_lastCacheUpdate.time_since_epoch();
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(timeDiff);
        return minutes.count() < 10;
    }

    // FavoritesService 实现
    FavoritesService::FavoritesService()
    {
        LoadFavoritesAsync();
    }

    IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> FavoritesService::GetFavoriteUrlsAsync()
    {
        if (!m_isLoaded)
        {
            co_await LoadFavoritesAsync();
        }
        
        // 转换为 WinRT 集合
        auto result = winrt::single_threaded_vector<winrt::hstring>();
        for (auto const& url : m_favoriteUrls)
        {
            result.Append(url);
        }
        
        co_return result;
    }

    IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> FavoritesService::GetFavoriteTitlesAsync()
    {
        if (!m_isLoaded)
        {
            co_await LoadFavoritesAsync();
        }
        
        // 转换为 WinRT 集合
        auto result = winrt::single_threaded_vector<winrt::hstring>();
        for (auto const& title : m_favoriteTitles)
        {
            result.Append(title);
        }
        
        co_return result;
    }

    IAsyncAction FavoritesService::AddToFavoritesAsync(winrt::hstring const& url, winrt::hstring const& title)
    {
        // 检查是否已存在
        bool exists = false;
        for (auto const& favUrl : m_favoriteUrls)
        {
            if (favUrl == url)
            {
                exists = true;
                break;
            }
        }
        
        if (!exists)
        {
            m_favoriteUrls.push_back(url);
            m_favoriteTitles.push_back(title);
            co_await SaveFavoritesAsync();
        }
    }

    IAsyncAction FavoritesService::RemoveFromFavoritesAsync(winrt::hstring const& url)
    {
        auto it = std::find(m_favoriteUrls.begin(), m_favoriteUrls.end(), url);
        if (it != m_favoriteUrls.end())
        {
            auto index = std::distance(m_favoriteUrls.begin(), it);
            m_favoriteUrls.erase(it);
            if (index < m_favoriteTitles.size())
            {
                m_favoriteTitles.erase(m_favoriteTitles.begin() + index);
            }
            co_await SaveFavoritesAsync();
        }
    }

    IAsyncOperation<bool> FavoritesService::IsFavoriteAsync(winrt::hstring const& url)
    {
        if (!m_isLoaded)
        {
            co_await LoadFavoritesAsync();
        }
        
        for (auto const& favUrl : m_favoriteUrls)
        {
            if (favUrl == url)
            {
                co_return true;
            }
        }
        
        co_return false;
    }

    IAsyncAction FavoritesService::LoadFavoritesAsync()
    {
        try
        {
            // 简化实现 - 使用默认收藏夹
            if (m_favoriteUrls.empty())
            {
                m_favoriteUrls.push_back(L"https://learn.microsoft.com/");
                m_favoriteTitles.push_back(L"Microsoft Learn");
                
                m_favoriteUrls.push_back(L"https://learn.microsoft.com/windows/apps/winui/");
                m_favoriteTitles.push_back(L"WinUI 3 文档");
                
                m_favoriteUrls.push_back(L"https://learn.microsoft.com/dotnet/");
                m_favoriteTitles.push_back(L".NET 文档");
            }
            
            m_isLoaded = true;
        }
        catch (...)
        {
            // 忽略加载错误
            m_isLoaded = true;
        }
        
        co_return;
    }

    IAsyncAction FavoritesService::SaveFavoritesAsync()
    {
        try
        {
            // TODO: 实现保存到本地存储
            // 这里可以序列化到 ApplicationData
        }
        catch (...)
        {
            // 忽略保存错误
        }
        
        co_return;
    }

    // SettingsService 实现
    SettingsService::SettingsService()
    {
        LoadStatisticsAsync();
    }

    IAsyncOperation<int32_t> SettingsService::GetTotalPagesViewedAsync()
    {
        if (!m_statisticsLoaded)
        {
            co_await LoadStatisticsAsync();
        }
        
        co_return m_totalPagesViewed;
    }

    IAsyncOperation<int32_t> SettingsService::GetFavoritesCountAsync()
    {
        if (!m_statisticsLoaded)
        {
            co_await LoadStatisticsAsync();
        }
        
        co_return m_favoritesCount;
    }

    IAsyncOperation<int32_t> SettingsService::GetDaysUsedAsync()
    {
        if (!m_statisticsLoaded)
        {
            co_await LoadStatisticsAsync();
        }
        
        co_return m_daysUsed;
    }

    IAsyncAction SettingsService::UpdateStatisticsAsync(int32_t pagesViewed, int32_t favoritesCount, int32_t daysUsed)
    {
        m_totalPagesViewed = pagesViewed;
        m_favoritesCount = favoritesCount;
        m_daysUsed = daysUsed;
        
        co_await SaveStatisticsAsync();
    }

    IAsyncAction SettingsService::LoadStatisticsAsync()
    {
        try
        {
            // 使用默认统计
            m_totalPagesViewed = 42;
            m_favoritesCount = 3;
            m_daysUsed = 7;
            m_statisticsLoaded = true;
        }
        catch (...)
        {
            m_totalPagesViewed = 0;
            m_favoritesCount = 0;
            m_daysUsed = 0;
            m_statisticsLoaded = true;
        }
        
        co_return;
    }

    IAsyncAction SettingsService::SaveStatisticsAsync()
    {
        try
        {
            // TODO: 保存到本地存储
        }
        catch (...)
        {
            // 忽略保存错误
        }
        
        co_return;
    }

    // ServiceLocator 实现
    ServiceLocator& ServiceLocator::Instance()
    {
        static ServiceLocator instance;
        return instance;
    }

    void ServiceLocator::RegisterDataService(std::shared_ptr<IDataService> service)
    {
        m_dataService = service;
    }

    void ServiceLocator::RegisterFavoritesService(std::shared_ptr<IFavoritesService> service)
    {
        m_favoritesService = service;
    }

    void ServiceLocator::RegisterSettingsService(std::shared_ptr<ISettingsService> service)
    {
        m_settingsService = service;
    }

    std::shared_ptr<IDataService> ServiceLocator::GetDataService() const
    {
        if (!m_dataService)
        {
            // 自动创建默认服务
            const_cast<ServiceLocator*>(this)->m_dataService = std::make_shared<DataService>();
        }
        return m_dataService;
    }

    std::shared_ptr<IFavoritesService> ServiceLocator::GetFavoritesService() const
    {
        if (!m_favoritesService)
        {
            // 自动创建默认服务
            const_cast<ServiceLocator*>(this)->m_favoritesService = std::make_shared<FavoritesService>();
        }
        return m_favoritesService;
    }

    std::shared_ptr<ISettingsService> ServiceLocator::GetSettingsService() const
    {
        if (!m_settingsService)
        {
            // 自动创建默认服务
            const_cast<ServiceLocator*>(this)->m_settingsService = std::make_shared<SettingsService>();
        }
        return m_settingsService;
    }

    void ServiceLocator::InitializeDefaultServices()
    {
        if (!m_dataService)
            m_dataService = std::make_shared<DataService>();
        
        if (!m_favoritesService)
            m_favoritesService = std::make_shared<FavoritesService>();
        
        if (!m_settingsService)
            m_settingsService = std::make_shared<SettingsService>();
    }
}