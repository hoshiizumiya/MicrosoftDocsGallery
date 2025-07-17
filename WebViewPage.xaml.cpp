#include "pch.h"
#include "WebViewPage.xaml.h"
#if __has_include("WebViewPage.g.cpp")
#include "WebViewPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MicrosoftDocsGallery::implementation
{
	WebViewPage::WebViewPage()
	{
		//如果你写 &OnWindowClosed，编译器会认为你在获取一个全局函数的地址。但 OnWindowClosed 是 MainWindow 类的成员函数，不在全局作用域，所以会报错：“绑定成员函数表达式上的非法操作”。
//this->Closed([this](auto const& sender, auto const& args) { this->WebContentShowPage::OnWindowClosed(sender, args); });
// 掉头发写法
//this->Closed({ this, &WebContentShowPage::OnWindowClosed });
	}
	
	void OnWindowClosed(Windows::Foundation::IInspectable const& sender,
											winrt::Microsoft::UI::Xaml::WindowEventArgs const& args)
	{
		//if (MSDNWebView2() != nullptr)
		//{
		//	MSDNWebView2().Close();}


	}

}
