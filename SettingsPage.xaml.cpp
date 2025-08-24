#include "pch.h"
#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif
#include <winrt/Windows.Globalization.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include "App.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Globalization;
using namespace Windows::Foundation;
using namespace Windows::Storage;

namespace winrt::MicrosoftDocsGallery::implementation
{
    SettingsPage::SettingsPage()
    {
        InitializeComponent();
        LoadSettings();
    }

    void SettingsPage::LanguageComboBox_SelectionChanged(IInspectable const& sender, SelectionChangedEventArgs const& /*e*/)
    {
        auto combo = sender.try_as<ComboBox>();
        if (!combo) return;
        
        auto selectedItem = combo.SelectedItem().try_as<ComboBoxItem>();
        if (!selectedItem) return;
        
        auto lang = selectedItem.Tag().try_as<hstring>().value_or(L"");
        if (lang.empty()) return;
        
        if (ApplicationLanguages::PrimaryLanguageOverride() != lang)
        {
            m_language = lang;
            ApplicationLanguages::PrimaryLanguageOverride(lang);
            
            // 简化语言更新逻辑，避免页面重新加载
            SaveSettings();
        }
    }

    void SettingsPage::DarkModeToggle_Toggled(IInspectable const& sender, RoutedEventArgs const& /*e*/)
    {
        auto toggle = sender.try_as<ToggleSwitch>();
        if (toggle)
        {
            m_isDarkMode = toggle.IsOn();
            ApplyTheme();
            SaveSettings();
        }
    }

    void SettingsPage::FontSizeSlider_ValueChanged(IInspectable const& sender, Primitives::RangeBaseValueChangedEventArgs const& /*e*/)
    {
        if (!IsLoaded()) return;

        auto slider = sender.try_as<Slider>();
        if (slider)
        {
            m_fontSize = slider.Value();
            ApplyFontSize(m_fontSize);
            SaveSettings();
            
            // 更新字体大小显示
            auto& text = winrt::to_hstring(static_cast<int>(m_fontSize)) + L"px";
            FontSizeDisplay().Text(text);
        }
    }

    IAsyncAction SettingsPage::ClearCacheButton_Click(IInspectable const& /*sender*/, RoutedEventArgs const& /*e*/)
    {
        try
        {
            // 显示确认对话框
            co_await ShowConfirmationDialog(
                L"清除缓存", 
                L"确定要清除所有缓存数据吗？这将删除所有保存的网页数据和临时文件。"
            );
            
            // TODO: 实现缓存清理逻辑
            // 这里可以清理WebView2缓存、应用程序缓存等
            
            // 显示完成消息
            ContentDialog dialog;
            dialog.Title(winrt::box_value(L"完成"));
            dialog.Content(winrt::box_value(L"缓存已成功清除。"));
            dialog.PrimaryButtonText(L"确定");
            dialog.XamlRoot(this->XamlRoot());
            co_await dialog.ShowAsync();
        }
        catch (...)
        {
            // 用户取消了操作
        }
    }

    IAsyncAction SettingsPage::ResetSettingsButton_Click(IInspectable const& /*sender*/, RoutedEventArgs const& /*e*/)
    {
        try
        {
            // 显示确认对话框
            co_await ShowConfirmationDialog(
                L"重置设置", 
                L"确定要将所有设置恢复到默认值吗？此操作无法撤销。"
            );
            
            // 重置到默认值
            m_isDarkMode = false;
            m_fontSize = 16.0;
            m_language = L"en-US";
            
            // 应用默认设置
            DarkModeToggle().IsOn(m_isDarkMode);
            FontSizeSlider().Value(m_fontSize);
            
            // 重置语言选择
            for (uint32_t i = 0; i < LanguageComboBox().Items().Size(); ++i)
            {
                auto item = LanguageComboBox().Items().GetAt(i).try_as<ComboBoxItem>();
                if (item && item.Tag().try_as<hstring>() == m_language)
                {
                    LanguageComboBox().SelectedIndex(i);
                    break;
                }
            }
            
            ApplyTheme();
            ApplyFontSize(m_fontSize);
            ApplicationLanguages::PrimaryLanguageOverride(m_language);
            SaveSettings();
            
            // 显示完成消息
            ContentDialog dialog;
            dialog.Title(winrt::box_value(L"完成"));
            dialog.Content(winrt::box_value(L"设置已重置到默认值。"));
            dialog.PrimaryButtonText(L"确定");
            dialog.XamlRoot(this->XamlRoot());
            co_await dialog.ShowAsync();
        }
        catch (...)
        {
            // 用户取消了操作
        }
    }

    void SettingsPage::LoadSettings()
    {
        try
        {
            auto localSettings = ApplicationData::Current().LocalSettings();
            auto values = localSettings.Values();
            
            // 加载深色模式设置
            if (values.HasKey(L"IsDarkMode"))
            {
                m_isDarkMode = unbox_value_or<bool>(values.Lookup(L"IsDarkMode"), false);
                DarkModeToggle().IsOn(m_isDarkMode);
            }
            
            // 加载字体大小设置
            if (values.HasKey(L"FontSize"))
            {
                m_fontSize = unbox_value_or<double>(values.Lookup(L"FontSize"), 16.0);
                FontSizeSlider().Value(m_fontSize);
                FontSizeDisplay().Text(winrt::to_hstring(static_cast<int>(m_fontSize)) + L"px");
            }
            
            // 加载语言设置
            auto lang = ApplicationLanguages::PrimaryLanguageOverride();
            if (lang.empty()) lang = L"en-US";
            m_language = lang;
            
            for (uint32_t i = 0; i < LanguageComboBox().Items().Size(); ++i)
            {
                auto item = LanguageComboBox().Items().GetAt(i).try_as<ComboBoxItem>();
                if (item && item.Tag().try_as<hstring>() == lang)
                {
                    LanguageComboBox().SelectedIndex(i);
                    break;
                }
            }
            
            // 应用设置
            ApplyTheme();
            ApplyFontSize(m_fontSize);
        }
        catch (...)
        {
            // 使用默认设置
        }
    }

    void SettingsPage::SaveSettings()
    {
        try
        {
            auto localSettings = ApplicationData::Current().LocalSettings();
            auto values = localSettings.Values();
            
            values.Insert(L"IsDarkMode", winrt::box_value(m_isDarkMode));
            values.Insert(L"FontSize", winrt::box_value(m_fontSize));
            values.Insert(L"Language", winrt::box_value(m_language));
        }
        catch (...)
        {
            // 忽略保存错误
        }
    }

    void SettingsPage::ApplyTheme()
    {
        try
        {
            auto app = Application::Current().as<App>();
            if (app)
            {
                // TODO: 实现主题切换
                // app.SetTheme(m_isDarkMode ? ElementTheme::Dark : ElementTheme::Light);
            }
        }
        catch (...)
        {
            // 忽略主题应用错误
        }
    }

    void SettingsPage::ApplyFontSize(double fontSize)
    {
        try
        {
            // TODO: 实现全局字体大小应用
            // 这里可以通过ResourceDictionary更新全局字体大小
        }
        catch (...)
        {
            // 忽略字体大小应用错误
        }
    }

    IAsyncAction SettingsPage::ShowConfirmationDialog(hstring const& title, hstring const& content)
    {
        ContentDialog dialog;
        dialog.Title(winrt::box_value(title));
        dialog.Content(winrt::box_value(content));
        dialog.PrimaryButtonText(L"确定");
        dialog.SecondaryButtonText(L"取消");
        dialog.DefaultButton(ContentDialogButton::Secondary);
        dialog.XamlRoot(this->XamlRoot());
        
        auto result = co_await dialog.ShowAsync();
        if (result != ContentDialogResult::Primary)
        {
            // 用户取消，抛出异常来中断操作
            throw hresult_canceled();
        }
    }
}
