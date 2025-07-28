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
	/// <summary>
	/// HomePage 构造函数
	/// 初始化首页，设置默认内容，启动异步加载和自动滚动
	/// </summary>
	HomePage::HomePage()
	{
		// 加载XAML，必须放在最前面
		// 这将加载对应的XAML文件并初始化UI元素
		InitializeComponent();

		// 初始化默认内容
		// 设置页面的基础数据，包括轮播图和最近浏览项目
		InitializeDefaultContent();

		// 在 Loaded 事件中异步加载数据
		// 当页面完全加载后，触发数据加载操作
		this->Loaded({ this, &HomePage::OnLoaded });

		// 设置自动滑动效果 - 使用WinRT异步模式
		// 启动后台任务来实现轮播图的自动滚动
		StartAutoScrollAsync();
	}

	/// <summary>
	/// 初始化页面的默认内容
	/// 包括特色内容轮播和最近浏览项目的初始化
	/// </summary>
	void HomePage::InitializeDefaultContent()
	{
		// 初始化特色内容轮播
		// 创建并填充特色主题的轮播数据
		DefaultCarousel();

		// 初始化最近浏览项目
		// 检查集合是否已存在，如果不存在则创建新的可观察集合
		if (!m_recentItems)
		{
			m_recentItems = single_threaded_observable_vector<IInspectable>();
		}
		m_recentItems.Clear();

		// 添加一些默认的最近浏览项目
		// 使用静态数据模拟用户的最近浏览历史
		auto recentItemsArray = winrt::array_view<winrt::hstring const>{
			L"WinUI 3 入门指南",
			L"C# 10 新特性",
			L"Azure Functions 开发",
			L"Entity Framework Core",
			L"ASP.NET Core MVC"
		};

		// 将每个项目添加到可观察集合中
		// 使用 box_value 将 hstring 包装为 IInspectable
		for (auto const& item : recentItemsArray)
		{
			m_recentItems.Append(box_value(item));
		}

		// 查找XAML中定义的最近浏览列表控件并绑定数据源
		auto recentList = this->FindName(L"RecentItemsList").try_as<ListView>();
		if (recentList && !recentList.ItemsSource())
		{
			recentList.ItemsSource(m_recentItems);
		}
	}

	/// <summary>
	/// 启动自动滚动异步任务
	/// 为轮播图实现自动滚动功能，每3秒滚动一次
	/// </summary>
	/// <returns>异步操作对象</returns>
	Windows::Foundation::IAsyncAction HomePage::StartAutoScrollAsync()
	{
		// 查找XAML中的ScrollViewer控件
		auto scrollView = this->FindName(L"scrollView").try_as<ScrollViewer>();
		if (!scrollView)
			co_return;

		// 使用weak_ref避免循环引用
		// 防止异步任务持有强引用导致内存泄漏
		auto weakThis = get_weak();
		auto weakScroll = make_weak(scrollView);

		// 无限循环实现定时滚动
		while (true)
		{
			// 使用WinRT的异步延迟，等待3秒
			// TimeSpan 使用100纳秒为单位，30000000 = 3秒
			co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{ 30000000 });

			// 检查对象是否仍然有效
			// 防止在对象销毁后继续执行
			if (auto strongThis = weakThis.get())
			{
				if (auto sv = weakScroll.get())
				{
					// 在UI线程上执行滚动操作
					// 确保UI更新在正确的线程上执行
					strongThis->DispatcherQueue().TryEnqueue([sv]()
					{
						// 获取可滚动的最大宽度
						auto max = sv.ScrollableWidth();
						if (max > 0)
						{
							// 静态变量记录当前滚动位置
							static double offset = 0;
							offset += 300; // 每次滚动300像素
							if (offset > max) offset = 0; // 滚动到末尾时重置
							
							// 执行平滑滚动动画
							sv.ChangeView(offset, nullptr, nullptr, true);
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

	/// <summary>
	/// 页面加载完成事件处理器
	/// 更新导航选择状态
	/// </summary>
	/// <param name="">发送者对象</param>
	/// <param name="">路由事件参数</param>
	void HomePage::Page_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		// 更新主窗口的导航选择，突出显示当前页面
		UpdateNavigationSelection();
	}

	/// <summary>
	/// 动画项目点击事件处理器
	/// 响应用户点击特色内容，导航到Microsoft Learn主页
	/// </summary>
	/// <param name="sender">触发事件的按钮</param>
	/// <param name="">路由事件参数</param>
	void HomePage::Animated_GotItem(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto button = sender.try_as<Button>();
		if (button)
		{
			 // 在WinUI 3中，需要通过XamlRoot获取主窗口
			 // 这是WinUI 3推荐的获取窗口引用的方式
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
						// 调用主窗口的方法打开WebView页面
						mainWindow.openWebViewPage(L"https://learn.microsoft.com/");
					}
				}
			}
		}
	}

	/// <summary>
	/// 打开WinUI文档按钮点击事件处理器
	/// 导航到WinUI官方文档页面
	/// </summary>
	/// <param name="sender">触发事件的按钮</param>
	/// <param name="">路由事件参数</param>
	void HomePage::OpenWinUIDocs_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto button = sender.try_as<Button>();
		if (button)
		{
			// 使用相同的方式获取主窗口引用
			auto xamlRoot = button.XamlRoot();
			if (xamlRoot)
			{
				auto content = xamlRoot.Content();
				if (content)
				{
					auto mainWindow = content.try_as<MicrosoftDocsGallery::MainWindow>();
					if (mainWindow)
					{
						// 打开WinUI应用开发文档页面
						mainWindow.openWebViewPage(L"https://learn.microsoft.com/windows/apps/winui/");
					}
				}
			}
		}
	}

	/// <summary>
	/// 快速访问按钮点击事件处理器
	/// 根据按钮的Tag属性导航到指定URL
	/// </summary>
	/// <param name="sender">触发事件的按钮</param>
	/// <param name="">路由事件参数</param>
	void HomePage::QuickAccess_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto button = sender.try_as<Button>();
		if (button)
		{
			// 从按钮的Tag属性获取URL
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
							// 使用Tag中存储的URL打开相应页面
							mainWindow.openWebViewPage(url.value());
						}
					}
				}
			}
		}
	}

	/// <summary>
	/// 初始化默认轮播内容
	/// 创建特色主题的数据集合并绑定到UI控件
	/// </summary>
	void HomePage::DefaultCarousel()
	{
		// 初始化特色内容集合
		if (!m_items)
		{
			m_items = single_threaded_observable_vector<IInspectable>();
		}
		m_items.Clear();

		// 定义特色主题数组
		// 这些是展示在首页轮播中的热门技术主题
		auto featuredTopicsArray = winrt::array_view<winrt::hstring const>{
			L"WinUI 3 最新功能",
			L".NET 8 性能优化",
			L"Azure AI 服务",
			L"Microsoft Graph API",
			L"PowerPlatform 自动化",
			L"DevOps 最佳实践"
		};

		// 将每个主题添加到集合中
		for (auto const& topic : featuredTopicsArray)
		{
			m_items.Append(box_value(topic));
		}

		// 查找并绑定轮播控件
		auto carousel = this->FindName(L"CardCarousel").try_as<ItemsControl>();
		if (carousel && !carousel.ItemsSource())
		{
			carousel.ItemsSource(m_items);
		}
	}

	/// <summary>
	/// 异步加载主题数据
	/// 从Microsoft Learn API获取最新的学习主题
	/// </summary>
	/// <returns>异步操作对象</returns>
	winrt::Windows::Foundation::IAsyncAction HomePage::LoadTopicsAsync()
	{
		try
		{
			// 创建HTTP客户端
			HttpClient client;
			// 构造API请求URI，获取中文本地化的前10个资源
			Uri uri{ L"https://learn.microsoft.com/api/catalog?locale=zh-cn&$top=10" };
			
			// 发送异步HTTP GET请求
			auto resp = co_await client.GetAsync(uri);

			// 检查响应是否成功
			if (resp.IsSuccessStatusCode())
			{
				// 读取响应内容为JSON字符串
				auto jsonStr = co_await resp.Content().ReadAsStringAsync();
				JsonObject root;

				// 尝试解析JSON数据
				if (JsonObject::TryParse(jsonStr, root))
				{
					// 检查是否包含resources字段
					if (root.HasKey(L"resources"))
					{
						auto resources = root.GetNamedValue(L"resources");
						if (resources.ValueType() == JsonValueType::Array)
						{
							auto arr = resources.GetArray();
							m_items.Clear();

							// 限制显示的项目数量为6个，使用std::min避免越界
							auto maxItems = (std::min)(arr.Size(), 6u);
							for (uint32_t i = 0; i < maxItems; ++i)
							{
								if (arr.GetAt(i).ValueType() == JsonValueType::Object)
								{
									auto obj = arr.GetAt(i).GetObject();
									// 提取标题并添加到集合
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
			// 如果网络请求失败，记录错误并继续使用默认内容
			// 这确保了应用在离线或网络问题时仍能正常工作
			OutputDebugStringA("Failed to load topics from API, using default content.\n");
		}
		co_return;
	}

	/// <summary>
	/// 异步加载最近浏览项目
	/// 从本地存储或缓存加载用户的浏览历史
	/// </summary>
	/// <returns>异步操作对象</returns>
	winrt::Windows::Foundation::IAsyncAction HomePage::LoadRecentItemsAsync()
	{
		try
		{
			// TODO: 从本地存储加载最近浏览的项目
			// 这里可以从ApplicationData加载用户的浏览历史
			// 未来可以实现真实的数据持久化

			// 使用WinRT异步延迟模拟数据加载时间
			co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{ 1000000 }); // 100ms

			// 暂时使用模拟数据
			// 在实际应用中，这里会从用户的浏览历史中加载数据
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
			// 忽略加载错误，确保应用稳定性
		}
		co_return;
	}

	/// <summary>
	/// 异步加载用户统计信息
	/// 获取用户的学习进度和使用统计数据
	/// </summary>
	/// <returns>异步操作对象</returns>
	winrt::Windows::Foundation::IAsyncAction HomePage::LoadUserStatisticsAsync()
	{
		try
		{
			// TODO: 从本地存储或服务加载用户统计信息
			// 这里可以加载用户的使用统计数据
			// 例如：学习时长、完成的课程数量、最后访问时间等

			// 使用WinRT异步延迟模拟数据处理时间
			co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{ 2000000 }); // 200ms

			// 统计数据会在XAML中硬编码显示，这里可以动态更新
			// 未来可以添加用户统计信息的实际计算和显示逻辑
		}
		catch (...)
		{
			// 忽略加载错误，确保应用稳定性
		}
		co_return;
	}

	/// <summary>
	/// 页面加载完成后的回调函数
	/// 启动所有异步数据加载任务
	/// </summary>
	/// <param name="">发送者对象</param>
	/// <param name="">路由事件参数</param>
	void HomePage::OnLoaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		// 异步加载所有数据
		// 这些操作并行执行，提高页面加载性能
		LoadTopicsAsync();          // 加载特色主题
		LoadRecentItemsAsync();     // 加载最近浏览项目
		LoadUserStatisticsAsync();  // 加载用户统计信息
		LoadUserPreferences();      // 加载用户偏好设置
	}

	/// <summary>
	/// 更新主窗口的导航选择状态
	/// 确保导航栏正确显示当前页面的选中状态
	/// </summary>
	void HomePage::UpdateNavigationSelection()
	{
		// 在WinUI 3中，通过页面的Parent层次查找MainWindow
		// 这是因为WinUI 3的窗口模型与传统的WPF不同
		auto parent = this->Parent();
		while (parent)
		{
			// 尝试向上查找MainWindow
			auto mainWindow = parent.try_as<MicrosoftDocsGallery::MainWindow>();
			if (mainWindow)
			{
				// 找到MainWindow，使用公共方法更新导航选择
				// "home"标识符对应主页的导航项
				mainWindow.SetNavigationSelection(L"home");
				break;
			}
			
			// 继续向上查找父元素
			auto parentElement = parent.try_as<FrameworkElement>();
			if (parentElement)
			{
				parent = parentElement.Parent();
			}
			else
			{
				// 没有更多父元素，退出查找
				break;
			}
		}
	}

	/// <summary>
	/// 加载用户偏好设置
	/// 从本地存储读取用户的个性化配置
	/// </summary>
	void HomePage::LoadUserPreferences()
	{
		try
		{
			// TODO: 从本地存储加载用户偏好设置
			// 这里可以加载用户的主题偏好、语言设置等
			// 例如：深色/浅色主题、默认语言、字体大小、布局偏好等
			// 可以使用 Windows.Storage.ApplicationData.Current.LocalSettings
		}
		catch (...)
		{
			// 忽略加载错误，使用默认设置
			// 确保应用在配置文件损坏时仍能正常运行
		}
	}
}
