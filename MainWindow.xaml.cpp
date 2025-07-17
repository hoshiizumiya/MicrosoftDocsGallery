#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Windows.UI.Xaml.Interop.h>
#include "winrt/Microsoft.UI.Xaml.Controls.h"


using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MicrosoftDocsGallery::implementation
{
	MainWindow::MainWindow()
	{
		//InitializeComponent();
		// 获取XAML中x:Name为 AppTitleBar 的元素，并设置为TitleBar
		//SetTitleBar(UIElement::AppTitleBar); Wrong usage
		//SetTitleBar(AppTitleBar());


	}
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

	void MainWindow::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
	}

	void MainWindow::NavigationView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
	{
		if (args.IsSettingsInvoked())//方法指示 InvokedItem 是否为 Settings 的菜单项
		{
			//打开设置界面 open setting page
			openSettingsPage();
		}
		else
		{
			//为确定哪个页面会被请求，已给每个页面设置独一无二的的 Tag 
			//Tag 在 xaml 中被自动解析并装箱成一个 IInspectable 属性。IInspectable 是所有 WinRT 接口的根属性，使用拆箱函数转化为目标 String 类型字符串
			// 这样设计主要为了语言互操作性 C# 的 object、JS 的 Object、C++ 的 IInspectable 可以相互转换
			//使用 unbox_value<T>()	从 IInspectable 提取具体类型
			hstring tag = unbox_value<hstring>(args.InvokedItemContainer().Tag());
			if (tag == L"home")
				openHomePage();
			else if (tag == L"fundamentals")
			{
				openFundamentalsPage();
			}

		}
	}

	void MainWindow::mainFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
	{
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
