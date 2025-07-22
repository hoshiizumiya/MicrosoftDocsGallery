#pragma once

#include "App.xaml.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        Windows::Foundation::IAsyncAction SetWindowStyle(winrt::Microsoft::UI::Xaml::Window);

        Windows::Foundation::IAsyncAction SetIconAsync(Microsoft::UI::Windowing::AppWindow window);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
