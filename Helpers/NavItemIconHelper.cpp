#include "pch.h"
#include "Helpers/NavItemIconHelper.h"

#if __has_include("Helpers/NavItemIconHelper.g.cpp")
#include "Helpers/NavItemIconHelper.g.cpp"
#endif


#include <winrt/Windows.UI.Xaml.Interop.h>

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace Windows::Foundation;

namespace winrt::MicrosoftDocsGallery::Helpers::implementation
{

    winrt::Microsoft::UI::Xaml::DependencyProperty NavItemIconHelper::s_selectedIconProperty = 
        DependencyProperty::RegisterAttached(
            L"SelectedIcon",
            winrt::xaml_typename<IInspectable>(),
            winrt::xaml_typename<class_type>(),  // 应该使用投影类型而非实现类型，Cpp的搜索模式会导致使用实现类型
            PropertyMetadata {
                nullptr, 
                &NavItemIconHelper::OnSelectedIconChanged
            });

    winrt::Microsoft::UI::Xaml::DependencyProperty NavItemIconHelper::s_showNotificationDotProperty =
        DependencyProperty::RegisterAttached(
            L"ShowNotificationDot",
            winrt::xaml_typename<bool>(),
            winrt::xaml_typename<class_type>(),  // 同上 L22
            PropertyMetadata { 
                false, 
                &NavItemIconHelper::OnShowNotificationDotChanged
            });

    winrt::Microsoft::UI::Xaml::DependencyProperty NavItemIconHelper::s_unselectedIconProperty =
        DependencyProperty::RegisterAttached(
            L"UnselectedIcon",
            winrt::xaml_typename<IInspectable>(),
            winrt::xaml_typename<class_type>(),   // 同上 L22
            PropertyMetadata { 
                nullptr,
                &NavItemIconHelper::OnUnselectedIconChanged
            });

    winrt::Microsoft::UI::Xaml::DependencyProperty NavItemIconHelper::s_staticIconVisibilityProperty =
        DependencyProperty::RegisterAttached(
            L"StaticIconVisibility",
            winrt::xaml_typename<Visibility>(),
            winrt::xaml_typename<class_type>(),    // 同上 L22
            PropertyMetadata {
                winrt::box_value(Visibility::Collapsed),
                &NavItemIconHelper::OnStaticIconVisibilityChanged
            });

    void NavItemIconHelper::OnSelectedIconChanged(
        winrt::Microsoft::UI::Xaml::DependencyObject const& d,
        winrt::Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e)
    {
        auto const icon = unbox_value<IInspectable>(e.NewValue());
        auto const control = d.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
        if (control)
        {
            /*control.Icon(icon);*/
        }
    }

    void NavItemIconHelper::OnShowNotificationDotChanged(
        winrt::Microsoft::UI::Xaml::DependencyObject const& d,
        winrt::Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e)
    {
        auto const showDot = unbox_value<bool>(e.NewValue());
        auto const control = d.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
        if (control)
        {
            /*control.UpdateVisualState(true);*/
        }
    }

    void NavItemIconHelper::OnUnselectedIconChanged(
        winrt::Microsoft::UI::Xaml::DependencyObject const& d,
        winrt::Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e)
    {
        auto const icon = unbox_value<IInspectable>(e.NewValue());
        auto const control = d.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
        if (control)
        {
            /*control.Icon(icon);*/
        }
    }

    void NavItemIconHelper::OnStaticIconVisibilityChanged(
        winrt::Microsoft::UI::Xaml::DependencyObject const& d,
        winrt::Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e)
    {
        auto const visibility = unbox_value<Visibility>(e.NewValue());
        auto const control = d.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
        if (control)
        {
            /*control.UpdateVisualState(true);*/
        }
    }

    DependencyProperty NavItemIconHelper::SelectedIconProperty()
    {
        return s_selectedIconProperty;
    }

    DependencyProperty NavItemIconHelper::ShowNotificationDotProperty()
    {
        return s_showNotificationDotProperty;
    }

    DependencyProperty NavItemIconHelper::UnselectedIconProperty()
    {
        return s_unselectedIconProperty;
    }

    DependencyProperty NavItemIconHelper::StaticIconVisibilityProperty()
    {
        return s_staticIconVisibilityProperty;
    }

    IInspectable NavItemIconHelper::GetSelectedIcon(DependencyObject const& obj)
    {
        return obj.GetValue(SelectedIconProperty());
    }

    void NavItemIconHelper::SetSelectedIcon(DependencyObject const& obj, IInspectable const& value)
    {
        obj.SetValue(SelectedIconProperty(), value);
    }

    bool NavItemIconHelper::GetShowNotificationDot(DependencyObject const& obj)
    {
        return unbox_value<bool>(obj.GetValue(ShowNotificationDotProperty()));
    }

    void NavItemIconHelper::SetShowNotificationDot(DependencyObject const& obj, bool value)
    {
        obj.SetValue(ShowNotificationDotProperty(), box_value(value));
    }

    IInspectable NavItemIconHelper::GetUnselectedIcon(DependencyObject const& obj)
    {
        return obj.GetValue(UnselectedIconProperty());
    }
    void NavItemIconHelper::SetUnselectedIcon(DependencyObject const& obj, IInspectable const& value)
    {
        obj.SetValue(UnselectedIconProperty(), value);
    }

    Visibility NavItemIconHelper::GetStaticIconVisibility(DependencyObject const& obj)
    {
        return unbox_value<Visibility>(obj.GetValue(StaticIconVisibilityProperty()));
    }

    void NavItemIconHelper::SetStaticIconVisibility(DependencyObject const& obj, Visibility const& value)
    {
        obj.SetValue(StaticIconVisibilityProperty(), box_value(value));
    }


}
