#pragma once

#include "HomePage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        HomePage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}
