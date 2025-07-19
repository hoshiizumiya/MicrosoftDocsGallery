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
        // 默认静态主题，防止网络失败时无内容
        std::vector<hstring> topics = {
            L"WinUI 3", L".NET", L"Azure", L"Power Platform", L"Microsoft Graph", L"Visual Studio", L"Windows", L"Office"
        };
        auto carousel = this->FindName(L"CardCarousel").try_as<ItemsControl>();
        auto items = single_threaded_observable_vector<IInspectable>();
        for (auto&& t : topics) items.Append(box_value(t));
        if (carousel) carousel.ItemsSource(items);

        // 异步获取微软文档主题
        auto weakThis = get_weak();
        std::thread([weakThis, items]() {
            try {
                HttpClient client;
                Uri uri{ L"https://learn.microsoft.com/api/catalog?locale=zh-cn&$top=8" };
                auto resp = client.GetAsync(uri).get();
                if (resp.IsSuccessStatusCode()) {
                    auto jsonStr = resp.Content().ReadAsStringAsync().get();
                    JsonObject root;
                    if (JsonObject::TryParse(jsonStr, root)) {
                        if (root.HasKey(L"resources")) {
                            auto arr = root.GetNamedArray(L"resources");
                            items.Clear();
                            for (uint32_t i = 0; i < arr.Size(); ++i) {
                                auto obj = arr.GetAt(i).GetObject();
                                if (obj.HasKey(L"title")) {
                                    items.Append(box_value(obj.GetNamedString(L"title")));
                                }
                            }
                        }
                    }
                }
            } catch (...) {
                // 网络异常忽略，保留静态内容
            }
        }).detach();

        // 自动滑动
        auto scrollView = this->FindName(L"scrollView").try_as<ScrollViewer>();
        if (scrollView)
        {
            auto weakScroll = winrt::make_weak(scrollView);
            auto dispatcherQueue = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
            std::thread([weakScroll, dispatcherQueue]() {
                double offset = 0;
                while (true) {
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
}
