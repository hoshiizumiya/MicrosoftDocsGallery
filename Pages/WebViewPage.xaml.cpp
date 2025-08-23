#include "pch.h"
#include "WebViewPage.xaml.h"
#if __has_include("WebViewPage.g.cpp")
#include "WebViewPage.g.cpp"
#endif
#include <random>
#include <sstream>
#include <algorithm>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::MicrosoftDocsGallery::implementation
{
    WebViewPage::WebViewPage()
    {
        InitializeComponent();
        m_isInitialized = false;
    }

    void WebViewPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        if (!m_isInitialized)
        {
            // 首次加载时，如果有参数则创建标签页
            if (e.Parameter())
            {
                auto url = winrt::unbox_value<winrt::hstring>(e.Parameter());
                if (!url.empty())
                {
                    AddNewTab(url, L"Loading...");
                }
            }
            else
            {
                // 创建默认标签页
                AddNewTab(L"https://learn.microsoft.com/", L"Microsoft Learn");
            }
            m_isInitialized = true;
        }
    }

    void WebViewPage::TabView_AddButtonClick(TabView const& /*sender*/, IInspectable const& /*args*/)
    {
        AddNewTab(L"https://learn.microsoft.com/", L"New Tab");
    }

    void WebViewPage::TabView_TabCloseRequested(TabView const& /*sender*/, TabViewTabCloseRequestedEventArgs const& args)
    {
        auto tabItem = args.Tab();
        auto tabId = winrt::unbox_value_or<winrt::hstring>(tabItem.Tag(), L"");
        
        if (!tabId.empty())
        {
            CloseTab(tabId);
        }
    }

    void WebViewPage::TabView_SelectionChanged(IInspectable const& sender, SelectionChangedEventArgs const& /*e*/)
    {
        auto tabView = sender.as<TabView>();
        if (auto selectedItem = tabView.SelectedItem())
        {
            auto tabItem = selectedItem.as<TabViewItem>();
            auto tabId = winrt::unbox_value_or<winrt::hstring>(tabItem.Tag(), L"");
            if (!tabId.empty())
            {
                SwitchToTab(tabId);
            }
        }
    }

    void WebViewPage::DocNavigation_ItemInvoked(NavigationView const& /*sender*/, NavigationViewItemInvokedEventArgs const& args)
    {
        auto invokedItem = args.InvokedItemContainer();
        if (invokedItem)
        {
            auto sectionId = winrt::unbox_value_or<winrt::hstring>(invokedItem.Tag(), L"");
            if (!sectionId.empty())
            {
                NavigateToSection(sectionId);
            }
        }
    }

    void WebViewPage::SectionSearch_TextChanged(IInspectable const& sender, TextChangedEventArgs const& /*e*/)
    {
        auto textBox = sender.as<TextBox>();
        if (textBox)
        {
            auto searchText = textBox.Text();
            FilterNavigationItems(searchText);
        }
    }

    void WebViewPage::FavoriteButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        throw winrt::hresult_not_implemented();
    }

    void WebViewPage::RefreshPage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        throw winrt::hresult_not_implemented();
    }

    void WebViewPage::OpenInBrowser_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        throw winrt::hresult_not_implemented();
    }

    void WebViewPage::SharePage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        throw winrt::hresult_not_implemented();
    }

    void WebViewPage::AddNewTab(hstring const& url, hstring const& title)
    {
        auto tabData = std::make_shared<TabData>();
        tabData->Id = GenerateTabId();
        tabData->Title = title;
        tabData->Url = url;
        tabData->IsLoading = true;
        tabData->WebView = CreateWebView();

        // 创建TabViewItem
        auto tabItem = CreateTabItem(tabData);
        
        // 添加到集合
        m_tabs.push_back(tabData);
        WebTabView().TabItems().Append(tabItem);
        
        // 选中新标签页
        WebTabView().SelectedItem(tabItem);
        m_currentTabId = tabData->Id;
        
        // 加载网页
        LoadWebPageAsync(tabData);
    }

    void WebViewPage::CloseTab(hstring const& tabId)
    {
        auto it = std::find_if(m_tabs.begin(), m_tabs.end(),
            [&tabId](const std::shared_ptr<TabData>& tab) { return tab->Id == tabId; });
        
        if (it != m_tabs.end())
        {
            // 从TabView中移除对应的TabViewItem
            auto tabItems = WebTabView().TabItems();
            for (uint32_t i = 0; i < tabItems.Size(); ++i)
            {
                auto tabItem = tabItems.GetAt(i).as<TabViewItem>();
                auto itemTabId = winrt::unbox_value_or<hstring>(tabItem.Tag(), L"");
                if (itemTabId == tabId)
                {
                    tabItems.RemoveAt(i);
                    break;
                }
            }
            
            m_tabs.erase(it);
            
            // 如果关闭的是当前标签页且还有其他标签页，切换到第一个
            if (m_currentTabId == tabId && !m_tabs.empty())
            {
                SwitchToTab(m_tabs[0]->Id);
            }
            else if (m_tabs.empty())
            {
                // 没有标签页了，清空目录
                DocNavigationView().MenuItems().Clear();
                m_currentTabId = L"";
            }
        }
    }

    void WebViewPage::SwitchToTab(hstring const& tabId)
    {
        auto tabData = GetTabDataById(tabId);
        if (tabData)
        {
            m_currentTabId = tabId;
            // 更新目录导航
            PopulateNavigationView(tabData);
        }
    }

    void WebViewPage::UpdateTabTitle(hstring const& tabId, hstring const& title)
    {
        auto tabData = GetTabDataById(tabId);
        if (tabData)
        {
            tabData->Title = title;
            
            // 更新TabViewItem的标题
            auto tabItems = WebTabView().TabItems();
            for (uint32_t i = 0; i < tabItems.Size(); ++i)
            {
                auto tabItem = tabItems.GetAt(i).as<TabViewItem>();
                auto itemTabId = winrt::unbox_value_or<hstring>(tabItem.Tag(), L"");
                if (itemTabId == tabId)
                {
                    tabItem.Header(winrt::box_value(title));
                    break;
                }
            }
        }
    }

    void WebViewPage::LoadDocumentStructure(hstring const& url)
    {
        // 根据URL生成示例目录结构
        auto navView = DocNavigationView();
        navView.MenuItems().Clear();
        
        // 添加一些示例导航项
        auto homeItem = NavigationViewItem();
        homeItem.Content(winrt::box_value(L"概述"));
        homeItem.Tag(winrt::box_value(L"overview"));
        homeItem.Icon(FontIcon());
        homeItem.Icon().as<FontIcon>().Glyph(L"\uE80F");
        navView.MenuItems().Append(homeItem);
        
        auto quickStartItem = NavigationViewItem();
        quickStartItem.Content(winrt::box_value(L"快速开始"));
        quickStartItem.Tag(winrt::box_value(L"quickstart"));
        quickStartItem.Icon(FontIcon());
        quickStartItem.Icon().as<FontIcon>().Glyph(L"\uE7C1");
        navView.MenuItems().Append(quickStartItem);
        
        auto tutorialItem = NavigationViewItem();
        tutorialItem.Content(winrt::box_value(L"教程"));
        tutorialItem.Tag(winrt::box_value(L"tutorial"));
        tutorialItem.Icon(FontIcon());
        tutorialItem.Icon().as<FontIcon>().Glyph(L"\uE8F1");
        navView.MenuItems().Append(tutorialItem);
        
        auto apiItem = NavigationViewItem();
        apiItem.Content(winrt::box_value(L"API 参考"));
        apiItem.Tag(winrt::box_value(L"api"));
        apiItem.Icon(FontIcon());
        apiItem.Icon().as<FontIcon>().Glyph(L"\uE943");
        navView.MenuItems().Append(apiItem);
    }

    void WebViewPage::NavigateToSection(hstring const& sectionId)
    {
        auto currentTab = GetCurrentTabData();
        if (currentTab && currentTab->WebView)
        {
            // 使用JavaScript导航到指定section - 使用WinRT字符串操作
            auto script = winrt::hstring(L"try { var element = document.getElementById('") + 
                         sectionId + 
                         winrt::hstring(L"'); if (element) { element.scrollIntoView({behavior: 'smooth'}); } } catch(e) { console.log('Navigation error:', e); }");
            currentTab->WebView.ExecuteScriptAsync(script);
        }
    }

    void WebViewPage::SavePageState()
    {
        // 保存当前标签页状态到本地存储
        try
        {
            // TODO: 实现状态保存到ApplicationData
        }
        catch (...)
        {
            // 忽略保存错误
        }
    }

    void WebViewPage::RestorePageState()
    {
        // 从本地存储恢复标签页状态
        try
        {
            // TODO: 实现状态恢复
        }
        catch (...)
        {
            // 忽略恢复错误
        }
    }

    TabViewItem WebViewPage::CreateTabItem(std::shared_ptr<TabData> tabData)
    {
        TabViewItem tabItem;
        tabItem.Header(winrt::box_value(tabData->Title));
        tabItem.Tag(winrt::box_value(tabData->Id));
        
        // 设置WebView2作为内容
        tabItem.Content(tabData->WebView);
        
        return tabItem;
    }

    WebView2 WebViewPage::CreateWebView()
    {
        WebView2 webView;
        
        // 配置WebView2事件
        webView.NavigationCompleted([this](auto const& sender, auto const& /*args*/)
        {
            auto webView = sender.as<WebView2>();
            
            // 查找对应的TabData
            for (auto& tabData : m_tabs)
            {
                if (tabData->WebView == webView)
                {
                    tabData->IsLoading = false;
                    
                    // 获取页面标题
                    webView.ExecuteScriptAsync(L"document.title").Completed([this, tabData](auto const& asyncOp, auto const& status)
                    {
                        if (status == AsyncStatus::Completed)
                        {
                            try
                            {
                                auto result = asyncOp.GetResults();
                                // 移除JSON字符串的引号
                                auto title = std::wstring(result);
                                if (title.size() >= 2 && title.front() == L'"' && title.back() == L'"')
                                {
                                    title = title.substr(1, title.size() - 2);
                                }
                                UpdateTabTitle(tabData->Id, hstring(title));
                            }
                            catch (...)
                            {
                                // 忽略标题更新错误
                            }
                        }
                    });
                    
                    // 提取目录结构
                    ExtractTableOfContentsAsync(tabData);
                    break;
                }
            }
        });
        
        return webView;
    }

    IAsyncAction WebViewPage::LoadWebPageAsync(std::shared_ptr<TabData> tabData)
    {
        if (tabData && tabData->WebView)
        {
            try
            {
                Windows::Foundation::Uri uri(tabData->Url);
                tabData->WebView.Source(uri);
            }
            catch (...)
            {
                // 处理无效URL
                tabData->IsLoading = false;
                UpdateTabTitle(tabData->Id, L"Invalid URL");
            }
        }
        co_return;
    }

    std::shared_ptr<TabData> WebViewPage::GetCurrentTabData()
    {
        return GetTabDataById(m_currentTabId);
    }

    std::shared_ptr<TabData> WebViewPage::GetTabDataById(hstring const& tabId)
    {
        auto it = std::find_if(m_tabs.begin(), m_tabs.end(),
            [&tabId](const std::shared_ptr<TabData>& tab) { return tab->Id == tabId; });
        
        return (it != m_tabs.end()) ? *it : nullptr;
    }

    hstring WebViewPage::GenerateTabId()
    {
        return hstring(L"tab_" + std::to_wstring(m_nextTabNumber++));
    }

    void WebViewPage::PopulateNavigationView(std::shared_ptr<TabData> tabData)
    {
        if (!tabData)
            return;
        
        // 为当前页面加载目录结构
        LoadDocumentStructure(tabData->Url);
    }

    void WebViewPage::FilterNavigationItems(hstring const& searchText)
    {
        auto navView = DocNavigationView();
        
        if (searchText.empty())
        {
            // 显示所有项目
            for (auto const& item : navView.MenuItems())
            {
                auto navItem = item.as<NavigationViewItem>();
                navItem.Visibility(Visibility::Visible);
            }
        }
        else
        {
            // 过滤项目 - 使用简单的字符串匹配
            auto searchStr = searchText.c_str();
            
            for (auto const& item : navView.MenuItems())
            {
                auto navItem = item.as<NavigationViewItem>();
                auto content = navItem.Content().as<hstring>();
                auto contentStr = content.c_str();
                
                bool isVisible = wcsstr(contentStr, searchStr) != nullptr;
                navItem.Visibility(isVisible ? Visibility::Visible : Visibility::Collapsed);
            }
        }
    }

    IAsyncAction WebViewPage::ExtractTableOfContentsAsync(std::shared_ptr<TabData> tabData)
    {
        if (!tabData || !tabData->WebView)
            co_return;
        
        try
        {
            // 使用JavaScript提取页面目录
            auto script = L"try { var headers = Array.from(document.querySelectorAll('h1, h2, h3, h4, h5, h6')); headers.map(h => ({text: h.textContent, id: h.id, level: parseInt(h.tagName.substring(1))})); } catch(e) { []; }";
            
            auto result = co_await tabData->WebView.ExecuteScriptAsync(script);
            
            // TODO: 解析结果并更新导航视图
            // 这里可以解析JavaScript返回的JSON数据，并动态创建导航项
        }
        catch (...)
        {
            // 如果JavaScript执行失败，使用默认的目录结构
            LoadDocumentStructure(tabData->Url);
        }
    }
}
