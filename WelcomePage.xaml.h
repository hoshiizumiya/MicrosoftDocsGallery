#pragma once

#include "WelcomePage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct WelcomePage : WelcomePageT<WelcomePage>
    {
        WelcomePage();

        void StartButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct WelcomePage : WelcomePageT<WelcomePage, implementation::WelcomePage>
    {
    };
}
