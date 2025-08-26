#include "pch.h"
#include "WebViewPage.xaml.h"
#if __has_include("WebViewPage.g.cpp")
#include "WebViewPage.g.cpp"
#endif
#include <random>
#include <sstream>
#include <algorithm>
#include <winrt/Windows.System.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::MicrosoftDocsGallery::implementation
{
	// --------------------------------------------------------------------------------------------
	// Ctor
	// CN: 初始化页面，标记尚未完成首次初始化。InitializeComponent 会加载 XAML 树。
	// EN: Initialize the page and mark that first-time init isn't done yet. InitializeComponent
	//     loads the XAML visual tree.
	// --------------------------------------------------------------------------------------------
	WebViewPage::WebViewPage()
	{
		InitializeComponent();
		m_isInitialized = false;
		this->NavigationCacheMode(Navigation::NavigationCacheMode::Required); // 启用导航缓存，避免重复加载（全部缓存无视帧大小）
	}

	// --------------------------------------------------------------------------------------------
	// OnNavigatedTo
	// CN: 页面被导航到时触发；首次触发时：
	//     1) 若 Frame.Navigate 传入了 URL 参数，则用它创建一个标签页并显示“Loading...”标题。
	//     2) 否则创建一个默认标签页（Microsoft Learn）。
	// EN: Triggered when the page is navigated to. On first enter:
	//     1) If a URL navigation parameter exists, create a tab with a "Loading..." title.
	//     2) Otherwise, create a default tab (Microsoft Learn).
	// 关键 API/流程：
	// - e.Parameter(): 从主页导航操作取得导航参数（IInspectable），再用 unbox_value<hstring> 解箱。
	// - AddNewTab: 创建 TabData、TabViewItem 与 WebView2，并开始导航。
	// --------------------------------------------------------------------------------------------
	void WebViewPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
	{
		if (!m_isInitialized)
		{
			// 首次加载时，如果有参数则创建标签页
			if (e.Parameter())
			{
				auto url = winrt::unbox_value<winrt::hstring>(e.Parameter());
				if (!url.empty())
				{
					AddNewTab(url, L"Loading...");
				}
			}
			else
			{
				// 创建默认标签页
				AddNewTab(L"https://learn.microsoft.com/", L"Microsoft Learn");
			}
			m_isInitialized = true;
		}
	}

	// --------------------------------------------------------------------------------------------
	// Page_Loaded (fallback)
	// CN: 若因导航路径特殊导致 OnNavigatedTo 未创建标签，这里兜底创建一个默认标签。
	// EN: Fallback to create a default tab if none exists when the visual tree finishes loading.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::Page_Loaded(IInspectable const&, RoutedEventArgs const&)
	{
		// 兜底：如果出于某些原因没有任何 Tab，则创建一个默认 Tab  EN: Fallback to ensure at least one default tab exists
		if (m_tabs.empty())
		{
			AddNewTab(L"https://learn.microsoft.com/", L"Microsoft Learn");
		}
	}

	// --------------------------------------------------------------------------------------------
	// TabView_AddButtonClick
	// CN: 点击“+”新增标签按钮时触发，创建一个默认导航的标签。
	// EN: Triggered when clicking the "+" button to add a new tab with a default URL.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::TabView_AddButtonClick(TabView const& /*sender*/, IInspectable const& /*args*/)
	{
		AddNewTab(L"https://learn.microsoft.com/", L"New Tab");
	}

	// --------------------------------------------------------------------------------------------
	// TabView_TabCloseRequested
	// CN: 关闭按钮点击时触发，读取 Tab 的 Tag(Id) 来定位并移除。
	// EN: Invoked when the close button is pressed; uses Tab.Tag (Id) to locate and remove it.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::TabView_TabCloseRequested(TabView const& /*sender*/, TabViewTabCloseRequestedEventArgs const& args)
	{
		auto tabItem = args.Tab();
		auto tabId = winrt::unbox_value_or<winrt::hstring>(tabItem.Tag(), L"");

		if (!tabId.empty())
		{
			CloseTab(tabId);
		}
	}

	// --------------------------------------------------------------------------------------------
	// TabView_SelectionChanged
	// CN: 选中标签变化时触发，通过 SelectedItem 找到 TabViewItem -> 读取其 Tag(Id) -> 切换。
	// EN: Triggered when selected tab changes; finds the TabViewItem and reads Tag(Id) to switch.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::TabView_SelectionChanged(IInspectable const& sender, SelectionChangedEventArgs const& /*e*/)
	{
		auto tabView = sender.as<TabView>();
		if (auto selectedItem = tabView.SelectedItem())
		{
			auto tabItem = selectedItem.as<TabViewItem>();
			auto tabId = winrt::unbox_value_or<winrt::hstring>(tabItem.Tag(), L"");
			if (!tabId.empty())
			{
				SwitchToTab(tabId);
			}
		}
	}

	// --------------------------------------------------------------------------------------------
	// 目录 NavigationView 处理
	// CN: 点击目录项 -> 从 Tag 中读出 sectionId -> 注入 JS 定位到相应元素。
	// EN: On TOC item click -> read sectionId from Tag -> inject JS to scroll the target element.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::DocNavigation_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const& /*sender*/, Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
	{
		auto invokedItem = args.InvokedItemContainer();
		if (invokedItem)
		{
			auto sectionId = winrt::unbox_value_or<winrt::hstring>(invokedItem.Tag(), L"");
			if (!sectionId.empty())
			{
				NavigateToSection(sectionId);
			}
		}
	}

	// --------------------------------------------------------------------------------------------
	// 文本过滤目录
	// CN: 简单包含匹配，匹配的显示，不匹配的折叠。
	// EN: Simple contains matching; show visible when matched, otherwise collapse.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::SectionSearch_TextChanged(IInspectable const& sender, TextChangedEventArgs const& /*e*/)
	{
		auto textBox = sender.as<TextBox>();
		if (textBox)
		{
			auto searchText = textBox.Text();
			FilterNavigationItems(searchText);
		}
	}

	// --------------------------------------------------------------------------------------------
	// 顶部按钮：收藏/刷新/浏览器打开/分享（示例或留空）
	// EN: Top action buttons: favorite/refresh/open-in-browser/share (sample or stubbed)
	// --------------------------------------------------------------------------------------------
	void WebViewPage::FavoriteButton_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
	{
		// TODO: 可在此调用收藏服务，将当前标签页加入收藏
		// TODO(EN): Call favorites service to add the current tab
	}

	void WebViewPage::RefreshPage_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
	{
		if (auto tab = GetCurrentTabData())
		{
			if (tab->WebView)
			{
				// CN: 直接调用 WebView2.Reload() 以刷新当前页
				// EN: Directly invoke WebView2.Reload() to refresh the current page
				tab->WebView.Reload();
			}
		}
	}

	void WebViewPage::OpenInBrowser_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
	{
		if (auto tab = GetCurrentTabData())
		{
			try
			{
				// CN: 使用 Windows.System.Launcher 调用系统默认浏览器打开链接
				// EN: Use Windows.System.Launcher to open the URL in the system default browser
				Windows::Foundation::Uri uri(tab->Url);
				Windows::System::Launcher::LaunchUriAsync(uri);
			}
			catch (...)
			{
			}
		}
	}

	void WebViewPage::SharePage_Click(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
	{
		// TODO: 可集成分享功能，目前留空避免抛错
		// TODO(EN): Integrate sharing if needed; left empty to avoid throwing
	}

	// --------------------------------------------------------------------------------------------
	// AddNewTab
	// CN: 创建一个新的标签数据模型 TabData，生成自定义 Header，创建 WebView2 并作为内容放入 TabViewItem。
	//     然后追加到 TabView 的 TabItems，选中它，并调用 LoadWebPageAsync 开始导航。
	// EN: Create a new TabData, build custom header, create WebView2 and set it as TabViewItem.Content.
	//     Append to TabView.TabItems, select it, and call LoadWebPageAsync to navigate.
	// 关键 API：
	// - TabViewItem.Content(UIElement): 指定标签内容区域
	// - WebView2: WinUI3 的 Web 组件，需安装 WebView2 Runtime
	// - EnsureCoreWebView2Async + Source/Reload/Navigate: 初始化与导航
	// --------------------------------------------------------------------------------------------
	void WebViewPage::AddNewTab(hstring const& url, hstring const& title)
	{
		// 初始化 initialize TabData
		auto tabData = std::make_shared<TabData>();
		tabData->Id = GenerateTabId();
		tabData->Title = title;
		tabData->Url = url;
		tabData->IsLoading = true;
		tabData->WebView = CreateWebView();

		// 创建TabViewItem
		auto tabItem = CreateTabItem(tabData);

		// 添加到集合
		m_tabs.push_back(tabData);
		WebTabView().TabItems().Append(tabItem);

		// 选中新标签页
		WebTabView().SelectedItem(tabItem);
		m_currentTabId = tabData->Id;

		// 加载网页
		LoadWebPageAsync(tabData);
	}

	// --------------------------------------------------------------------------------------------
	// CloseTab
	// CN: 通过 Id 找到要关闭的 Tab，先从 TabView.TabItems 移除，再从内部集合移除。
	// EN: Find the tab by Id, remove it from TabView.TabItems first, then from our collection.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::CloseTab(hstring const& tabId)
	{
		auto it = std::find_if(m_tabs.begin(), m_tabs.end(),
							   [&tabId](const std::shared_ptr<TabData>& tab) { return tab->Id == tabId; });

		if (it != m_tabs.end())
		{
			// 从TabView中移除对应的TabViewItem
			auto tabItems = WebTabView().TabItems();
			for (uint32_t i = 0; i < tabItems.Size(); ++i)
			{
				auto tabItem = tabItems.GetAt(i).as<TabViewItem>();
				auto itemTabId = winrt::unbox_value_or<hstring>(tabItem.Tag(), L"");
				if (itemTabId == tabId)
				{
					tabItems.RemoveAt(i);
					break;
				}
			}

			m_tabs.erase(it);

			// 如果关闭的是当前标签页且还有其他标签页，切换到第一个
			if (m_currentTabId == tabId && !m_tabs.empty())
			{
				SwitchToTab(m_tabs[0]->Id);
			}
			else if (m_tabs.empty())
			{
				// 没有标签页了，清空目录
				DocNavigationView().MenuItems().Clear();
				m_currentTabId = L"";
			}
		}
	}

	// --------------------------------------------------------------------------------------------
	// SwitchToTab
	// CN: 更新当前 Id，并为对应的页面加载/刷新目录（示例实现）。
	// EN: Update current Id and (re)load TOC for that page (sample implementation).
	// --------------------------------------------------------------------------------------------
	void WebViewPage::SwitchToTab(hstring const& tabId)
	{
		auto tabData = GetTabDataById(tabId);
		if (tabData)
		{
			m_currentTabId = tabId;
			// 更新目录导航
			PopulateNavigationView(tabData);
		}
	}

	// --------------------------------------------------------------------------------------------
	// UpdateTabTitle
	// CN: 刷新数据模型与 Header 文本（包含 Tooltip）。
	// EN: Update the data model and header text (with tooltip).
	// --------------------------------------------------------------------------------------------
	void WebViewPage::UpdateTabTitle(hstring const& tabId, hstring const& title)
	{
		auto tabData = GetTabDataById(tabId);
		if (tabData)
		{
			tabData->Title = title;

			// 更新TabViewItem的标题
			auto tabItems = WebTabView().TabItems();
			for (uint32_t i = 0; i < tabItems.Size(); ++i)
			{
				auto tabItem = tabItems.GetAt(i).as<TabViewItem>();
				auto itemTabId = winrt::unbox_value_or<hstring>(tabItem.Tag(), L"");
				if (itemTabId == tabId)
				{
					// 同步 Header 文本
					UpdateTabHeaderTitle(tabData, title);
					break;
				}
			}
		}
	}

	// --------------------------------------------------------------------------------------------
	// LoadDocumentStructure (sample TOC)
	// CN: 用示例数据填充侧边目录，真实项目里可由 ExtractTableOfContentsAsync 的 JS 结果构造。
	// EN: Populate the side TOC with sample data; in real project, build from JS result of ExtractTableOfContentsAsync.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::LoadDocumentStructure(hstring const& /*url*/)
	{
		// 根据URL生成示例目录结构
		auto navView = DocNavigationView();
		navView.MenuItems().Clear();

		// 添加一些示例导航项
		/*auto homeItem = NavigationViewItem();
		homeItem.Content(winrt::box_value(L"概述"));
		homeItem.Tag(winrt::box_value(L"overview"));
		homeItem.Icon(FontIcon());
		homeItem.Icon().as<FontIcon>().Glyph(L"\uE80F");
		navView.MenuItems().Append(homeItem);

		auto quickStartItem = NavigationViewItem();
		quickStartItem.Content(winrt::box_value(L"快速开始"));
		quickStartItem.Tag(winrt::box_value(L"quickstart"));
		quickStartItem.Icon(FontIcon());
		quickStartItem.Icon().as<FontIcon>().Glyph(L"\uE7C1");
		navView.MenuItems().Append(quickStartItem);

		auto tutorialItem = NavigationViewItem();
		tutorialItem.Content(winrt::box_value(L"教程"));
		tutorialItem.Tag(winrt::box_value(L"tutorial"));
		tutorialItem.Icon(FontIcon());
		tutorialItem.Icon().as<FontIcon>().Glyph(L"\uE8F1");
		navView.MenuItems().Append(tutorialItem);

		auto apiItem = NavigationViewItem();
		apiItem.Content(winrt::box_value(L"API 参考"));
		apiItem.Tag(winrt::box_value(L"api"));
		apiItem.Icon(FontIcon());
		apiItem.Icon().as<FontIcon>().Glyph(L"\uE943");
		navView.MenuItems().Append(apiItem);*/
	}

	// --------------------------------------------------------------------------------------------
	// NavigateToSection
	// CN: 注入 JS，通过 document.getElementById(id).scrollIntoView(...) 平滑滚动到页面元素。
	// EN: Inject JS to scroll smoothly to an element via document.getElementById(id).scrollIntoView(...).
	// 注意/Notes:
	// - ExecuteScriptAsync 返回字符串结果（JSON 编码的字符串），这里只是简单执行无返回处理。
	// --------------------------------------------------------------------------------------------
	void WebViewPage::NavigateToSection(hstring const& sectionId)
	{
		auto currentTab = GetCurrentTabData();
		if (currentTab && currentTab->WebView)
		{
			// 使用JavaScript导航到指定section - 使用WinRT字符串操作
			auto script = winrt::hstring(L"try { var element = document.getElementById('") +
				sectionId +
				winrt::hstring(L"'); if (element) { element.scrollIntoView({behavior: 'smooth'}); } } catch(e) { console.log('Navigation error:', e); }");
			currentTab->WebView.ExecuteScriptAsync(script);
		}
	}

	// --------------------------------------------------------------------------------------------
	// Save/Restore page state (stubs)
	// CN: 这里保留接口以供扩展。
	// EN: Placeholders to be implemented if persistence is needed.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::SavePageState()
	{
		try { /* TODO */ }
		catch (...) { /* ignore */ }
	}

	void WebViewPage::RestorePageState()
	{
		try { /* TODO */ }
		catch (...) { /* ignore */ }
	}

	// --------------------------------------------------------------------------------------------
	// BuildTabHeader
	// CN: 动态构建标签头部（图标 + 标题 + 进度圈），并将控件引用保存到 TabData，方便后续更新。
	// EN: Dynamically build the tab header (icon + title + progress) and cache references in TabData for updates.
	// --------------------------------------------------------------------------------------------
	UIElement WebViewPage::BuildTabHeader(std::shared_ptr<TabData> const& tabData)
	{
		// Grid with 3 columns: icon, text, progress
		Grid header{};
		ColumnDefinition col0{}; col0.Width(GridLengthHelper::FromValueAndType(0, GridUnitType::Auto));
		ColumnDefinition col1{}; col1.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
		ColumnDefinition col2{}; col2.Width(GridLengthHelper::FromValueAndType(0, GridUnitType::Auto));
		header.ColumnDefinitions().Append(col0);
		header.ColumnDefinitions().Append(col1);
		header.ColumnDefinitions().Append(col2);

		// icon
		tabData->HeaderIcon = FontIcon();
		tabData->HeaderIcon.Glyph(L"\uE7BE");
		tabData->HeaderIcon.FontSize(12);
		Thickness marginIcon{ 0,0,6,0 };
		tabData->HeaderIcon.Margin(marginIcon);
		header.Children().Append(tabData->HeaderIcon);
		Grid::SetColumn(tabData->HeaderIcon, 0);

		// text
		tabData->HeaderText = TextBlock();
		tabData->HeaderText.VerticalAlignment(VerticalAlignment::Center);
		tabData->HeaderText.Text(tabData->Title);
		tabData->HeaderText.TextTrimming(TextTrimming::CharacterEllipsis);
		tabData->HeaderText.MaxWidth(150);
		header.Children().Append(tabData->HeaderText);
		Grid::SetColumn(tabData->HeaderText, 1);

		// progress
		tabData->HeaderProgress = ProgressRing();
		tabData->HeaderProgress.Width(16);
		tabData->HeaderProgress.Height(16);
		Thickness marginProg{ 6,0,0,0 };
		tabData->HeaderProgress.Margin(marginProg);
		tabData->HeaderProgress.IsActive(tabData->IsLoading);
		tabData->HeaderProgress.Visibility(tabData->IsLoading ? Visibility::Visible : Visibility::Collapsed);
		header.Children().Append(tabData->HeaderProgress);
		Grid::SetColumn(tabData->HeaderProgress, 2);

		return header;
	}

	// --------------------------------------------------------------------------------------------
	// UpdateTabHeaderLoading
	// CN: 根据 isLoading 切换进度圈显示与动画状态。
	// EN: Toggle progress visibility and animation based on isLoading.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::UpdateTabHeaderLoading(std::shared_ptr<TabData> const& tabData, bool isLoading)
	{
		tabData->IsLoading = isLoading;
		if (tabData->HeaderProgress)
		{
			tabData->HeaderProgress.IsActive(isLoading);
			tabData->HeaderProgress.Visibility(isLoading ? Visibility::Visible : Visibility::Collapsed);
		}
	}

	// --------------------------------------------------------------------------------------------
	// UpdateTabHeaderTitle
	// CN: 更新标题文本，并设置 ToolTip 展示完整标题。
	// EN: Update the title text and set a tooltip for full title visibility.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::UpdateTabHeaderTitle(std::shared_ptr<TabData> const& tabData, hstring const& title)
	{
		if (tabData->HeaderText)
		{
			tabData->HeaderText.Text(title);
			ToolTipService::SetToolTip(tabData->HeaderText, box_value(title));
		}
	}

	// --------------------------------------------------------------------------------------------
	// CreateTabItem
	// CN: 创建 TabViewItem，设置自定义 Header，内容为 WebView2。内容对齐方式设为 Stretch 填满。
	// EN: Create TabViewItem, set custom header, content as WebView2. Align content stretch to fill.
	// --------------------------------------------------------------------------------------------
	TabViewItem WebViewPage::CreateTabItem(std::shared_ptr<TabData> tabData)
	{
		TabViewItem tabItem;
		tabItem.Tag(winrt::box_value(tabData->Id));
		// 确保内容拉伸填满
		tabItem.HorizontalContentAlignment(HorizontalAlignment::Stretch);
		tabItem.VerticalContentAlignment(VerticalAlignment::Stretch);

		// 设置自定义 Header
		auto header = BuildTabHeader(tabData);
		tabItem.Header(header);

		// 设置WebView2作为内容
		tabItem.Content(tabData->WebView);

		return tabItem;
	}

	// --------------------------------------------------------------------------------------------
	// CreateWebView
	// CN: 创建 WebView2 并订阅导航事件：
	//     - NavigationStarting: 标记加载中（显示进度圈）。
	//     - NavigationCompleted: 取消加载中；执行 JS 读取 document.title 更新标签标题；调用提取目录。
	// EN: Create WebView2 and subscribe to navigation events:
	//     - NavigationStarting: mark as loading (show progress ring).
	//     - NavigationCompleted: clear loading; Execute JS to read document.title and update title; extract TOC.
	// 注意/Notes:
	// - EnsureCoreWebView2Async() 必须在导航前调用一次初始化内核。
	// - ExecuteScriptAsync 返回 JSON 编码字符串，需要去除开头/结尾引号。
	// --------------------------------------------------------------------------------------------
	WebView2 WebViewPage::CreateWebView()
	{
		// 创建一个完整的WebView2控件 EN: Create a full WebView2 control
		WebView2 webView;
		// 拉伸以适应Tab内容
		webView.HorizontalAlignment(HorizontalAlignment::Stretch);
		webView.VerticalAlignment(VerticalAlignment::Stretch);

		// 配置WebView2事件，处理导航状态与标题更新 EN: Configure WebView2 events to handle navigation state and title updates
		// NavigationStarting API 来自 WebView2 控件，在导航开始时触发 EN: NavigationStarting API comes from WebView2 control, triggered at navigation start
		webView.NavigationStarting([this](auto const& sender, auto const& /*args*/) {
			// sender.as<WebView2>() 获取当前触发事件的 WebView2 实例 
			// EN: sender.as<WebView2>() gets the current WebView2 instance triggering the event
			auto webView = sender.as<WebView2>();
			for (auto& tabData : m_tabs)
			{
				if (tabData->WebView == webView)
				{
					UpdateTabHeaderLoading(tabData, true);
					break;
				}
			}
								   });

		// CN: 在导航完成后更新状态与标题 EN: On navigation complete, update status and title
		webView.NavigationCompleted([this](auto const& sender, auto const& /*args*/) {
			auto webView = sender.as<WebView2>();

			// 查找与我们保存变量相同的 TabData 如果匹配到对应的页面则更新状态与标题 EN: Find the corresponding TabData; if matched, update status and title
			for (auto& tabData : m_tabs)
			{
				if (tabData->WebView == webView)
				{
					UpdateTabHeaderLoading(tabData, false);

					// 获取页面标题（注意返回值是 JSON 字符串）  EN: Get the page title (note the return value is a JSON string)
					webView.ExecuteScriptAsync(L"document.title").Completed([this, tabData](auto const& asyncOp, auto const& status) {
						if (status == AsyncStatus::Completed)
						{
							try
							{
								auto result = asyncOp.GetResults();
								// 移除JSON字符串的引号
								auto title = std::wstring(result);
								if (title.size() >= 2 && title.front() == L'"' && title.back() == L'"')
								{
									title = title.substr(1, title.size() - 2);
								}
								UpdateTabTitle(tabData->Id, hstring(title));
							}
							catch (...)
							{
								// 忽略标题更新错误
							}
						}
																			});

					// 提取目录结构（示例）
					ExtractTableOfContentsAsync(tabData);
					break;
				}
			}
									});

		return webView;
	}

	// --------------------------------------------------------------------------------------------
	// LoadWebPageAsync
	// CN: 确保 WebView2 初始化（EnsureCoreWebView2Async）后设置 Source 导航到目标 URL。
	// EN: Ensure WebView2 core is initialized via EnsureCoreWebView2Async, then set Source to navigate.
	// 错误处理：若出错则停止加载动画并设置标题为 "Invalid URL"。
	// --------------------------------------------------------------------------------------------
	IAsyncAction WebViewPage::LoadWebPageAsync(std::shared_ptr<TabData> tabData)
	{
		if (tabData && tabData->WebView)
		{
			try
			{
				// 先确保初始化内核，再设置 Source 触发导航
				co_await tabData->WebView.EnsureCoreWebView2Async();

				Windows::Foundation::Uri uri(tabData->Url);
				tabData->WebView.Source(uri);
			}
			catch (...)
			{
				// 处理无效URL
				UpdateTabHeaderLoading(tabData, false);
				UpdateTabTitle(tabData->Id, L"Invalid URL");
			}
		}
		co_return;
	}

	// --------------------------------------------------------------------------------------------
	// GetCurrentTabData / GetTabDataById / GenerateTabId
	// CN: 简单的查找与递增 Id 生成部分。
	// EN: Simple getters and incrementing id generator.
	// --------------------------------------------------------------------------------------------
	std::shared_ptr<TabData> WebViewPage::GetCurrentTabData()
	{
		return GetTabDataById(m_currentTabId);
	}

	std::shared_ptr<TabData> WebViewPage::GetTabDataById(hstring const& tabId)
	{
		auto it = std::find_if(m_tabs.begin(), m_tabs.end(),
							   [&tabId](const std::shared_ptr<TabData>& tab) { return tab->Id == tabId; });

		return (it != m_tabs.end()) ? *it : nullptr;
	}

	hstring WebViewPage::GenerateTabId()
	{
		return hstring(L"tab_" + std::to_wstring(m_nextTabNumber++));
	}

	// --------------------------------------------------------------------------------------------
	// PopulateNavigationView / FilterNavigationItems / ExtractTableOfContentsAsync
	// CN: 目录相关方法（示例实现）。ExtractTableOfContentsAsync 通过执行 JS 获取标题列表，实际解析留作 TODO。
	// EN: TOC-related sample methods. ExtractTableOfContentsAsync runs JS to collect headers; parsing TBD.
	// --------------------------------------------------------------------------------------------
	void WebViewPage::PopulateNavigationView(std::shared_ptr<TabData> tabData)
	{
		if (!tabData)
			return;

		// 为当前页面加载目录结构
		LoadDocumentStructure(tabData->Url);
	}

	void WebViewPage::FilterNavigationItems(hstring const& searchText)
	{
		auto navView = DocNavigationView();

		if (searchText.empty())
		{
			// 显示所有项目
			for (auto const& item : navView.MenuItems())
			{
				auto navItem = item.as<NavigationViewItem>();
				navItem.Visibility(Visibility::Visible);
			}
		}
		else
		{
			// 过滤项目 - 使用简单的字符串匹配
			auto searchStr = searchText.c_str();

			for (auto const& item : navView.MenuItems())
			{
				auto navItem = item.as<NavigationViewItem>();
				auto content = navItem.Content().as<hstring>();
				auto contentStr = content.c_str();

				bool isVisible = wcsstr(contentStr, searchStr) != nullptr;
				navItem.Visibility(isVisible ? Visibility::Visible : Visibility::Collapsed);
			}
		}
	}

	IAsyncAction WebViewPage::ExtractTableOfContentsAsync(std::shared_ptr<TabData> tabData)
	{
		if (!tabData || !tabData->WebView)
			co_return;

		try
		{
			// 使用JavaScript提取页面目录
			auto script = L"try { var headers = Array.from(document.querySelectorAll('h1, h2, h3, h4, h5, h6')); headers.map(h => ({text: h.textContent, id: h.id, level: parseInt(h.tagName.substring(1))})); } catch(e) { []; }";

			auto result = co_await tabData->WebView.ExecuteScriptAsync(script);

			// TODO: 解析 JSON 结果，并据此动态填充 NavigationView.MenuItems
			// TODO(EN): Parse the JSON result and populate NavigationView.MenuItems dynamically
		}
		catch (...)
		{
			// 如果JavaScript执行失败，使用默认的目录结构
			LoadDocumentStructure(tabData->Url);
		}
	}

	// TabView 键盘事件处理部分
#pragma region KeyboardHandler

	// CN: 处理 Ctrl+T 新建标签，Ctrl+W 关闭当前标签，Ctrl+1~9 切换到对应编号标签。 
	// EN: Handle Ctrl+T to add tab, Ctrl+W to close current tab, Ctrl+1~9 to switch to numbered tab.
	void WebViewPage::NewTabKeyboardAccelerator_Invoked(Microsoft::UI::Xaml::Input::KeyboardAccelerator const& /*sender*/,
														Microsoft::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args)
	{
		// 获取触发的 TabView（若为空则回退到页面上的 WebTabView）
		Microsoft::UI::Xaml::Controls::TabView tabView = nullptr;
		if (args.Element())
		{
			tabView = args.Element().try_as<Microsoft::UI::Xaml::Controls::TabView>();
		}
		if (!tabView)
		{
			tabView = WebTabView();
		}

		// 新建标签页（与“+”按钮一致的默认行为）
		AddNewTab(L"https://learn.microsoft.com/", L"New Tab");
		args.Handled(true);
	}

	void WebViewPage::CloseSelectedTabKeyboardAccelerator_Invoked(Microsoft::UI::Xaml::Input::KeyboardAccelerator const& /*sender*/,
																  Microsoft::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args)
	{
		// 获取触发的 TabView（若为空则回退到页面上的 WebTabView）
		Microsoft::UI::Xaml::Controls::TabView invokedTabView = nullptr;
		if (args.Element())
		{
			invokedTabView = args.Element().try_as<Microsoft::UI::Xaml::Controls::TabView>();
		}
		if (!invokedTabView)
		{
			invokedTabView = WebTabView();
		}

		// 仅在选中项可关闭时关闭
		if (auto selected = invokedTabView.SelectedItem())
		{
			auto selectedItem = selected.as<Microsoft::UI::Xaml::Controls::TabViewItem>();
			if (selectedItem && selectedItem.IsClosable())
			{
				auto tabId = winrt::unbox_value_or<winrt::hstring>(selectedItem.Tag(), L"");
				if (!tabId.empty())
				{
					CloseTab(tabId);
				}
			}
		}

		args.Handled(true);
	}

	void WebViewPage::NavigateToNumberedTabKeyboardAccelerator_Invoked(Microsoft::UI::Xaml::Input::KeyboardAccelerator const& sender,
																	   Microsoft::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args)
	{
		// 获取触发的 TabView（若为空则回退到页面上的 WebTabView）
		Microsoft::UI::Xaml::Controls::TabView invokedTabView = nullptr;
		if (args.Element())
		{
			invokedTabView = args.Element().try_as<Microsoft::UI::Xaml::Controls::TabView>();
		}
		if (!invokedTabView)
		{
			invokedTabView = WebTabView();
		}

		int32_t tabToSelect = 0;
		switch (sender.Key())
		{
		case winrt::Windows::System::VirtualKey::Number1: tabToSelect = 0; break;
		case winrt::Windows::System::VirtualKey::Number2: tabToSelect = 1; break;
		case winrt::Windows::System::VirtualKey::Number3: tabToSelect = 2; break;
		case winrt::Windows::System::VirtualKey::Number4: tabToSelect = 3; break;
		case winrt::Windows::System::VirtualKey::Number5: tabToSelect = 4; break;
		case winrt::Windows::System::VirtualKey::Number6: tabToSelect = 5; break;
		case winrt::Windows::System::VirtualKey::Number7: tabToSelect = 6; break;
		case winrt::Windows::System::VirtualKey::Number8: tabToSelect = 7; break;
		case winrt::Windows::System::VirtualKey::Number9:
		{
			auto count = static_cast<int32_t>(invokedTabView.TabItems().Size());
			tabToSelect = count > 0 ? (count - 1) : 0;
		}
		break;
		default:
			break;
		}

		auto count = static_cast<int32_t>(invokedTabView.TabItems().Size());
		if (count > 0 && tabToSelect >= 0 && tabToSelect < count)
		{
			invokedTabView.SelectedIndex(tabToSelect);
		}

		args.Handled(true);
	}
#pragma endregion
}
