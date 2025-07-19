#include "pch.h"
#include "WebViewPage.xaml.h"
#if __has_include("WebViewPage.g.cpp")
#include "WebViewPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MicrosoftDocsGallery::implementation
{
	WebViewPage::WebViewPage()
	{
		InitializeComponent();

	}

	void WebViewPage::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
	{
		if (e.Parameter())
		{
			auto url = winrt::unbox_value<winrt::hstring>(e.Parameter());
			if (!url.empty())
			{
				MSDNWebView2().Source(winrt::Windows::Foundation::Uri(url));
			}
		}
	}
}
