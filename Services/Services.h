#pragma once
#include "pch.h"
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

using namespace winrt;

namespace winrt::MicrosoftDocsGallery::Services
{
    // IDataService - plain C++ service interface (not WinRT interface)
    struct IDataService
    {
        virtual ~IDataService() = default;
        virtual winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> GetTopicTitlesAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> GetTopicUrlsAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncAction RefreshDataAsync() = 0;
    };

    // Concrete DataService
    class DataService : public IDataService
    {
    public:
        DataService();

        winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> GetTopicTitlesAsync() override;
        winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> GetTopicUrlsAsync() override;
        winrt::Windows::Foundation::IAsyncAction RefreshDataAsync() override;

    private:
        std::vector<winrt::hstring> m_topicTitles;
        std::vector<winrt::hstring> m_topicUrls;
        winrt::Windows::Foundation::DateTime m_lastCacheUpdate{};

        void InitializeDefaultTopics();
        bool IsCacheValid() const;
    };

    // IFavoritesService - plain C++ service interface
    struct IFavoritesService
    {
        virtual ~IFavoritesService() = default;
        virtual winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> GetFavoriteUrlsAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> GetFavoriteTitlesAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncAction AddToFavoritesAsync(winrt::hstring const& url, winrt::hstring const& title) = 0;
        virtual winrt::Windows::Foundation::IAsyncAction RemoveFromFavoritesAsync(winrt::hstring const& url) = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<bool> IsFavoriteAsync(winrt::hstring const& url) = 0;
    };

    class FavoritesService : public IFavoritesService
    {
    public:
        FavoritesService();

        winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> GetFavoriteUrlsAsync() override;
        winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> GetFavoriteTitlesAsync() override;
        winrt::Windows::Foundation::IAsyncAction AddToFavoritesAsync(winrt::hstring const& url, winrt::hstring const& title) override;
        winrt::Windows::Foundation::IAsyncAction RemoveFromFavoritesAsync(winrt::hstring const& url) override;
        winrt::Windows::Foundation::IAsyncOperation<bool> IsFavoriteAsync(winrt::hstring const& url) override;

    private:
        std::vector<winrt::hstring> m_favoriteUrls;
        std::vector<winrt::hstring> m_favoriteTitles;
        bool m_isLoaded{ false };

        winrt::Windows::Foundation::IAsyncAction LoadFavoritesAsync();
        winrt::Windows::Foundation::IAsyncAction SaveFavoritesAsync();
    };

    // ISettingsService - plain C++ service interface
    struct ISettingsService
    {
        virtual ~ISettingsService() = default;
        virtual winrt::Windows::Foundation::IAsyncOperation<int32_t> GetTotalPagesViewedAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<int32_t> GetFavoritesCountAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncOperation<int32_t> GetDaysUsedAsync() = 0;
        virtual winrt::Windows::Foundation::IAsyncAction UpdateStatisticsAsync(int32_t pagesViewed, int32_t favoritesCount, int32_t daysUsed) = 0;
    };

    class SettingsService : public ISettingsService
    {
    public:
        SettingsService();

        winrt::Windows::Foundation::IAsyncOperation<int32_t> GetTotalPagesViewedAsync() override;
        winrt::Windows::Foundation::IAsyncOperation<int32_t> GetFavoritesCountAsync() override;
        winrt::Windows::Foundation::IAsyncOperation<int32_t> GetDaysUsedAsync() override;
        winrt::Windows::Foundation::IAsyncAction UpdateStatisticsAsync(int32_t pagesViewed, int32_t favoritesCount, int32_t daysUsed) override;

    private:
        int32_t m_totalPagesViewed{ 42 };
        int32_t m_favoritesCount{ 3 };
        int32_t m_daysUsed{ 7 };
        bool m_statisticsLoaded{ false };

        winrt::Windows::Foundation::IAsyncAction LoadStatisticsAsync();
        winrt::Windows::Foundation::IAsyncAction SaveStatisticsAsync();
    };

    // ServiceLocator - simple DI container
    class ServiceLocator
    {
    public:
        static ServiceLocator& Instance();

        void RegisterDataService(std::shared_ptr<IDataService> service);
        void RegisterFavoritesService(std::shared_ptr<IFavoritesService> service);
        void RegisterSettingsService(std::shared_ptr<ISettingsService> service);

        std::shared_ptr<IDataService> GetDataService() const;
        std::shared_ptr<IFavoritesService> GetFavoritesService() const;
        std::shared_ptr<ISettingsService> GetSettingsService() const;

        void InitializeDefaultServices();

    private:
        ServiceLocator() = default;

        std::shared_ptr<IDataService> m_dataService;
        std::shared_ptr<IFavoritesService> m_favoritesService;
        std::shared_ptr<ISettingsService> m_settingsService;
    };
}