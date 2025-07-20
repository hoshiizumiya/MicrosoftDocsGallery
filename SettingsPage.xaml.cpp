#include "pch.h"
#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif
#include <winrt/Windows.Globalization.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include "App.xaml.h"
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Globalization;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::MicrosoftDocsGallery::implementation
{
	SettingsPage::SettingsPage()
	{
		InitializeComponent();

		// 设置ComboBox默认选中项
		auto lang = ApplicationLanguages::PrimaryLanguageOverride();
		if (lang.empty()) lang = L"en-US";
		for (int i = 0; i < LanguageComboBox().Items().Size(); ++i)
		{
			auto item = LanguageComboBox().Items().GetAt(i).try_as<ComboBoxItem>();
			if (item && item.Tag().try_as<hstring>() == lang)
			{
				LanguageComboBox().SelectedIndex(i);
				break;
			}
		}
	}

	void SettingsPage::LanguageComboBox_SelectionChanged(IInspectable const& sender, SelectionChangedEventArgs const& e)
	{
		auto combo = sender.try_as<ComboBox>();
		if (!combo) return;
		auto sel = combo.SelectedItem().try_as<ComboBoxItem>();
		if (!sel) return;
		// 正确处理 std::optional<winrt::hstring>
		hstring lang = sel.Tag().try_as<hstring>().value_or(L"");
		if (lang.empty()) return;
		if (ApplicationLanguages::PrimaryLanguageOverride() != lang)
		{
			ApplicationLanguages::PrimaryLanguageOverride(lang);
			// 先将 Content() 转为 IInspectable，再 try_as<winrt::Microsoft::UI::Xaml::Controls::Frame>()
			auto content = Window::Current().Content();
			auto frame = content ? content.as<IInspectable>().try_as<winrt::Microsoft::UI::Xaml::Controls::Frame>() : nullptr;
			if (frame)
			{
				frame.Navigate(xaml_typename<winrt::MicrosoftDocsGallery::SettingsPage>());
				frame.BackStack().Clear();
			}
		}
	}
}
