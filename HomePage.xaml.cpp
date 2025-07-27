#include "pch.h"
#include "HomePage.xaml.h"
#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif

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
		// 加载XAML，必须放在最前面
		InitializeComponent();

		// 初始化默认内容
		InitializeDefaultContent();

		// 在 Loaded 事件中异步加载数据
		this->Loaded({ this, &HomePage::OnLoaded });

		// 设置自动滑动效果 - 使用WinRT异步模式
		StartAutoScrollAsync();
	}

	void HomePage::InitializeDefaultContent()
	{
		// 初始化特色内容轮播
		DefaultCarousel();

		// 初始化最近浏览项目
		if (!m_recentItems)
		{
			m_recentItems = single_threaded_observable_vector<IInspectable>();
		}
		m_recentItems.Clear();

		// 添加一些默认的最近浏览项目
		auto recentItemsArray = winrt::array_view<winrt::hstring const>{
			L"WinUI 3 入门指南",
			L"C# 10 新特性",
			L"Azure Functions 开发",
			L"Entity Framework Core",
			L"ASP.NET Core MVC"
		};

		for (auto const& item : recentItemsArray)
		{
			m_recentItems.Append(box_value(item));
		}

		auto recentList = this->FindName(L"RecentItemsList").try_as<ListView>();
		if (recentList && !recentList.ItemsSource())
		{
			recentList.ItemsSource(m_recentItems);
		}
	}

	Windows::Foundation::IAsyncAction HomePage::StartAutoScrollAsync()
	{
		auto scrollView = this->FindName(L"scrollView").try_as<ScrollViewer>();
		if (!scrollView)
			co_return;

		// 使用weak_ref避免循环引用
		auto weakThis = get_weak();
		auto weakScroll = make_weak(scrollView);

		while (true)
		{
			// 使用WinRT的异步延迟
			co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{ 30000000 }); // 3秒 (100ns 单位)

			// 检查对象是否仍然有效
			if (auto strongThis = weakThis.get())
			{
				if (auto sv = weakScroll.get())
				{
					// 在UI线程上执行滚动操作
					strongThis->DispatcherQueue().TryEnqueue([sv]()
					{
						auto max = sv.ScrollableWidth();
						if (max > 0)
						{
							static double offset = 0;
							offset += 300; // 每次滚动300像素
							if (offset > max) offset = 0;
							sv.ChangeView(offset, nullptr, nullptr, true); // 平滑滚动
						}
					});
				}
				else
				{
					// ScrollViewer已被销毁，退出循环
					break;
				}
			}
			else
			{
				// HomePage已被销毁，退出循环
				break;
			}
		}
	}

	void HomePage::Page_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		UpdateNavigationSelection();
	}

	void HomePage::Animated_GotItem(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto button = sender.try_as<Button>();
		if (button)
		{
			 // 在WinUI 3中，需要通过XamlRoot获取主窗口
			auto xamlRoot = button.XamlRoot();
			if (xamlRoot)
			{
				// 通过Content属性获取MainWindow
				auto content = xamlRoot.Content();
				if (content)
				{
					auto mainWindow = content.try_as<MicrosoftDocsGallery::MainWindow>();
					if (mainWindow)
					{
						mainWindow.openWebViewPage(L"https://learn.microsoft.com/");
					}
				}
			}
		}
	}

	void HomePage::OpenWinUIDocs_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto button = sender.try_as<Button>();
		if (button)
		{
			auto xamlRoot = button.XamlRoot();
			if (xamlRoot)
			{
				auto content = xamlRoot.Content();
				if (content)
				{
					auto mainWindow = content.try_as<MicrosoftDocsGallery::MainWindow>();
					if (mainWindow)
					{
						mainWindow.openWebViewPage(L"https://learn.microsoft.com/windows/apps/winui/");
					}
				}
			}
		}
	}

	void HomePage::QuickAccess_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto button = sender.try_as<Button>();
		if (button)
		{
			auto url = button.Tag().try_as<hstring>();
			if (url)
			{
				auto xamlRoot = button.XamlRoot();
				if (xamlRoot)
				{
					auto content = xamlRoot.Content();
					if (content)
					{
						auto mainWindow = content.try_as<MicrosoftDocsGallery::MainWindow>();
						if (mainWindow)
						{
							mainWindow.openWebViewPage(url.value());
						}
					}
				}
			}
		}
	}

	void HomePage::DefaultCarousel()
	{
		// 初始化特色内容集合
		if (!m_items)
		{
			m_items = single_threaded_observable_vector<IInspectable>();
		}
		m_items.Clear();

		auto featuredTopicsArray = winrt::array_view<winrt::hstring const>{
			L"WinUI 3 最新功能",
			L".NET 8 性能优化",
			L"Azure AI 服务",
			L"Microsoft Graph API",
			L"PowerPlatform 自动化",
			L"DevOps 最佳实践"
		};

		for (auto const& topic : featuredTopicsArray)
		{
			m_items.Append(box_value(topic));
		}

		auto carousel = this->FindName(L"CardCarousel").try_as<ItemsControl>();
		if (carousel && !carousel.ItemsSource())
		{
			carousel.ItemsSource(m_items);
		}
	}

	winrt::Windows::Foundation::IAsyncAction HomePage::LoadTopicsAsync()
	{
		try
		{
			HttpClient client;
			Uri uri{ L"https://learn.microsoft.com/api/catalog?locale=zh-cn&$top=10" };
			auto resp = co_await client.GetAsync(uri);

			if (resp.IsSuccessStatusCode())
			{
				auto jsonStr = co_await resp.Content().ReadAsStringAsync();
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

							auto maxItems = (std::min)(arr.Size(), 6u); // 限制为6个项目，使用std::min
							for (uint32_t i = 0; i < maxItems; ++i)
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
		catch (...)
		{
			// 如果网络请求失败，使用默认内容
			OutputDebugStringA("Failed to load topics from API, using default content.\n");
		}
		co_return;
	}

	winrt::Windows::Foundation::IAsyncAction HomePage::LoadRecentItemsAsync()
	{
		try
		{
			// TODO: 从本地存储加载最近浏览的项目
			// 这里可以从ApplicationData加载用户的浏览历史

			// 使用WinRT异步延迟而不是std::chrono
			co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{ 1000000 }); // 100ms

			// 暂时使用模拟数据
			m_recentItems.Clear();
			auto simulatedRecentArray = winrt::array_view<winrt::hstring const>{
				L"WinUI 3 入门教程",
				L"Azure Functions 快速开始",
				L"Entity Framework 数据访问",
				L"ASP.NET Core Web API"
			};

			for (auto const& item : simulatedRecentArray)
			{
				m_recentItems.Append(box_value(item));
			}
		}
		catch (...)
		{
			// 忽略加载错误
		}
		co_return;
	}

	winrt::Windows::Foundation::IAsyncAction HomePage::LoadUserStatisticsAsync()
	{
		try
		{
			// TODO: 从本地存储或服务加载用户统计信息
			// 这里可以加载用户的使用统计数据

			// 使用WinRT异步延迟
			co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{ 2000000 }); // 200ms

			// 统计数据会在XAML中硬编码显示，这里可以动态更新
		}
		catch (...)
		{
			// 忽略加载错误
		}
		co_return;
	}

	void HomePage::OnLoaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		// 异步加载所有数据
		LoadTopicsAsync();
		LoadRecentItemsAsync();
		LoadUserStatisticsAsync();
		LoadUserPreferences();
	}

	void HomePage::UpdateNavigationSelection()
	{
		// 在WinUI 3中，通过页面的Parent层次查找MainWindow
		auto parent = this->Parent();
		while (parent)
		{
			// 尝试向上查找MainWindow
			auto mainWindow = parent.try_as<MicrosoftDocsGallery::MainWindow>();
			if (mainWindow)
			{
				// 找到MainWindow，使用公共方法更新导航选择
				mainWindow.SetNavigationSelection(L"home");
				break;
			}
			
			// 继续向上查找
			auto parentElement = parent.try_as<FrameworkElement>();
			if (parentElement)
			{
				parent = parentElement.Parent();
			}
			else
			{
				break;
			}
		}
	}

	void HomePage::LoadUserPreferences()
	{
		try
		{
			// TODO: 从本地存储加载用户偏好设置
			// 这里可以加载用户的主题偏好、语言设置等
		}
		catch (...)
		{
			// 忽略加载错误
		}
	}
}
