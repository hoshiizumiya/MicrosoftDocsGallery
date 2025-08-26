#pragma once

#include "WebViewPage.g.h"
#include <vector>
#include <memory>
#include <map>

namespace winrt::MicrosoftDocsGallery::implementation
{
	// -------------------------------------------------------------------------------------------------
	// TabData
	// 作用(CN): 表示一个浏览标签页的数据模型，保存与该标签关联的 WebView2 实例、标题、加载状态等信息。
	// Purpose(EN): Data model for a single browsing tab, holding its WebView2 instance, title, URL,
	//              loading state, and references to custom header UI elements for easy updates.
	// -------------------------------------------------------------------------------------------------
	struct TabData
	{
		// 标签唯一标识
		// CN: 每个标签的唯一 Id，用于在集合中索引与切换
		// EN: Unique Id per tab, used for indexing and switching
		winrt::hstring Id;

		// 标签标题
		// CN: 当前页面标题（初始可为占位，如 "Loading..."）
		// EN: Current page title (can be a placeholder like "Loading...")
		winrt::hstring Title;

		// 目标 URL
		// CN: 该标签要导航的 URL
		// EN: The URL this tab navigates to
		winrt::hstring Url;

		// 加载状态
		// CN: 是否处于加载中，用于切换进度圈显示
		// EN: Is the page loading; drives the progress ring visibility
		bool IsLoading = false;

		// WebView2 控件实例
		// CN: 承载网页内容的控件
		// EN: The control that hosts web content
		winrt::Microsoft::UI::Xaml::Controls::WebView2 WebView = nullptr;

		// 自定义 Header 元素句柄
		// CN: 用于快速更新标签头部的图标、标题文本、进度圈
		// EN: References to update the custom tab header (icon, title text, progress)
		winrt::Microsoft::UI::Xaml::Controls::FontIcon HeaderIcon{ nullptr };
		winrt::Microsoft::UI::Xaml::Controls::TextBlock HeaderText{ nullptr };
		winrt::Microsoft::UI::Xaml::Controls::ProgressRing HeaderProgress{ nullptr };

		TabData() = default;
	};

	// -------------------------------------------------------------------------------------------------
	// WebViewPage
	// 作用(CN): 包含目录导航与多标签 WebView2 浏览区域的页面，负责标签的创建、关闭、切换、以及目录构建。
	// Purpose(EN): Page hosting a NavigationView (TOC) and a TabView with WebView2 contents; manages
	//              tab creation/closing/switching and table-of-contents population.
	// -------------------------------------------------------------------------------------------------
	struct WebViewPage : WebViewPageT<WebViewPage>
	{
		WebViewPage();

		// 页面导航进入事件
		// CN: 首次进入时根据参数或默认 URL 创建一个标签页
		// EN: On first navigation, creates a tab using parameter URL or a default URL
		void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);

		// 页面加载完成事件（兜底）
		// CN: 若因某些路径未创建任何标签，则在 Loaded 时确保至少有一个默认标签
		// EN: Fallback to ensure at least one default tab exists once the visual tree is loaded
		void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

		// TabView 事件
		// CN: 新增标签、请求关闭标签、选择变化
		// EN: TabView events for add, close-request, and selection changes
		void TabView_AddButtonClick(winrt::Microsoft::UI::Xaml::Controls::TabView const& sender, winrt::Windows::Foundation::IInspectable const& args);
		void TabView_TabCloseRequested(winrt::Microsoft::UI::Xaml::Controls::TabView const& sender, winrt::Microsoft::UI::Xaml::Controls::TabViewTabCloseRequestedEventArgs const& args);
		void TabView_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

		// NavigationView（目录）事件
		// CN: 点击目录项进行页面滚动定位；输入框过滤目录项
		// EN: Invoke TOC item to scroll, filter TOC items by search box
		void DocNavigation_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
		void SectionSearch_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);

		// 顶部按钮：收藏、刷新、浏览器打开、分享
		// CN: 处理顶部快捷操作
		// EN: Handle top quick actions
		void FavoriteButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void RefreshPage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void OpenInBrowser_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void SharePage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

		// 公共 API（供外部调用）
		// CN: 新增/关闭/切换标签、更新标题、加载目录、定位到页面某个片段、状态保存/恢复
		// EN: Public APIs to add/close/switch tabs, update title, load TOC, navigate to a section, save/restore state
		void AddNewTab(winrt::hstring const& url, winrt::hstring const& title);
		void CloseTab(winrt::hstring const& tabId);
		void SwitchToTab(winrt::hstring const& tabId);
		void UpdateTabTitle(winrt::hstring const& tabId, winrt::hstring const& title);
		void LoadDocumentStructure(winrt::hstring const& url);
		void NavigateToSection(winrt::hstring const& sectionId);
		void SavePageState();
		void RestorePageState();

		// 帮助方法（内部实现）
		// CN: 创建 Tab 项、创建 WebView2、异步加载页面、查找当前标签/按 Id 查找、生成标签 Id
		// EN: Internal helpers to create a TabViewItem, create a WebView2, load page asynchronously,
		//     get current tab or find by id, and generate unique tab ids
		winrt::Microsoft::UI::Xaml::Controls::TabViewItem CreateTabItem(std::shared_ptr<TabData> tabData);
		winrt::Microsoft::UI::Xaml::Controls::WebView2 CreateWebView();
		winrt::Windows::Foundation::IAsyncAction LoadWebPageAsync(std::shared_ptr<TabData> tabData);
		std::shared_ptr<TabData> GetCurrentTabData();
		std::shared_ptr<TabData> GetTabDataById(winrt::hstring const& tabId);
		winrt::hstring GenerateTabId();

		// 目录相关（示例实现）
		// CN: 切换标签时填充目录、按文本过滤、从页面提取目录（示例 JS）
		// EN: Populate TOC on tab switch, filter by text, extract TOC via sample JavaScript
		void PopulateNavigationView(std::shared_ptr<TabData> tabData);
		void FilterNavigationItems(winrt::hstring const& searchText);
		winrt::Windows::Foundation::IAsyncAction ExtractTableOfContentsAsync(std::shared_ptr<TabData> tabData);

		// 自定义 Header 构建与更新
		// CN: 构建头部 UI（图标+标题+进度圈）；在加载状态或标题变化时刷新头部
		// EN: Build custom header (icon + title + progress); refresh header on loading/title changes
		winrt::Microsoft::UI::Xaml::UIElement BuildTabHeader(std::shared_ptr<TabData> const& tabData);
		void UpdateTabHeaderLoading(std::shared_ptr<TabData> const& tabData, bool isLoading);
		void UpdateTabHeaderTitle(std::shared_ptr<TabData> const& tabData, winrt::hstring const& title);

		// 成员变量
		// CN: 标签集合、当前标签 Id、初始化标记、递增 Id 序列
		// EN: Tab collection, current tab id, init flag, and incrementing id sequence
		std::vector<std::shared_ptr<TabData>> m_tabs;
		winrt::hstring m_currentTabId;
		bool m_isInitialized = false;
		int m_nextTabNumber = 1;

		// 键盘快捷键处理
		// CN: 处理 Ctrl+T（新标签）、Ctrl+W（关闭标签）、Ctrl+1~9（切换标签）
		// EN: Handle Ctrl+T (new tab), Ctrl+W (close tab), Ctrl+1~9 (switch to tab)
		void NewTabKeyboardAccelerator_Invoked(Microsoft::UI::Xaml::Input::KeyboardAccelerator const&, Microsoft::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args);
		void CloseSelectedTabKeyboardAccelerator_Invoked(Microsoft::UI::Xaml::Input::KeyboardAccelerator const&, Microsoft::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args);
		void NavigateToNumberedTabKeyboardAccelerator_Invoked(Microsoft::UI::Xaml::Input::KeyboardAccelerator const& sender, Microsoft::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args);

	};
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
	struct WebViewPage : WebViewPageT<WebViewPage, implementation::WebViewPage>
	{
	};
}
