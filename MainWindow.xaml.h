#pragma once

#include "MainWindow.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        // Navigation methods
        void openHomePage();
        void openFundamentalsPage();
        void openSettingsPage();
        void openWebViewPage(winrt::hstring const& url);
        void openFavoritesPage();

        // Window management
        winrt::Windows::Foundation::IAsyncAction SetIconAsync(winrt::Microsoft::UI::Windowing::AppWindow window);
        void InitWindowStyle(winrt::Microsoft::UI::Xaml::Window const& window);
        void SaveWindowState();
        void RestoreWindowState();

        // Favorites management
        void AddToFavorites(winrt::hstring const& url, winrt::hstring const& title);
        void RemoveFromFavorites(winrt::hstring const& url);
        bool IsInFavorites(winrt::hstring const& url);

        // Search functionality
        void ShowSearchSuggestions(winrt::hstring const& query);
        
        // Navigation management
        void SetNavigationSelection(winrt::hstring const& pageTag);

        // Event handlers (XAML bound)
        void NavigationView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
        void mainFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void nav_BackRequested(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void SearchBox_QuerySubmitted(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs const& args);
        void SearchBox_TextChanged(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const& e);

    private:
        // Helper methods
        void UpdateNavigationSelection(winrt::hstring const& pageTag);
        void LoadFavorites();
        void SaveFavorites();
        void UpdateFavoritesInNavigation();
        void AddSearchSuggestions(winrt::hstring const& query);
        void InitializeApplication();

        // Member variables - 使用简单的平行数组存储收藏夹
        winrt::Windows::Foundation::Collections::IVector<winrt::hstring> m_favoriteUrls{ winrt::single_threaded_vector<winrt::hstring>() };
        winrt::Windows::Foundation::Collections::IVector<winrt::hstring> m_favoriteTitles{ winrt::single_threaded_vector<winrt::hstring>() };
        bool m_favoritesLoaded = false;
        winrt::hstring m_currentSearchQuery = L"";
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
