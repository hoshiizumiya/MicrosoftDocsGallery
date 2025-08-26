MicrosoftDocsGallery
====================
🪄一个使用 Windows App SDK、WinUI 3 与 C++/WinRT 构建的示例型桌面应用，演示在标准 C++/WinRT 环境中如何使用 XAML、IDL、WinRT 组件与现代异步模式搭建文档浏览体验：主页聚合精选内容、标签页式 WebView2 浏览器、可扩展的收藏与统计服务，以及符合 MVVM 思想的视图与视图模型分层。

特性总览
- WinUI 3 桌面应用，XAML 构建 UI，支持主题、样式与资源字典
- WebView2 内嵌浏览，标签(TabView)管理与多标签切换
- 目录(NavigationView)与页面内导航，支持搜索过滤
- 可替换的服务层(Data/Favorites/Settings)，通过简单 ServiceLocator 解耦
- ViewModel 层提供命令与通知属性，便于数据绑定
- 示例数据模型与扩展点，便于快速迭代

技术栈
- 语言与标准：C++/WinRT (C++17)
- UI 框架：WinUI 3 (Windows App SDK)
- 组件系统：C++/WinRT（IDL 生成头与 WinRT 投影）

依赖与环境
- 操作系统：Windows 10 1809 及以上（推荐 Windows 11）
- 必需组件：
  - Visual Studio 2022（安装“使用 C++ 的桌面开发”工作负载）
  - Windows App SDK（对应 VS 扩展与 NuGet 包）
  - Windows SDK（与项目配置一致）
  - Microsoft Edge WebView2 Runtime

目录结构
- App.xaml / App.xaml.cpp：应用入口、窗口创建、全局样式与资源加载
- MainWindow.idl：主窗口本地 WinRT 声明（XAML 支持）
- Pages / XAML：
  - HomePage.xaml(.*)：主页与入口导航
  - WebViewPage.xaml(.*)：WebView2 标签浏览页（TabView + WebView2 + NavigationView）
  - SettingsPage.xaml(.*)：设置示例页
  - FavoritesPage.xaml(.*)：收藏示例页
- ViewModels：
  - HomePageViewModel.(h|cpp)：示例 VM，演示异步加载、命令、通知属性
- Services：
  - Services.h / Services.cpp：服务接口与默认实现（Data/Favorites/Settings）与 ServiceLocator
  - Services.idl：用于 WinRT 类型生成的占位。当前服务层采用“纯 C++ 接口 + 实现”，避免与 WinRT 生成冲突
- Models：
  - Models.h：示例模型实体（LearningTopic/AppStatistics 等）
- Helpers：
  - NavItemIconHelper.idl / .cpp：示例 WinRT 辅助类型（图标帮助）
- Styles：
  - NavigationView.xaml：样式与主题资源

构建与运行
- 使用 Visual Studio 打开解决方案并还原 NuGet 包
- 选择 x64 Debug 或 Release
- 生成并运行（F5）
- 如首次运行 WebView2 页面空白，请确认系统已安装 WebView2 Runtime

快速上手
1) 主页与数据
- 首页通过 HomePageViewModel 加载精选主题与统计信息
- 异步方法使用 IAsyncAction/IAsyncOperation 返回类型
- 命令（Refresh/Navigate 等）用于绑定按钮交互

2) 标签页浏览（WebView2）
- WebViewPage 支持多个标签：
  - TabView 动态添加、关闭与选择
  - 每个标签承载一个 WebView2 控件
  - 使用 EnsureCoreWebView2Async 初始化，再设置 Source 导航
  - NavigationCompleted 回调中提取 document.title 更新标签标题
  - 目录区用 NavigationView 呈现，支持输入框过滤
- 提供按钮：刷新、在默认浏览器打开、分享（预留）

3) 服务层（可替换/可测试）
- IDataService/IFavoritesService/ISettingsService 为纯 C++ 接口
- 默认实现：
  - DataService：返回示例主题数据，支持刷新与简单缓存逻辑
  - FavoritesService：内存收藏与异步存取桩（可扩展本地/漫游存储）
  - SettingsService：示例统计值读写（可扩展持久化）
- ServiceLocator 提供默认实例与注册接口，避免到处 new

4) XAML/IDL 与生成文件
- XAML 与 IDL 会生成实现头与源（位于 Generated Files 目录）
- 对应类逻辑请放在项目源文件中，避免直接改生成文件
- 本项目的 Services.idl 留空以避免与服务层类产生重名投影冲突
- 新增需要 XAML 识别/绑定的本地 WinRT 类型时，再编写对应 IDL

扩展指南
- 新增服务
  - 在 Services.h 定义接口与实现类
  - 在 Services.cpp 实现并在 ServiceLocator 中注册或按需懒加载
  - 在 ViewModel 中通过 ServiceLocator 获取实例
- 新增页面
  - 新建 XAML 页面与 .idl（如需本地 WinRT）
  - 在 .xaml.cpp 绑定 VM（可新增对应 ViewModel）
  - 在导航处注册路由并进行页面跳转
- 新增导航目录/搜索
  - 参考 WebViewPage 的 PopulateNavigationView 与 FilterNavigationItems
  - 解析真实 HTML 目录，扩展 ExecuteScriptAsync 解析 JSON 结果

编码约定与建议
- 使用智能指针管理服务生命周期（std::shared_ptr）、避免C裸指针
- 异步方法优先使用 IAsyncAction/IAsyncOperation，统一协程风格
- 捕获异常并在 UI 层降级处理，避免抛至消息循环
- 保持 UI 操作在 UI 线程，耗时操作放到后台后 co_await 回到 UI
- View 与 ViewModel 解耦，事件通过 event 或命令回调沟通

常见问题与排错
- 构建错误：清理后重建；确保 Windows App SDK 与 Windows SDK 版本与项目一致
- 生成文件冲突：参考根据自定义 .idl 文件输出的 Generated Files 下的原始文件

路线图
- 完成 Favorites/Settings 的本地持久化存储
- 为目录解析引入真实 JSON 解析并动态构建 NavigationView
- 增加设置页 UI 与主题切换
- 编写基础单元测试与 UI 自动化脚本（可选）

贡献
- 提交 PR 前请保持代码风格一致，避免修改生成文件
- 变更服务接口时请同步更新默认实现与使用方
- 请附带说明、截图或复现步骤

版权与许可
- 使用前请根据仓库 License 文件（如有）遵循相应条款
- 所含第三方组件遵循其各自的许可证

附录：与本项目配套的资料
- Windows App SDK：https://learn.microsoft.com/windows/apps/windows-app-sdk/
- WinUI 3：https://learn.microsoft.com/windows/apps/winui/winui3/
- C++/WinRT：https://aka.ms/cppwinrt
- WebView2：https://learn.microsoft.com/microsoft-edge/webview2/

========================================================================
    MicrosoftDocsGallery Project Overview
========================================================================

This project demonstrates how to get started writing WinUI3 apps directly
with standard C++, using the Windows App SDK and C++/WinRT packages and
XAML compiler support to generate implementation headers from interface
(IDL) files. These headers can then be used to implement the local
Windows Runtime classes referenced in the app's XAML pages.

Steps:
1. Create an interface (IDL) file to define any local Windows Runtime
    classes referenced in the app's XAML pages.
2. Build the project once to generate implementation templates under
    the "Generated Files" folder, as well as skeleton class definitions
    under "Generated Files\sources".
3. Use the skeleton class definitions for reference to implement your
    Windows Runtime classes.

========================================================================
Learn more about Windows App SDK here:
https://docs.microsoft.com/windows/apps/windows-app-sdk/
Learn more about WinUI3 here:
https://docs.microsoft.com/windows/apps/winui/winui3/
Learn more about C++/WinRT here:
http://aka.ms/cppwinrt/
========================================================================
