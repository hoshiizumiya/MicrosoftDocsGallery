#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::MicrosoftDocsGallery::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        SetTitleBar(AppTitleBar());
        
        // 初始化窗口样式
        InitWindowStyle(*this);
        
        // 加载收藏夹
        LoadFavorites();
        
        // 恢复窗口状态
        RestoreWindowState();
        
        // 窗口关闭时保存状态
        this->Closed([this](auto const&, auto const&) { 
            this->SaveWindowState(); 
        });
    }

    void MainWindow::openHomePage()
    {
        mainFrame().Navigate(xaml_typename<HomePage>());
        UpdateNavigationSelection(L"home");
    }

    void MainWindow::openFundamentalsPage()
    {
        mainFrame().Navigate(xaml_typename<FundamentalsPage>());
        UpdateNavigationSelection(L"fundamentals");
    }

    void MainWindow::openSettingsPage()
    {
        mainFrame().Navigate(xaml_typename<SettingsPage>());
        UpdateNavigationSelection(L"Settings");
    }

    void MainWindow::openWebViewPage(hstring const& url)
    {
        mainFrame().Navigate(xaml_typename<WebViewPage>(), box_value(url));
        UpdateNavigationSelection(L"webview");
    }

    void MainWindow::openFavoritesPage()
    {
        mainFrame().Navigate(xaml_typename<FavoritesPage>());
        UpdateNavigationSelection(L"fav");
    }

    void MainWindow::SaveWindowState()
    {
        try
        {
            auto appWindow = this->AppWindow();
            if (appWindow)
            {
                // 简化窗口状态保存，移除有问题的API调用
                // TODO: 保存到ApplicationData
                // 这里可以将窗口位置和大小保存到本地设置
            }
        }
        catch (...)
        {
            // 忽略保存错误
        }
    }

    void MainWindow::RestoreWindowState()
    {
        try
        {
            // TODO: 从ApplicationData恢复
            // 这里可以从本地设置恢复窗口位置和大小
        }
        catch (...)
        {
            // 忽略恢复错误，使用默认窗口大小
        }
    }

    void MainWindow::AddToFavorites(hstring const& url, hstring const& title)
    {
        // 检查是否已存在
        if (!IsInFavorites(url))
        {
            // 添加到收藏夹列表
            m_favoriteUrls.Append(url);
            m_favoriteTitles.Append(title);
            
            // 保存到本地存储
            SaveFavorites();
            
            // 更新导航视图中的收藏夹项
            UpdateFavoritesInNavigation();
        }
    }

    void MainWindow::RemoveFromFavorites(hstring const& url)
    {
        // 查找要删除的项目
        for (uint32_t i = 0; i < m_favoriteUrls.Size(); ++i)
        {
            if (m_favoriteUrls.GetAt(i) == url)
            {
                m_favoriteUrls.RemoveAt(i);
                m_favoriteTitles.RemoveAt(i);
                
                // 保存到本地存储
                SaveFavorites();
                
                // 更新导航视图
                UpdateFavoritesInNavigation();
                break;
            }
        }
    }

    bool MainWindow::IsInFavorites(hstring const& url)
    {
        for (uint32_t i = 0; i < m_favoriteUrls.Size(); ++i)
        {
            if (m_favoriteUrls.GetAt(i) == url)
            {
                return true;
            }
        }
        return false;
    }

    void MainWindow::ShowSearchSuggestions(winrt::hstring const& query)
    {
        throw winrt::hresult_not_implemented();
    }

    void MainWindow::NavigationView_ItemInvoked(NavigationView const& /*sender*/, NavigationViewItemInvokedEventArgs const& args)
    {
        hstring tag;

        if (args.InvokedItemContainer())
        {
            tag = unbox_value<hstring>(args.InvokedItemContainer().Tag());
        }
        
        if (tag == L"home")
            openHomePage();
        else if (tag == L"fundamentals")
            openFundamentalsPage();
        else if (tag == L"webview")
            openWebViewPage(L"https://learn.microsoft.com/windows/apps/winui/");
        else if (tag == L"fav")
        {
            // 打开收藏夹页面
            openFavoritesPage();
        }
        else if (tag == L"Settings")
            openSettingsPage();
        else if (tag == L"Contribute")
        {
            openWebViewPage(L"https://github.com/hoshiizumiya/MicrosoftDocsGallery");
        }
        else if (tag.size() > 4)
        {
            // 处理特定收藏夹项目点击 - 使用WinRT字符串操作
            auto tagStr = tag.c_str();
            if (wcslen(tagStr) > 4)
            {
                // 检查前缀
                if (wcsncmp(tagStr, L"fav_", 4) == 0)
                {
                    // 提取URL部分（跳过前4个字符"fav_"）
                    auto url = winrt::hstring(tagStr + 4);
                    openWebViewPage(url);
                }
            }
        }
    }

    void MainWindow::mainFrame_Navigated(IInspectable const& /*sender*/, Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        // 更新导航选择状态
        try
        {
            // 更新返回按钮显示
            if (this->mainFrame().CanGoBack())
            {
                this->HeadLogo().Margin(Thickness{ 0,0,8,0 });
            }
            else
            {
                this->HeadLogo().Margin(Thickness{ 16,0,0,0 });
            }
        }
        catch (...)
        {
            // 忽略错误
        }
    }

    void MainWindow::nav_BackRequested(IInspectable const& /*sender*/, IInspectable const& /*args*/)
    {
        if (mainFrame().CanGoBack())
        {
            mainFrame().GoBack();
        }
    }

    void MainWindow::SearchBox_QuerySubmitted(AutoSuggestBox const& sender, AutoSuggestBoxQuerySubmittedEventArgs const& /*args*/)
    {
        auto query = sender.Text();
        if (!query.empty())
        {
            // 构建搜索URL - 使用WinRT字符串操作
            auto searchUrl = winrt::hstring(L"https://learn.microsoft.com/search/?terms=") + query;
            openWebViewPage(searchUrl);
        }
    }

    void MainWindow::SearchBox_TextChanged(AutoSuggestBox const& sender, AutoSuggestBoxTextChangedEventArgs const& args)
    {
        // 只在用户输入时提供建议
        if (args.Reason() == AutoSuggestionBoxTextChangeReason::UserInput)
        {
            auto query = sender.Text();
            AddSearchSuggestions(query);
        }
    }

    IAsyncAction MainWindow::SetIconAsync(Microsoft::UI::Windowing::AppWindow window)
    {
        if (!window) 
            co_return;
        
        try
        {
            // TODO: 设置应用程序图标
            // 这里可以设置窗口图标
        }
        catch (...)
        {
            // 忽略图标设置错误
        }
        
        co_return;
    }

    void MainWindow::InitWindowStyle(Window const& window)
    {
        window.ExtendsContentIntoTitleBar(true);
        
        auto appWindow = window.AppWindow();
        if (appWindow)
        {
            // 简化标题栏设置，移除有问题的API调用
            // TODO: 设置标题栏属性
        }
        
        // 异步设置图标
        SetIconAsync(appWindow);
    }

    void MainWindow::UpdateNavigationSelection(hstring const& pageTag)
    {
        auto navView = nav();
        
        // 检查主菜单项
        for (auto const& item : navView.MenuItems())
        {
            auto navItem = item.try_as<NavigationViewItem>();
            if (navItem && navItem.Tag().try_as<hstring>() == pageTag)
            {
                navView.SelectedItem(navItem);
                return;
            }
        }
        
        // 检查页脚菜单项
        for (auto const& item : navView.FooterMenuItems())
        {
            auto navItem = item.try_as<NavigationViewItem>();
            if (navItem && navItem.Tag().try_as<hstring>() == pageTag)
            {
                navView.SelectedItem(navItem);
                return;
            }
        }
    }

    void MainWindow::LoadFavorites()
    {
        if (m_favoritesLoaded)
            return;
        
        try
        {
            // 简化实现，使用默认收藏夹
            m_favoriteUrls.Append(L"https://learn.microsoft.com/");
            m_favoriteTitles.Append(L"Microsoft Learn");
            
            m_favoriteUrls.Append(L"https://learn.microsoft.com/windows/apps/winui/");
            m_favoriteTitles.Append(L"WinUI 3 文档");
            
            m_favoriteUrls.Append(L"https://learn.microsoft.com/dotnet/");
            m_favoriteTitles.Append(L".NET 文档");
            
            UpdateFavoritesInNavigation();
            m_favoritesLoaded = true;
        }
        catch (...)
        {
            // 忽略加载错误，使用默认收藏夹
        }
    }

    void MainWindow::SaveFavorites()
    {
        try
        {
            // TODO: 实现收藏夹保存功能
            // 这里可以将收藏夹序列化并保存到本地存储
        }
        catch (...)
        {
            // 忽略保存错误
        }
    }

    void MainWindow::UpdateFavoritesInNavigation()
    {
        // 这里可以动态更新导航视图中的收藏夹项
        // 由于需要访问XAML中的导航项，这里简化处理
        // 在实际应用中，可以通过FindName查找收藏夹容器并动态添加项目
    }

    void MainWindow::AddSearchSuggestions(hstring const& query)
    {
        if (query.empty())
        {
            SearchBox().ItemsSource(nullptr);
            return;
        }
        
        // 创建搜索建议列表
        auto suggestions = winrt::single_threaded_vector<winrt::Windows::Foundation::IInspectable>();
        
        // 添加一些预定义的搜索建议 - 使用WinRT集合
        auto commonSearches = winrt::single_threaded_vector<winrt::hstring>();
        commonSearches.Append(L"WinUI 3 教程");
        commonSearches.Append(L"C# 基础");
        commonSearches.Append(L".NET Core");
        commonSearches.Append(L"XAML 控件");
        commonSearches.Append(L"Azure 服务");
        commonSearches.Append(L"Visual Studio");
        commonSearches.Append(L"Entity Framework");
        commonSearches.Append(L"ASP.NET Core");
        
        auto queryLower = query.c_str();
        // 简化搜索逻辑，避免复杂的字符串转换
        for (auto const& search : commonSearches)
        {
            // 简单的包含检查
            if (search.size() >= query.size())
            {
                auto searchStr = search.c_str();
                if (wcsstr(searchStr, queryLower) != nullptr)
                {
                    suggestions.Append(winrt::box_value(search));
                }
            }
        }
        
        // 添加收藏夹中匹配的项目
        for (uint32_t i = 0; i < m_favoriteTitles.Size(); ++i)
        {
            auto title = m_favoriteTitles.GetAt(i);
            auto titleStr = title.c_str();
            if (wcsstr(titleStr, queryLower) != nullptr)
            {
                suggestions.Append(winrt::box_value(title));
            }
        }
        
        SearchBox().ItemsSource(suggestions);
    }

    void MainWindow::InitializeApplication()
    {
		// todo: 初始化应用程序设置
    }
    
    void MainWindow::SetNavigationSelection(hstring const& pageTag)
    {
        UpdateNavigationSelection(pageTag);
    }
}