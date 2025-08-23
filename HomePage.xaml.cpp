#include "pch.h"
#include "HomePage.xaml.h"
#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif
#include "ViewModels\HomePageViewModel.h"

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
        // 简化 ViewModel 初始化，避免复杂的 WinRT 类型问题
        // 这里可以后续添加真正的 ViewModel 初始化
        
        // 暂时使用简单的数据上下文设置
        this->DataContext(winrt::box_value(L"HomePage"));
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

    void HomePage::Animated_GotItem(winrt::Windows::Foundation::IInspectable const&/* sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&/* e*/)
    {
        throw winrt::hresult_not_implemented();
    }

    void HomePage::QuickAccess_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        throw winrt::hresult_not_implemented();
    }


    void HomePage::OpenWinUIDocs_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
    {
        throw winrt::hresult_not_implemented();
    }
}





