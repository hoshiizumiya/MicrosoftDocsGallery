#pragma once

#include "SettingsPage.g.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>
    {
        SettingsPage();
        void LanguageComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
