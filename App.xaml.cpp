#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Windows.Globalization.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/microsoft.ui.interop.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

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
        SetWindowStyle(window);
        window.Activate();
        // 首次启动显示欢迎页
        auto mainWindow = window.as<winrt::MicrosoftDocsGallery::implementation::MainWindow>();
        if (mainWindow)
        {
            mainWindow->openWelcomePage();
        }
    }


    Windows::Foundation::IAsyncAction App::SetWindowStyle(Window window)
    {
        auto appWindow = window.AppWindow();

        //BUG The icon can not be set!I dont know why?????
        //appWindow.SetIcon(L"ms-appx:///Assets/icon.ico");
        co_await SetIconAsync(appWindow);

        //this->Title(L"Microsoft Docs Gallery");
        //this->Title(winrt::Windows::ApplicationModel::Package::Current().DisplayName());

        window.ExtendsContentIntoTitleBar(true);
        if (appWindow)
        {
            auto titleBar = appWindow.TitleBar();
            if (titleBar)
            {
                titleBar.PreferredHeightOption(winrt::Microsoft::UI::Windowing::TitleBarHeightOption::Tall);
            }
        }

    }

    Windows::Foundation::IAsyncAction App::SetIconAsync(Microsoft::UI::Windowing::AppWindow window)
    {
        using namespace Windows::Storage;
        using namespace Windows::Foundation;
        using namespace Microsoft::UI::Windowing;

        Uri uri{ L"ms-appx:///Assets/icon.ico" };
        try
        {
            StorageFile storageFile = co_await StorageFile::GetFileFromApplicationUriAsync(uri);
            if (storageFile)
            {
                window.SetIcon(storageFile.Path());
            }
        }
        catch (...)
        {
            // Failed to load icon, use default or ignore
        }

    }
}
