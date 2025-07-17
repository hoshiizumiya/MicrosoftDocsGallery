#pragma once

#include "WebViewPage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct WebViewPage : WebViewPageT<WebViewPage>
    {
        WebViewPage();

    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct WebViewPage : WebViewPageT<WebViewPage, implementation::WebViewPage>
    {
    };
}
