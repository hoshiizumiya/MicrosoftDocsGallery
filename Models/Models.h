#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <string>
#include <vector>
#include <memory>

namespace winrt::MicrosoftDocsGallery::Models
{
    // 学习主题模型
    struct LearningTopic
    {
        winrt::hstring Title;
        winrt::hstring Description;
        winrt::hstring Url;
        winrt::hstring Icon;
        winrt::hstring Category;
        winrt::Windows::Foundation::DateTime DateAdded;
        bool IsFavorite{ false };
        int ViewCount{ 0 };
        
        LearningTopic() = default;
        LearningTopic(
            winrt::hstring title, 
            winrt::hstring description, 
            winrt::hstring url, 
            winrt::hstring icon, 
            winrt::hstring category
        ) : Title(title), Description(description), Url(url), Icon(icon), Category(category)
        {
            DateAdded = winrt::Windows::Foundation::DateTime::clock::now();
        }
    };

    // 用户偏好设置模型
    struct UserPreferences
    {
        bool IsDarkMode{ false };
        double FontSize{ 16.0 };
        winrt::hstring Language{ L"zh-CN" };
        winrt::hstring DefaultCategory{ L"All" };
        bool AutoLoadContent{ true };
        bool ShowNotifications{ true };
        
        UserPreferences() = default;
    };

    // 应用统计模型
    struct AppStatistics
    {
        int TotalPagesViewed{ 0 };
        int FavoritesCount{ 0 };
        int DaysUsed{ 0 };
        int TotalLearningMinutes{ 0 };
        winrt::Windows::Foundation::DateTime LastUsed;
        winrt::Windows::Foundation::DateTime FirstUsed;
        
        AppStatistics() 
        {
            auto now = winrt::Windows::Foundation::DateTime::clock::now();
            LastUsed = now;
            FirstUsed = now;
        }
    };

    // 收藏夹项目模型
    struct FavoriteItem
    {
        winrt::hstring Title;
        winrt::hstring Url;
        winrt::hstring Category;
        winrt::Windows::Foundation::DateTime DateAdded;
        winrt::hstring Notes;
        int VisitCount{ 0 };
        
        FavoriteItem() = default;
        FavoriteItem(winrt::hstring title, winrt::hstring url, winrt::hstring category)
            : Title(title), Url(url), Category(category)
        {
            DateAdded = winrt::Windows::Foundation::DateTime::clock::now();
        }
    };

    // 浏览历史模型
    struct HistoryItem
    {
        winrt::hstring Title;
        winrt::hstring Url;
        winrt::Windows::Foundation::DateTime LastVisited;
        int VisitCount{ 1 };
        winrt::Windows::Foundation::TimeSpan Duration;
        
        HistoryItem() = default;
        HistoryItem(winrt::hstring title, winrt::hstring url)
            : Title(title), Url(url)
        {
            LastVisited = winrt::Windows::Foundation::DateTime::clock::now();
        }
    };
}