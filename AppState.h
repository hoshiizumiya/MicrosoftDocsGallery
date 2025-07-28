#pragma once

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::MicrosoftDocsGallery
{
    // 应用程序全局状态管理类
    class AppState
    {
    public:
        static AppState& Instance()
        {
            static AppState instance;
            return instance;
        }

        // 收藏夹管理
        struct FavoriteItem
        {
            winrt::hstring Title;
            winrt::hstring Url;
            winrt::hstring Category;
            winrt::hstring DateAdded;
        };

        // 用户设置
        struct UserSettings
        {
            bool IsDarkMode = false;
            double FontSize = 16.0;
            winrt::hstring Language = L"en-US";
            bool AutoSave = true;
            bool ShowNotifications = true;
        };

        // 浏览历史
        struct HistoryItem
        {
            winrt::hstring Title;
            winrt::hstring Url;
            winrt::hstring Timestamp;
            int VisitCount = 1;
        };

        // 应用统计
        struct AppStatistics
        {
            int PagesViewed = 0;
            int FavoritesCount = 0;
            int DaysUsed = 0;
            int MinutesLearning = 0;
            winrt::hstring LastUsed;
        };

        // 公共方法
        void Initialize();
        void SaveState();
        void LoadState();

        // 收藏夹管理
        void AddFavorite(FavoriteItem const& item);
        void RemoveFavorite(winrt::hstring const& url);
        bool IsFavorite(winrt::hstring const& url) const;
        winrt::Windows::Foundation::Collections::IVector<FavoriteItem> GetFavorites() const;
        winrt::Windows::Foundation::Collections::IVector<FavoriteItem> GetFavoritesByCategory(winrt::hstring const& category) const;

        // 历史记录管理
        void AddToHistory(HistoryItem const& item);
        void ClearHistory();
        winrt::Windows::Foundation::Collections::IVector<HistoryItem> GetRecentHistory(int count = 10) const;
        winrt::Windows::Foundation::Collections::IVector<HistoryItem> GetMostVisited(int count = 5) const;

        // 用户设置
        UserSettings& GetSettings() { return m_settings; }
        void SaveSettings();
        void LoadSettings();

        // 统计信息
        AppStatistics& GetStatistics() { return m_statistics; }
        void UpdateStatistics();
        void IncrementPageViewed();
        void AddLearningTime(int minutes);

        // 搜索建议
        winrt::Windows::Foundation::Collections::IVector<winrt::hstring> GetSearchSuggestions(winrt::hstring const& query, int maxResults = 10) const;
        void AddSearchTerm(winrt::hstring const& term);

        // 主题管理
        void SetTheme(bool isDarkMode);
        void SetFontSize(double size);
        void SetLanguage(winrt::hstring const& language);

    private:
        AppState() = default;
        ~AppState() = default;
        AppState(AppState const&) = delete;
        AppState& operator=(AppState const&) = delete;

        // 数据存储 - 使用WinRT集合
        winrt::Windows::Foundation::Collections::IVector<FavoriteItem> m_favorites{ winrt::single_threaded_vector<FavoriteItem>() };
        winrt::Windows::Foundation::Collections::IVector<HistoryItem> m_history{ winrt::single_threaded_vector<HistoryItem>() };
        winrt::Windows::Foundation::Collections::IVector<winrt::hstring> m_searchTerms{ winrt::single_threaded_vector<winrt::hstring>() };
        UserSettings m_settings;
        AppStatistics m_statistics;

        // 持久化相关
        void SaveToLocalStorage(winrt::hstring const& key, winrt::hstring const& value);
        winrt::hstring LoadFromLocalStorage(winrt::hstring const& key, winrt::hstring const& defaultValue = L"");
        
        // JSON序列化辅助
        winrt::hstring SerializeFavorites() const;
        void DeserializeFavorites(winrt::hstring const& json);
        winrt::hstring SerializeHistory() const;
        void DeserializeHistory(winrt::hstring const& json);
        winrt::hstring SerializeSettings() const;
        void DeserializeSettings(winrt::hstring const& json);

        // 数据验证
        bool IsValidUrl(winrt::hstring const& url) const;
        winrt::hstring SanitizeTitle(winrt::hstring const& title) const;
        
        // 内部状态
        bool m_initialized = false;
        winrt::Windows::Storage::ApplicationDataContainer m_localSettings{ nullptr };
    };

    // 便捷宏定义
    #define APP_STATE AppState::Instance()
}