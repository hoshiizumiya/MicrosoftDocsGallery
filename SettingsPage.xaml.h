#pragma once

#include "SettingsPage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>
    {
        SettingsPage();

    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
