#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Windows.Globalization.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Windowing.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Globalization;


void InitWindowStyle(winrt::Microsoft::UI::Xaml::Window const& window)
{
    window.ExtendsContentIntoTitleBar(true);
    auto appWindow = window.AppWindow();
    if (appWindow)
    {
        auto titleBar = appWindow.TitleBar();
        if (titleBar)
        {
            titleBar.PreferredHeightOption(winrt::Microsoft::UI::Windowing::TitleBarHeightOption::Tall);
        }
        // 设置窗口图标（异步 fire-and-forget）
        auto mainWindow = window.as<winrt::MicrosoftDocsGallery::MainWindow>();
        if (mainWindow)
        {
            mainWindow.SetIconAsync(appWindow);
        }
    }
}

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
        InitWindowStyle(window);
        window.Activate();
    }
}
