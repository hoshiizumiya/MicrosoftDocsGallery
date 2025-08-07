#pragma once
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Windows.Foundation.h>

namespace winrt::MicrosoftDocsGallery::Converters
{
    // 布尔值取反转换器
    struct BoolNegationConverter : winrt::implements<BoolNegationConverter, winrt::Microsoft::UI::Xaml::Data::IValueConverter>
    {
        winrt::Windows::Foundation::IInspectable Convert(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            winrt::hstring const& language)
        {
            if (auto boolValue = value.try_as<bool>())
            {
                return winrt::box_value(!boolValue.value());
            }
            return winrt::box_value(true);
        }

        winrt::Windows::Foundation::IInspectable ConvertBack(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            winrt::hstring const& language)
        {
            if (auto boolValue = value.try_as<bool>())
            {
                return winrt::box_value(!boolValue.value());
            }
            return winrt::box_value(false);
        }
    };

    // 可见性转换器
    struct BoolToVisibilityConverter : winrt::implements<BoolToVisibilityConverter, winrt::Microsoft::UI::Xaml::Data::IValueConverter>
    {
        winrt::Windows::Foundation::IInspectable Convert(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            winrt::hstring const& language)
        {
            bool isVisible = false;
            if (auto boolValue = value.try_as<bool>())
            {
                isVisible = boolValue.value();
            }

            // 检查是否是反向转换
            bool invert = false;
            if (parameter)
            {
                if (auto paramStr = parameter.try_as<winrt::hstring>())
                {
                    invert = paramStr.value() == L"Invert";
                }
            }

            if (invert) isVisible = !isVisible;

            return winrt::box_value(isVisible ? 
                winrt::Microsoft::UI::Xaml::Visibility::Visible : 
                winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        }

        winrt::Windows::Foundation::IInspectable ConvertBack(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            winrt::hstring const& language)
        {
            if (auto visibility = value.try_as<winrt::Microsoft::UI::Xaml::Visibility>())
            {
                bool isVisible = visibility.value() == winrt::Microsoft::UI::Xaml::Visibility::Visible;
                
                bool invert = false;
                if (parameter)
                {
                    if (auto paramStr = parameter.try_as<winrt::hstring>())
                    {
                        invert = paramStr.value() == L"Invert";
                    }
                }

                if (invert) isVisible = !isVisible;
                return winrt::box_value(isVisible);
            }
            return winrt::box_value(false);
        }
    };

    // 日期时间格式化转换器
    struct DateTimeToStringConverter : winrt::implements<DateTimeToStringConverter, winrt::Microsoft::UI::Xaml::Data::IValueConverter>
    {
        winrt::Windows::Foundation::IInspectable Convert(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            winrt::hstring const& language)
        {
            if (auto dateTime = value.try_as<winrt::Windows::Foundation::DateTime>())
            {
                auto calendar = winrt::Windows::Globalization::Calendar();
                calendar.SetDateTime(dateTime.value());
                
                // 根据参数决定格式
                winrt::hstring format = L"ShortDate";
                if (parameter)
                {
                    if (auto paramStr = parameter.try_as<winrt::hstring>())
                    {
                        format = paramStr.value();
                    }
                }

                if (format == L"ShortDate")
                {
                    return winrt::box_value(winrt::hstring{
                        std::to_wstring(calendar.Year()) + L"/" +
                        std::to_wstring(calendar.Month()) + L"/" +
                        std::to_wstring(calendar.Day())
                    });
                }
                else if (format == L"ShortTime")
                {
                    return winrt::box_value(winrt::hstring{
                        std::to_wstring(calendar.Hour()) + L":" +
                        std::to_wstring(calendar.Minute())
                    });
                }
                else if (format == L"Relative")
                {
                    // 相对时间显示（如：2小时前）
                    auto now = winrt::Windows::Foundation::DateTime::clock::now();
                    auto diff = now.time_since_epoch() - dateTime.value().time_since_epoch();
                    auto hours = std::chrono::duration_cast<std::chrono::hours>(diff).count();
                    
                    if (hours < 1)
                    {
                        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();
                        return winrt::box_value(std::to_wstring(minutes) + L" 分钟前");
                    }
                    else if (hours < 24)
                    {
                        return winrt::box_value(std::to_wstring(hours) + L" 小时前");
                    }
                    else
                    {
                        auto days = hours / 24;
                        return winrt::box_value(std::to_wstring(days) + L" 天前");
                    }
                }
            }
            return winrt::box_value(L"");
        }

        winrt::Windows::Foundation::IInspectable ConvertBack(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            winrt::hstring const& language)
        {
            // 通常不需要从字符串转换回日期时间
            throw winrt::hresult_not_implemented();
        }
    };
}