#pragma once

#include "WebViewPage.g.h"
#include <vector>
#include <memory>
#include <map>

namespace winrt::MicrosoftDocsGallery::implementation
{
    // Tab数据结构
    struct TabData
    {
        winrt::hstring Id;
        winrt::hstring Title;
        winrt::hstring Url;
        bool IsLoading = false;
        winrt::Microsoft::UI::Xaml::Controls::WebView2 WebView = nullptr;
        
        TabData() = default;
    };

    struct WebViewPage : WebViewPageT<WebViewPage>
    {
        WebViewPage();

        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        
        // TabView events
        void TabView_AddButtonClick(winrt::Microsoft::UI::Xaml::Controls::TabView const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void TabView_TabCloseRequested(winrt::Microsoft::UI::Xaml::Controls::TabView const& sender, winrt::Microsoft::UI::Xaml::Controls::TabViewTabCloseRequestedEventArgs const& args);
        void TabView_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

        // NavigationView TOC events
        void DocNavigation_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
        void SectionSearch_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        void FavoriteButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void RefreshPage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OpenInBrowser_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SharePage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        // Public API methods
        void AddNewTab(winrt::hstring const& url, winrt::hstring const& title);
        void CloseTab(winrt::hstring const& tabId);
        void SwitchToTab(winrt::hstring const& tabId);
        void UpdateTabTitle(winrt::hstring const& tabId, winrt::hstring const& title);
        void LoadDocumentStructure(winrt::hstring const& url);
        void NavigateToSection(winrt::hstring const& sectionId);
        void SavePageState();
        void RestorePageState();

    private:
        // Helper methods
        winrt::Microsoft::UI::Xaml::Controls::TabViewItem CreateTabItem(std::shared_ptr<TabData> tabData);
        winrt::Microsoft::UI::Xaml::Controls::WebView2 CreateWebView();
        winrt::Windows::Foundation::IAsyncAction LoadWebPageAsync(std::shared_ptr<TabData> tabData);
        std::shared_ptr<TabData> GetCurrentTabData();
        std::shared_ptr<TabData> GetTabDataById(winrt::hstring const& tabId);
        winrt::hstring GenerateTabId();
        
        // 新增的辅助方法
        void PopulateNavigationView(std::shared_ptr<TabData> tabData);
        void FilterNavigationItems(winrt::hstring const& searchText);
        winrt::Windows::Foundation::IAsyncAction ExtractTableOfContentsAsync(std::shared_ptr<TabData> tabData);

        // Member variables
        std::vector<std::shared_ptr<TabData>> m_tabs;
        winrt::hstring m_currentTabId;
        bool m_isInitialized = false;
        int m_nextTabNumber = 1;
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct WebViewPage : WebViewPageT<WebViewPage, implementation::WebViewPage>
    {
    };
}
