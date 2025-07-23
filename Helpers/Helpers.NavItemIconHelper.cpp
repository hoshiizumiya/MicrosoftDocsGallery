#include "pch.h"
#include <winrt/Windows.UI.Xaml.Interop.h>
#include "Helpers.NavItemIconHelper.h"
#include "Helpers.NavItemIconHelper.g.cpp"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;

namespace winrt::MicrosoftDocsGallery::Helpers::implementation
{
    Microsoft::UI::Xaml::DependencyProperty NavItemIconHelper::SelectedIconProperty()
    {
        static Microsoft::UI::Xaml::DependencyProperty prop = Microsoft::UI::Xaml::DependencyProperty::RegisterAttached(
            L"SelectedIcon",
            winrt::xaml_typename<winrt::Windows::Foundation::IInspectable>(),
            winrt::xaml_typename<winrt::MicrosoftDocsGallery::Helpers::NavItemIconHelper>(),
            Microsoft::UI::Xaml::PropertyMetadata(nullptr));
        return prop;
    }
    Microsoft::UI::Xaml::DependencyProperty NavItemIconHelper::ShowNotificationDotProperty()
    {
        static Microsoft::UI::Xaml::DependencyProperty prop = Microsoft::UI::Xaml::DependencyProperty::RegisterAttached(
            L"ShowNotificationDot",
            winrt::xaml_typename<bool>(),
            winrt::xaml_typename<winrt::MicrosoftDocsGallery::Helpers::NavItemIconHelper>(),
            Microsoft::UI::Xaml::PropertyMetadata(winrt::box_value(false)));
        return prop;
    }
    Microsoft::UI::Xaml::DependencyProperty NavItemIconHelper::UnselectedIconProperty()
    {
        static Microsoft::UI::Xaml::DependencyProperty prop = Microsoft::UI::Xaml::DependencyProperty::RegisterAttached(
            L"UnselectedIcon",
            winrt::xaml_typename<winrt::Windows::Foundation::IInspectable>(),
            winrt::xaml_typename<winrt::MicrosoftDocsGallery::Helpers::NavItemIconHelper>(),
            Microsoft::UI::Xaml::PropertyMetadata(nullptr));
        return prop;
    }
    Microsoft::UI::Xaml::DependencyProperty NavItemIconHelper::StaticIconVisibilityProperty()
    {
        static Microsoft::UI::Xaml::DependencyProperty prop = Microsoft::UI::Xaml::DependencyProperty::RegisterAttached(
            L"StaticIconVisibility",
            winrt::xaml_typename<winrt::Microsoft::UI::Xaml::Visibility>(),
            winrt::xaml_typename<winrt::MicrosoftDocsGallery::Helpers::NavItemIconHelper>(),
            Microsoft::UI::Xaml::PropertyMetadata(winrt::box_value(Microsoft::UI::Xaml::Visibility::Collapsed)));
        return prop;
    }

    IInspectable NavItemIconHelper::GetSelectedIcon(Microsoft::UI::Xaml::DependencyObject const& obj)
    {
        return obj.GetValue(SelectedIconProperty());
    }
    void NavItemIconHelper::SetSelectedIcon(Microsoft::UI::Xaml::DependencyObject const& obj, IInspectable const& value)
    {
        obj.SetValue(SelectedIconProperty(), value);
    }

    bool NavItemIconHelper::GetShowNotificationDot(Microsoft::UI::Xaml::DependencyObject const& obj)
    {
        return unbox_value<bool>(obj.GetValue(ShowNotificationDotProperty()));
    }
    void NavItemIconHelper::SetShowNotificationDot(Microsoft::UI::Xaml::DependencyObject const& obj, bool value)
    {
        obj.SetValue(ShowNotificationDotProperty(), box_value(value));
    }

    IInspectable NavItemIconHelper::GetUnselectedIcon(Microsoft::UI::Xaml::DependencyObject const& obj)
    {
        return obj.GetValue(UnselectedIconProperty());
    }
    void NavItemIconHelper::SetUnselectedIcon(Microsoft::UI::Xaml::DependencyObject const& obj, IInspectable const& value)
    {
        obj.SetValue(UnselectedIconProperty(), value);
    }

    Microsoft::UI::Xaml::Visibility NavItemIconHelper::GetStaticIconVisibility(Microsoft::UI::Xaml::DependencyObject const& obj)
    {
        return unbox_value<Microsoft::UI::Xaml::Visibility>(obj.GetValue(StaticIconVisibilityProperty()));
    }
    void NavItemIconHelper::SetStaticIconVisibility(Microsoft::UI::Xaml::DependencyObject const& obj, Microsoft::UI::Xaml::Visibility const& value)
    {
        obj.SetValue(StaticIconVisibilityProperty(), box_value(value));
    }
}
