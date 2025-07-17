#pragma once

#include "SettingsPage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>
    {
        SettingsPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
