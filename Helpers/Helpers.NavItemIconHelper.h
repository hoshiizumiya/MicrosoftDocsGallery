#pragma once

#include "Helpers/NavItemIconHelper.g.h"

namespace winrt::MicrosoftDocsGallery::Helpers::implementation
{
    struct NavItemIconHelper : NavItemIconHelperT<NavItemIconHelper>
    {
        NavItemIconHelper() = default;

        static winrt::Microsoft::UI::Xaml::DependencyProperty SelectedIconProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty ShowNotificationDotProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty UnselectedIconProperty();
        static winrt::Microsoft::UI::Xaml::DependencyProperty StaticIconVisibilityProperty();

        static winrt::Windows::Foundation::IInspectable GetSelectedIcon(winrt::Microsoft::UI::Xaml::DependencyObject const& obj);
        static void SetSelectedIcon(winrt::Microsoft::UI::Xaml::DependencyObject const& obj, winrt::Windows::Foundation::IInspectable const& value);
        static bool GetShowNotificationDot(winrt::Microsoft::UI::Xaml::DependencyObject const& obj);
        static void SetShowNotificationDot(winrt::Microsoft::UI::Xaml::DependencyObject const& obj, bool value);
        static winrt::Windows::Foundation::IInspectable GetUnselectedIcon(winrt::Microsoft::UI::Xaml::DependencyObject const& obj);
        static void SetUnselectedIcon(winrt::Microsoft::UI::Xaml::DependencyObject const& obj, winrt::Windows::Foundation::IInspectable const& value);
        static winrt::Microsoft::UI::Xaml::Visibility GetStaticIconVisibility(winrt::Microsoft::UI::Xaml::DependencyObject const& obj);
        static void SetStaticIconVisibility(winrt::Microsoft::UI::Xaml::DependencyObject const& obj, winrt::Microsoft::UI::Xaml::Visibility const& value);

    };
}

namespace winrt::MicrosoftDocsGallery::Helpers::factory_implementation
{
    struct NavItemIconHelper : NavItemIconHelperT<NavItemIconHelper, implementation::NavItemIconHelper> {};
}
