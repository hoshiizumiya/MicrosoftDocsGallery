#pragma once

#include "HomePage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        HomePage();

        // 事件处理器 - 在 MVVM 中这些应该最小化
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        // ViewModel 实例 - 使用 IInspectable 指针
        winrt::Windows::Foundation::IInspectable m_viewModel{ nullptr };

        // 事件令牌
        winrt::event_token m_navigationRequestedToken;

        // 私有方法
        void InitializeViewModel();
        void OnNavigationRequested(winrt::Windows::Foundation::IInspectable const& sender, winrt::hstring const& destination);
        void UpdateNavigationSelection();
    public:
        void Animated_GotItem(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void QuickAccess_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OpenWinUIDocs_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}
