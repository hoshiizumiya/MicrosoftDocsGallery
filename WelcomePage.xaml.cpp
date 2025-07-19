#include "pch.h"
#include "WelcomePage.xaml.h"
#if __has_include("WelcomePage.g.cpp")
#include "WelcomePage.g.cpp"
#endif
#include "MainWindow.xaml.h"
using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MicrosoftDocsGallery::implementation
{
	WelcomePage::WelcomePage()
	{

	}

    void WelcomePage::StartButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        // 获取主窗口并跳转到首页
        auto window = Microsoft::UI::Xaml::Window::Current().try_as<winrt::MicrosoftDocsGallery::MainWindow>();
        if (window)
        {
            window.as<winrt::MicrosoftDocsGallery::implementation::MainWindow>()->openHomePage();
        }
    }
}
