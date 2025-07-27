#pragma once

#include "HomePage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        HomePage();

        void DefaultCarousel();

        // 事件处理器
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Animated_GotItem(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OpenWinUIDocs_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void QuickAccess_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        // 异步数据加载方法 - 使用WinRT异步模式
        void OnLoaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        winrt::Windows::Foundation::IAsyncAction LoadTopicsAsync();
        winrt::Windows::Foundation::IAsyncAction LoadRecentItemsAsync();
        winrt::Windows::Foundation::IAsyncAction LoadUserStatisticsAsync();
        winrt::Windows::Foundation::IAsyncAction StartAutoScrollAsync();

        // 持久化集合
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> m_items{ nullptr };
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> m_recentItems{ nullptr };

    private:
        // 辅助方法
        void UpdateNavigationSelection();
        void InitializeDefaultContent();
        void LoadUserPreferences();
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}
