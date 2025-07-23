#pragma once

#include "MainWindow.g.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Windowing.h>

namespace winrt::MicrosoftDocsGallery::implementation
{
	struct MainWindow : MainWindowT<MainWindow>
	{
	public:
		MainWindow();

		void openHomePage();
		void openFundamentalsPage();
		void openSettingsPage();
		void openWebViewPage(hstring const& url);

		void NavigationView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
		void mainFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
		void nav_BackRequested(winrt::Microsoft::UI::Xaml::Controls::TitleBar const& sender, winrt::Windows::Foundation::IInspectable const& args);
		void SearchBox_QuerySubmitted(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs const& args);
		void SearchBox_TextChanged(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const& args);
		winrt::Windows::Foundation::IAsyncAction SetIconAsync(winrt::Microsoft::UI::Windowing::AppWindow window);
		void InitWindowStyle(winrt::Microsoft::UI::Xaml::Window const& window);
	};
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
