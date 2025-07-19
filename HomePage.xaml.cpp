#include "pch.h"
#include "HomePage.xaml.h"
#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Data.Json.h>
#include <vector>
#include <thread>
#include <chrono>
#include <future>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Web::Http;
using namespace Windows::Data::Json;

namespace winrt::MicrosoftDocsGallery::implementation
{
	HomePage::HomePage()
	{
		// 加载XAML，必须放在最前面.加载并解析与该页面关联的 XAML 文件（如 HomePage.xaml），并将 XAML 中定义的控件与 C++ 后台代码进行关联。没有这句，XAML 里的控件不会被实例化，FindName 也会失败。
		InitializeComponent();
		
		DefaultCarousel();

		// 在 Loaded 事件中异步加载微软文档主题
		this->Loaded({ this, &HomePage::OnLoaded });

		// 自动滑动
		auto scrollView = this->FindName(L"scrollView").try_as<ScrollViewer>();
		if (scrollView)
		{
			auto weakScroll = winrt::make_weak(scrollView);
			auto dispatcherQueue = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
			std::thread([weakScroll, dispatcherQueue]() {
				double offset = 0;
				while (true)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1800));
					auto sv = weakScroll.get();
					if (!sv) break;
					dispatcherQueue.TryEnqueue([sv, &offset]() mutable {
						double max = sv.ScrollableWidth();
						offset += 180;
						if (offset > max) offset = 0;
						sv.ChangeView(offset, nullptr, nullptr);
											   });
				}
						}).detach();
		}
	}



	void HomePage::Page_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		// 通过主窗口查找NavigationView并高亮Home
		auto window = Window::Current().Content().try_as<FrameworkElement>();
		if (window)
		{
			auto navObj = window.FindName(L"nav");
			auto nav = navObj.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationView>();
			if (nav)
			{
				for (uint32_t i = 0; i < nav.MenuItems().Size(); ++i)
				{
					auto navItem = nav.MenuItems().GetAt(i).try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
					if (navItem && navItem.Tag().try_as<hstring>() == L"home")
					{
						nav.SelectedItem(navItem);
						break;
					}
				}
			}
		}
	}

	void HomePage::Animated_GotItem(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
	}

	void HomePage::OpenWinUIDocs_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto window = Window::Current().Content().try_as<winrt::MicrosoftDocsGallery::MainWindow>();
		if (window)
		{
			window.openWebViewPage(L"https://learn.microsoft.com/windows/apps/winui/");
		}
	}

	//
	void HomePage::DefaultCarousel()
	{
		// 只初始化一次 m_items，不要重复赋值
		if (!m_items)
		{
			m_items = single_threaded_observable_vector<IInspectable>();
		}
		m_items.Clear();
		std::vector<hstring> topics = { L"WinUI 3 Sample" };
		for (auto&& t : topics)
		{
			m_items.Append(box_value(t));
		}
		// 只绑定一次 ItemsSource
		auto carousel = this->FindName(L"CardCarousel").try_as<ItemsControl>();
		if (carousel && !carousel.ItemsSource())
		{
			carousel.ItemsSource(m_items);
		}
	}

	// 新增异步加载方法——BUG
	winrt::Windows::Foundation::IAsyncAction HomePage::LoadTopicsAsync()
	{
		try
		{
			HttpClient client;
			Uri uri{ L"https://learn.microsoft.com/api/catalog?locale=zh-cn&$top=8" };
			auto resp = co_await client.GetAsync(uri);
			if (resp.IsSuccessStatusCode())
			{
				auto jsonStr = co_await resp.Content().ReadAsStringAsync();
				OutputDebugStringW(jsonStr.c_str()); // 输出原始JSON，便于调试
				JsonObject root;
				if (JsonObject::TryParse(jsonStr, root))
				{
					if (root.HasKey(L"resources"))
					{
						auto resources = root.GetNamedValue(L"resources");
						if (resources.ValueType() == JsonValueType::Array)
						{
							auto arr = resources.GetArray();
							m_items.Clear();
							for (uint32_t i = 0; i < arr.Size(); ++i)
							{
								if (arr.GetAt(i).ValueType() == JsonValueType::Object)
								{
									auto obj = arr.GetAt(i).GetObject();
									if (obj.HasKey(L"title"))
									{
										hstring title = obj.GetNamedString(L"title");
										m_items.Append(box_value(title));
									}
								}
							}
						}
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			OutputDebugStringA(e.what());
		}
		catch (...)
		{
			OutputDebugStringA("Unknown exception in LoadTopicsAsync.\n");
		}
		co_return;
	}

	void HomePage::OnLoaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		LoadTopicsAsync();
	}
}
