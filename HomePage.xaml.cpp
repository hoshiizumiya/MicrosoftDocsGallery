#include "pch.h"
#include "HomePage.xaml.h"
#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;

namespace winrt::MicrosoftDocsGallery::implementation
{
    HomePage::HomePage()
    {
        InitializeComponent();
        InitializeViewModel();
    }

    void HomePage::InitializeViewModel()
    {
        // 创建 ViewModel 实例
        m_viewModel = winrt::make<ViewModels::implementation::HomePageViewModel>();
        
        // 设置数据上下文，使 XAML 绑定生效
        this->DataContext(m_viewModel);

        // 订阅 ViewModel 事件
        m_navigationRequestedToken = m_viewModel.NavigationRequested({ this, &HomePage::OnNavigationRequested });

        // 异步初始化 ViewModel
        m_viewModel.InitializeAsync();
    }

    void HomePage::Page_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        // 在 MVVM 模式中，Page_Loaded 事件处理应该最小化
        // 大部分逻辑应该在 ViewModel 的 InitializeAsync 中处理
        UpdateNavigationSelection();
    }

    void HomePage::OnNavigationRequested(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::hstring const& destination)
    {
        // 处理来自 ViewModel 的导航请求
        // 在WinUI 3中，通过XamlRoot获取主窗口
        auto xamlRoot = this->XamlRoot();
        if (xamlRoot)
        {
            auto content = xamlRoot.Content();
            if (content)
            {
                auto mainWindow = content.try_as<MicrosoftDocsGallery::MainWindow>();
                if (mainWindow)
                {
                    if (destination == L"GoBack")
                    {
                        // 处理返回导航
                        // 这里可以调用主窗口的返回方法
                    }
                    else
                    {
                        // 导航到指定URL
                        mainWindow.openWebViewPage(destination);
                    }
                }
            }
        }
    }

    void HomePage::UpdateNavigationSelection()
    {
        // 在WinUI 3中，通过页面的Parent层次查找MainWindow
        auto parent = this->Parent();
        while (parent)
        {
            auto mainWindow = parent.try_as<MicrosoftDocsGallery::MainWindow>();
            if (mainWindow)
            {
                mainWindow.SetNavigationSelection(L"home");
                break;
            }
            
            auto parentElement = parent.try_as<FrameworkElement>();
            if (parentElement)
            {
                parent = parentElement.Parent();
            }
            else
            {
                break;
            }
        }
    }
}
