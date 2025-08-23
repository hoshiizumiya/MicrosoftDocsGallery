#pragma once

#include "NavigationView.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct NavigationView : NavigationViewT<NavigationView>
    {
        NavigationView()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct NavigationView : NavigationViewT<NavigationView, implementation::NavigationView>
    {
    };
}
