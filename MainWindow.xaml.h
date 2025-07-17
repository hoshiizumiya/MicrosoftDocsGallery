#pragma once

#include "MainWindow.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
	struct MainWindow : MainWindowT<MainWindow>
	{
	private:
		void openHomePage();
		void openFundamentalsPage();
		void openSettingsPage();
	public:
		MainWindow();

		void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

		void NavigationView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
		void mainFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
		void nav_BackRequested(winrt::Microsoft::UI::Xaml::Controls::TitleBar const& sender, winrt::Windows::Foundation::IInspectable const& args);
		void SearchBox_QuerySubmitted(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs const& args);
		void SearchBox_TextChanged(winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const& args);
	};
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
