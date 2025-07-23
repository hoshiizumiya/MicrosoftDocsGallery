#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Windows.UI.Xaml.Interop.h>
#include "winrt/Microsoft.UI.Xaml.Controls.h"
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MicrosoftDocsGallery::implementation
{
	MainWindow::MainWindow()
	{
		InitializeComponent();
		SetTitleBar(AppTitleBar());
	}

	//void NavigateToPage()
	void MainWindow::openHomePage()
	{
		mainFrame().Navigate(xaml_typename<HomePage>());
	}
	void MainWindow::openFundamentalsPage()
	{
		mainFrame().Navigate(xaml_typename<FundamentalsPage>());

	}
	void MainWindow::openSettingsPage()
	{
		mainFrame().Navigate(xaml_typename<SettingsPage>());

	}
	void MainWindow::openWebViewPage(hstring const& url)
	{
		mainFrame().Navigate(xaml_typename<WebViewPage>(), box_value(url));
	}

	void MainWindow::NavigationView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
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
		else if (tag == L"Settings")
			openSettingsPage();
		else if (tag == L"Contribute")
		{
			openWebViewPage(L"https://github.com/hoshiizumiya/MicrosoftDocsGallery");
		}
		// 设置导航栏高亮
		for (auto const& item : sender.MenuItems())
		{
			auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
			if (navItem && navItem.Tag().try_as<hstring>() == tag)
			{
				sender.SelectedItem(navItem);
				break;
			}
		}
		for (auto const& item : sender.FooterMenuItems())
		{
			auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
			if (navItem && navItem.Tag().try_as<hstring>() == tag)
			{
				sender.SelectedItem(navItem);
				break;
			}
		}
	}

	void MainWindow::mainFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
	{
		// 动态高亮导航栏当前项
		auto navView = nav();
		hstring pageTag;
		auto pageType = e.SourcePageType().Name;
		if (pageType == L"HomePage") pageTag = L"home";
		else if (pageType == L"FundamentalsPage") pageTag = L"fundamentals";
		else if (pageType == L"WebViewPage") pageTag = L"webview";
		else if (pageType == L"WelcomePage") pageTag = L"welcome";
		else if (pageType == L"SettingsPage") pageTag = L"Settings";
		for (auto const& item : navView.MenuItems())
		{
			auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
			if (navItem && navItem.Tag().try_as<hstring>() == pageTag)
			{
				navView.SelectedItem(navItem);
				break;
			}
		}
		for (auto const& item : navView.FooterMenuItems())
		{
			auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
			if (navItem && navItem.Tag().try_as<hstring>() == pageTag)
			{
				navView.SelectedItem(navItem);
				break;
			}
		}
		// 只设置 HeadLogo 的 Margin，不再手动设置 IsBackButtonVisible
		if (mainFrame().CanGoBack())
		{
			HeadLogo().Margin(Thickness{ 0,0,8,0 });
		}
		else
		{
			HeadLogo().Margin(Thickness{ 16,0,0,0 });
		}
	}

	void MainWindow::nav_BackRequested(winrt::Microsoft::UI::Xaml::Controls::TitleBar const& sender, Windows::Foundation::IInspectable const& args)
	{
		if (mainFrame().CanGoBack())
		{
			mainFrame().GoBack();
			// 强制刷新导航栏选中项
			if (mainFrame().Content())
			{
				// 通过 RTTI 获取当前页面类型名
				std::wstring_view typeName = winrt::to_hstring(winrt::get_class_name(mainFrame().Content()));
				hstring pageTag;
				if (typeName.find(L"HomePage") != std::wstring_view::npos) pageTag = L"home";
				else if (typeName.find(L"FundamentalsPage") != std::wstring_view::npos) pageTag = L"fundamentals";
				else if (typeName.find(L"WebViewPage") != std::wstring_view::npos) pageTag = L"webview";
				else if (typeName.find(L"WelcomePage") != std::wstring_view::npos) pageTag = L"welcome";
				else if (typeName.find(L"SettingsPage") != std::wstring_view::npos) pageTag = L"Settings";
				auto navView = nav();
				for (auto const& item : navView.MenuItems())
				{
					auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
					if (navItem && navItem.Tag().try_as<hstring>() == pageTag)
					{
						navView.SelectedItem(navItem);
						break;
					}
				}
				for (auto const& item : navView.FooterMenuItems())
				{
					auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
					if (navItem && navItem.Tag().try_as<hstring>() == pageTag)
					{
						navView.SelectedItem(navItem);
						break;
					}
				}
			}
		}
	}

}

void winrt::MicrosoftDocsGallery::implementation::MainWindow::SearchBox_QuerySubmitted(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs const& args)
{

}

void winrt::MicrosoftDocsGallery::implementation::MainWindow::SearchBox_TextChanged(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const& args)
{

}

winrt::Windows::Foundation::IAsyncAction winrt::MicrosoftDocsGallery::implementation::MainWindow::SetIconAsync(winrt::Microsoft::UI::Windowing::AppWindow window)
{
	if (!window) co_return;
	using namespace Windows::Storage;
	using namespace Windows::Foundation;
	using namespace Microsoft::UI::Windowing;
	Uri uri{ L"/Assets/msdn.ico" };
	try
	{
		StorageFile storageFile = co_await StorageFile::GetFileFromApplicationUriAsync(uri);
		if (storageFile)
		{
			window.SetIcon(storageFile.Path());
			OutputDebugString((L"Set icon path: " + storageFile.Path() + L"\n").c_str());

		}
	}
	catch (...)
	{
		// Failed to load icon, use default or ignore
		OutputDebugString(L"Failed to set icon.\n");
	}
}


void winrt::MicrosoftDocsGallery::implementation::MainWindow::InitWindowStyle(winrt::Microsoft::UI::Xaml::Window const& window)
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