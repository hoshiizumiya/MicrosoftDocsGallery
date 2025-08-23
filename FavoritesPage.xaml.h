#pragma once

#include "FavoritesPage.g.h"
#include <vector>
#include <algorithm>

namespace winrt::MicrosoftDocsGallery::implementation
{
    // 收藏夹项目数据结构
    struct FavoriteItem
    {
        winrt::hstring Title;
        winrt::hstring Url;
        winrt::hstring Category;
        winrt::hstring DateAdded;
        
        FavoriteItem() = default;
        FavoriteItem(winrt::hstring title, winrt::hstring url, winrt::hstring category)
            : Title(title), Url(url), Category(category)
        {
            // 生成当前日期
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::tm tm;
            localtime_s(&tm, &time_t);
            
            wchar_t buffer[100];
            wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%Y-%m-%d", &tm);
            DateAdded = winrt::hstring(buffer);
        }
    };

    struct FavoritesPage : FavoritesPageT<FavoritesPage>
    {
        FavoritesPage();

        // 收藏夹管理方法
        void AddToFavorites(winrt::hstring const& url, winrt::hstring const& title);
        void RemoveFromFavorites(winrt::hstring const& url);
        void ClearAllFavorites(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        bool IsInFavorites(winrt::hstring const& url);

        // 事件处理器
        void FavoriteItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void RemoveFavorite_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SearchBox_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        void CategoryFilter_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void SortBy_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void BrowseDocs_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        // 辅助方法
        void LoadFavorites();
        void SaveFavorites();
        void UpdateFavoritesDisplay();
        void FilterAndSortFavorites();
        winrt::hstring DetermineCategoryFromUrl(winrt::hstring const& url);
        
        // 数据成员
        std::vector<FavoriteItem> m_allFavorites;
        std::vector<FavoriteItem> m_filteredFavorites;
        winrt::hstring m_searchText = L"";
        winrt::hstring m_selectedCategory = L"All";
        winrt::hstring m_sortBy = L"DateAdded";
        
        // WinRT集合
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> m_favoritesCollection{ nullptr };
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct FavoritesPage : FavoritesPageT<FavoritesPage, implementation::FavoritesPage>
    {
    };
}