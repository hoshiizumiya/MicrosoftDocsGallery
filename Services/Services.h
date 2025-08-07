#pragma once
#include "Models/Models.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Data.Json.h>
#include <vector>
#include <memory>
#include <functional>

namespace winrt::MicrosoftDocsGallery::Services
{
    using namespace Models;

    // 数据服务接口
    struct IDataService
    {
        virtual ~IDataService() = default;
        virtual winrt::Windows::Foundation::IAsyncOperation<std::vector<LearningTopic>> GetTopicsAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<std::vector<LearningTopic>> GetTopicsByCategoryAsync(winrt::hstring const& category) = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<std::vector<LearningTopic>> SearchTopicsAsync(winrt::hstring const& query) = 0;
        virtual winrt::Windows::Foundation::IAsyncAction SaveTopicAsync(LearningTopic const& topic) = 0;
    };

    // 收藏夹服务接口
    struct IFavoritesService
    {
        virtual ~IFavoritesService() = default;
        virtual winrt::Windows::Foundation::IAsyncOperation<std::vector<FavoriteItem>> GetFavoritesAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncAction AddToFavoritesAsync(FavoriteItem const& item) = 0;
        virtual winrt::Windows::Foundation::IAsyncAction RemoveFromFavoritesAsync(winrt::hstring const& url) = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<bool> IsFavoriteAsync(winrt::hstring const& url) = 0;
    };

    // 设置服务接口
    struct ISettingsService
    {
        virtual ~ISettingsService() = default;
        virtual winrt::Windows::Foundation::IAsyncOperation<UserPreferences> GetUserPreferencesAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncAction SaveUserPreferencesAsync(UserPreferences const& preferences) = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<AppStatistics> GetAppStatisticsAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncAction UpdateAppStatisticsAsync(AppStatistics const& statistics) = 0;
    };

    // 数据服务实现
    class DataService : public IDataService
    {
    public:
        DataService();

        // IDataService 实现
        winrt::Windows::Foundation::IAsyncOperation<std::vector<LearningTopic>> GetTopicsAsync() override;
        winrt::Windows::Foundation::IAsyncOperation<std::vector<LearningTopic>> GetTopicsByCategoryAsync(winrt::hstring const& category) override;
        winrt::Windows::Foundation::IAsyncOperation<std::vector<LearningTopic>> SearchTopicsAsync(winrt::hstring const& query) override;
        winrt::Windows::Foundation::IAsyncAction SaveTopicAsync(LearningTopic const& topic) override;

    private:
        std::vector<LearningTopic> m_cachedTopics;
        winrt::Windows::Foundation::DateTime m_lastCacheUpdate;
        
        winrt::Windows::Foundation::IAsyncOperation<std::vector<LearningTopic>> LoadTopicsFromApiAsync();
        std::vector<LearningTopic> GetDefaultTopics();
        bool IsCacheValid() const;
    };

    // 收藏夹服务实现
    class FavoritesService : public IFavoritesService
    {
    public:
        FavoritesService();

        // IFavoritesService 实现
        winrt::Windows::Foundation::IAsyncOperation<std::vector<FavoriteItem>> GetFavoritesAsync() override;
        winrt::Windows::Foundation::IAsyncAction AddToFavoritesAsync(FavoriteItem const& item) override;
        winrt::Windows::Foundation::IAsyncAction RemoveFromFavoritesAsync(winrt::hstring const& url) override;
        winrt::Windows::Foundation::IAsyncOperation<bool> IsFavoriteAsync(winrt::hstring const& url) override;

    private:
        std::vector<FavoriteItem> m_favorites;
        bool m_isLoaded{ false };
        
        winrt::Windows::Foundation::IAsyncAction LoadFavoritesAsync();
        winrt::Windows::Foundation::IAsyncAction SaveFavoritesAsync();
        winrt::hstring GetFavoritesFilePath() const;
    };

    // 设置服务实现
    class SettingsService : public ISettingsService
    {
    public:
        SettingsService();

        // ISettingsService 实现
        winrt::Windows::Foundation::IAsyncOperation<UserPreferences> GetUserPreferencesAsync() override;
        winrt::Windows::Foundation::IAsyncAction SaveUserPreferencesAsync(UserPreferences const& preferences) override;
        winrt::Windows::Foundation::IAsyncOperation<AppStatistics> GetAppStatisticsAsync() override;
        winrt::Windows::Foundation::IAsyncAction UpdateAppStatisticsAsync(AppStatistics const& statistics) override;

    private:
        UserPreferences m_userPreferences;
        AppStatistics m_appStatistics;
        bool m_preferencesLoaded{ false };
        bool m_statisticsLoaded{ false };

        winrt::Windows::Foundation::IAsyncAction LoadPreferencesAsync();
        winrt::Windows::Foundation::IAsyncAction LoadStatisticsAsync();
        winrt::Windows::Storage::StorageFolder GetLocalSettingsFolder();
    };

    // 服务定位器 - 依赖注入的简单实现
    class ServiceLocator
    {
    public:
        static ServiceLocator& Instance();

        // 服务注册
        void RegisterDataService(std::shared_ptr<IDataService> service);
        void RegisterFavoritesService(std::shared_ptr<IFavoritesService> service);
        void RegisterSettingsService(std::shared_ptr<ISettingsService> service);

        // 服务获取
        std::shared_ptr<IDataService> GetDataService() const;
        std::shared_ptr<IFavoritesService> GetFavoritesService() const;
        std::shared_ptr<ISettingsService> GetSettingsService() const;

        // 初始化默认服务
        void InitializeDefaultServices();

    private:
        ServiceLocator() = default;
        
        std::shared_ptr<IDataService> m_dataService;
        std::shared_ptr<IFavoritesService> m_favoritesService;
        std::shared_ptr<ISettingsService> m_settingsService;
    };
}