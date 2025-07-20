#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Windows.Globalization.h>
#include <winrt/Microsoft.UI.Xaml.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Globalization;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MicrosoftDocsGallery::implementation
{
    /// <summary>
    /// Initializes the singleton application object.  This is the first line of authored code
    /// executed, and as such is the logical equivalent of main() or WinMain().
    /// </summary>
    App::App()
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([](IInspectable const&, UnhandledExceptionEventArgs const& e)
        {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        });
#endif
        // 设置默认语言为英语
        if (ApplicationLanguages::PrimaryLanguageOverride().empty()) {
            ApplicationLanguages::PrimaryLanguageOverride(L"en-US");
        }
    }

    /// <summary>
    /// Invoked when the application is launched.
    /// </summary>
    /// <param name="e">Details about the launch request and process.</param>
    void App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const& e)
    {
        window = make<MainWindow>();
        window.Activate();
        // 首次启动显示欢迎页
        auto mainWindow = window.as<winrt::MicrosoftDocsGallery::implementation::MainWindow>();
        if (mainWindow)
        {
            mainWindow->openWelcomePage();
        }
    }
}
