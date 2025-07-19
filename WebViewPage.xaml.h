#pragma once

#include "WebViewPage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct WebViewPage : WebViewPageT<WebViewPage>
    {
        WebViewPage();

        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct WebViewPage : WebViewPageT<WebViewPage, implementation::WebViewPage>
    {
    };
}
