#pragma once

#include "WelcomePage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct WelcomePage : WelcomePageT<WelcomePage>
    {
        WelcomePage();

    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct WelcomePage : WelcomePageT<WelcomePage, implementation::WelcomePage>
    {
    };
}
