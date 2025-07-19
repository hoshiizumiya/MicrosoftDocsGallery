#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Windows.UI.Xaml.Interop.h>
#include "winrt/Microsoft.UI.Xaml.Controls.h"
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Windows.Graphics.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MicrosoftDocsGallery::implementation
{
	MainWindow::MainWindow()
	{
		SetWindowStyle();
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
	void MainWindow::openWelcomePage()
	{
		mainFrame().Navigate(xaml_typename<WelcomePage>());
	}

	void MainWindow::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		openHomePage();
	}

	void MainWindow::NavigationView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
	{
		hstring tag;
		if (args.IsSettingsInvoked())
		{
			openSettingsPage();
			// 设置导航栏高亮
			for (auto const& item : sender.FooterMenuItems()) {
				auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
				if (navItem && navItem.Tag().try_as<hstring>() == L"Settings") {
					sender.SelectedItem(navItem);
					break;
				}
			}
			return;
		}
		else if (args.InvokedItemContainer())
		{
			tag = unbox_value<hstring>(args.InvokedItemContainer().Tag());
		}
		if (tag == L"home")
			openHomePage();
		else if (tag == L"fundamentals")
			openFundamentalsPage();
		else if (tag == L"webview")
			openWebViewPage(L"https://learn.microsoft.com/windows/apps/winui/");
		else if (tag == L"welcome")
			openWelcomePage();
		else if (tag == L"Settings")
			openSettingsPage();
		else if (tag==L"Contribute")
		{
			openWebViewPage(L"https://github.com/hoshiizumiya/MicrosoftDocsGallery");
		}
		// 设置导航栏高亮
		for (auto const& item : sender.MenuItems()) {
			auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
			if (navItem && navItem.Tag().try_as<hstring>() == tag) {
				sender.SelectedItem(navItem);
				break;
			}
		}
		for (auto const& item : sender.FooterMenuItems()) {
			auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
			if (navItem && navItem.Tag().try_as<hstring>() == tag) {
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
		for (auto const& item : navView.MenuItems()) {
			auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
			if (navItem && navItem.Tag().try_as<hstring>() == pageTag) {
				navView.SelectedItem(navItem);
				break;
			}
		}
		for (auto const& item : navView.FooterMenuItems()) {
			auto navItem = item.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
			if (navItem && navItem.Tag().try_as<hstring>() == pageTag) {
				navView.SelectedItem(navItem);
				break;
			}
		}
		// ...原有代码...
		if (AppTitleBar().IsBackButtonVisible())
		{
			HeadLogo().Margin(Thickness{0,0,8,0});
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
		}
	}



}

void winrt::MicrosoftDocsGallery::implementation::MainWindow::SearchBox_QuerySubmitted(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs const& args)
{

}

void winrt::MicrosoftDocsGallery::implementation::MainWindow::SearchBox_TextChanged(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const& args)
{

}

void winrt::MicrosoftDocsGallery::implementation::MainWindow::SetWindowStyle()
{
	// 获取XAML中x:Name为 AppTitleBar 的元素，并设置为TitleBar
	SetTitleBar(AppTitleBar());

	//The icon can not be set!I dont konw why?????
	this->AppWindow().SetIcon(L"ms-appx:///Assets/icon.ico");
	//this->Title(L"Microsoft Docs Gallery");
	//this->Title(winrt::Windows::ApplicationModel::Package::Current().DisplayName());

	//TitleBar
	this->ExtendsContentIntoTitleBar(true);
	auto appWindow = this->AppWindow();
	if (appWindow)
	{
		auto titleBar = appWindow.TitleBar();
		if (titleBar)
		{
			titleBar.PreferredHeightOption(winrt::Microsoft::UI::Windowing::TitleBarHeightOption::Tall);
		}
	}
}
