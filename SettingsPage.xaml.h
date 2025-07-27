#pragma once

#include "SettingsPage.g.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>
    {
        SettingsPage();
        
        // 事件处理器
        void LanguageComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void DarkModeToggle_Toggled(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void FontSizeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        winrt::Windows::Foundation::IAsyncAction ClearCacheButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        winrt::Windows::Foundation::IAsyncAction ResetSettingsButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        // 辅助方法
        void LoadSettings();
        void SaveSettings();
        void ApplyTheme();
        void ApplyFontSize(double fontSize);
        winrt::Windows::Foundation::IAsyncAction ShowConfirmationDialog(winrt::hstring const& title, winrt::hstring const& content);

        // 设置存储
        bool m_isDarkMode = false;
        double m_fontSize = 16.0;
        winrt::hstring m_language = L"en-US";
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
