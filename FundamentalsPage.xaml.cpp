#include "pch.h"
#include "FundamentalsPage.xaml.h"
#if __has_include("FundamentalsPage.g.cpp")
#include "FundamentalsPage.g.cpp"
#endif
#include <algorithm>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::MicrosoftDocsGallery::implementation
{
    FundamentalsPage::FundamentalsPage()
    {
        InitializeComponent();
        InitializeTopics();
        PopulateTopicsView();
    }

    void FundamentalsPage::TopicCard_Click(IInspectable const& sender, RoutedEventArgs const& /*e*/)
    {
        auto button = sender.as<Button>();
        auto url = button.Tag().as<hstring>();
        
        if (!url.empty())
        {
            NavigateToTopic(url);
        }
    }

    void FundamentalsPage::CategoryFilter_SelectionChanged(IInspectable const& sender, SelectionChangedEventArgs const& /*e*/)
    {
        auto comboBox = sender.as<ComboBox>();
        auto selectedItem = comboBox.SelectedItem().as<ComboBoxItem>();
        
        if (selectedItem)
        {
            m_selectedCategory = selectedItem.Tag().as<hstring>();
            FilterTopics();
        }
    }

    void FundamentalsPage::SearchBox_TextChanged(IInspectable const& sender, TextChangedEventArgs const& /*e*/)
    {
        auto textBox = sender.as<TextBox>();
        m_searchText = textBox.Text();
        FilterTopics();
    }

    void FundamentalsPage::InitializeTopics()
    {
        m_allTopics.clear();
        
        // WinUI 主题
        m_allTopics.emplace_back(
            L"WinUI 3 入门",
            L"学习如何使用WinUI 3创建现代Windows应用程序，包括XAML基础和控件使用。",
            L"https://learn.microsoft.com/windows/apps/winui/winui3/",
            L"\uE8A5", // 应用图标
            L"WinUI"
        );
        
        m_allTopics.emplace_back(
            L"XAML 平台",
            L"深入了解XAML标记语言，掌握数据绑定、样式和模板的使用方法。",
            L"https://learn.microsoft.com/windows/apps/xaml-platform/",
            L"\uE943", // 代码图标
            L"WinUI"
        );
        
        m_allTopics.emplace_back(
            L"应用架构指南",
            L"学习如何设计和构建可维护的Windows应用程序架构。",
            L"https://learn.microsoft.com/windows/apps/design/",
            L"\uE9D9", // 架构图标
            L"WinUI"
        );

        // .NET 主题
        m_allTopics.emplace_back(
            L".NET 基础",
            L"掌握.NET平台的核心概念，包括运行时、框架和工具链。",
            L"https://learn.microsoft.com/dotnet/fundamentals/",
            L"\uE756", // 设置图标
            L".NET"
        );
        
        m_allTopics.emplace_back(
            L"ASP.NET Core",
            L"学习如何使用ASP.NET Core构建现代Web应用程序和API。",
            L"https://learn.microsoft.com/aspnet/core/",
            L"\uE774", // 网络图标
            L".NET"
        );

        // C# 主题
        m_allTopics.emplace_back(
            L"C# 编程指南",
            L"从基础语法到高级特性，全面掌握C#编程语言。",
            L"https://learn.microsoft.com/dotnet/csharp/",
            L"\uE943", // 代码图标
            L"C#"
        );
        
        m_allTopics.emplace_back(
            L"异步编程",
            L"学习C#中的async/await模式，掌握异步和并行编程技术。",
            L"https://learn.microsoft.com/dotnet/csharp/asynchronous-programming/",
            L"\uE9F3", // 同步图标
            L"C#"
        );

        // Azure 主题
        m_allTopics.emplace_back(
            L"Azure 基础",
            L"了解Azure云平台的核心服务和概念，开始你的云计算之旅。",
            L"https://learn.microsoft.com/azure/",
            L"\uE753", // 云图标
            L"Azure"
        );
        
        m_allTopics.emplace_back(
            L"Azure DevOps",
            L"学习如何使用Azure DevOps进行项目管理、CI/CD和应用部署。",
            L"https://learn.microsoft.com/azure/devops/",
            L"\uE90F", // 部署图标
            L"Azure"
        );

        // Web开发主题
        m_allTopics.emplace_back(
            L"Blazor 框架",
            L"使用C#构建交互式Web应用程序，无需JavaScript。",
            L"https://learn.microsoft.com/aspnet/core/blazor/",
            L"\uE774", // 网络图标
            L"Web开发"
        );
        
        m_allTopics.emplace_back(
            L"Web API 设计",
            L"学习RESTful API设计最佳实践和ASP.NET Core Web API开发。",
            L"https://learn.microsoft.com/aspnet/core/web-api/",
            L"\uE968", // API图标
            L"Web开发"
        );

        // 初始化过滤后的主题
        m_filteredTopics = m_allTopics;
    }

    void FundamentalsPage::PopulateTopicsView()
    {
        // 清空当前项目
        TopicsGridView().Items().Clear();
        
        // 为每个过滤后的主题创建UI元素
        for (const auto& topic : m_filteredTopics)
        {
            // 创建Border容器
            Border container;
            container.Width(280);
            container.Height(200);
            container.Padding(Thickness{ 16, 16, 16, 16 });
            
            // 创建内容StackPanel
            StackPanel contentPanel;
            contentPanel.Spacing(12);
            
            // 图标和标题行
            StackPanel headerPanel;
            headerPanel.Orientation(Orientation::Horizontal);
            headerPanel.Spacing(12);
            
            FontIcon icon;
            icon.Glyph(topic.Icon);
            icon.FontSize(24);
            
            TextBlock titleBlock;
            titleBlock.Text(topic.Title);
            titleBlock.FontSize(16);
            titleBlock.TextWrapping(TextWrapping::Wrap);
            
            headerPanel.Children().Append(icon);
            headerPanel.Children().Append(titleBlock);
            
            // 描述文本
            TextBlock descBlock;
            descBlock.Text(topic.Description);
            descBlock.TextWrapping(TextWrapping::Wrap);
            descBlock.FontSize(14);
            descBlock.MaxLines(3);
            descBlock.TextTrimming(TextTrimming::CharacterEllipsis);
            
            // 分类标签
            Border categoryBorder;
            Microsoft::UI::Xaml::CornerRadius cornerRadius;
            cornerRadius.TopLeft = 12;
            cornerRadius.TopRight = 12; 
            cornerRadius.BottomLeft = 12;
            cornerRadius.BottomRight = 12;
            categoryBorder.CornerRadius(cornerRadius);
            categoryBorder.Padding(Thickness{ 8, 4, 8, 4 });
            categoryBorder.HorizontalAlignment(HorizontalAlignment::Left);
            
            TextBlock categoryBlock;
            categoryBlock.Text(topic.Category);
            categoryBlock.FontSize(12);
            
            categoryBorder.Child(categoryBlock);
            
            // 开始学习按钮
            Button learnButton;
            learnButton.Content(winrt::box_value(L"开始学习"));
            learnButton.HorizontalAlignment(HorizontalAlignment::Stretch);
            learnButton.Tag(winrt::box_value(topic.Url));
            learnButton.Click({ this, &FundamentalsPage::TopicCard_Click });
            
            // 组装内容
            contentPanel.Children().Append(headerPanel);
            contentPanel.Children().Append(descBlock);
            contentPanel.Children().Append(categoryBorder);
            contentPanel.Children().Append(learnButton);
            
            container.Child(contentPanel);
            
            // 添加到GridView
            TopicsGridView().Items().Append(container);
        }
    }

    void FundamentalsPage::FilterTopics()
    {
        m_filteredTopics.clear();
        
        for (auto const& topic : m_allTopics)
        {
            bool categoryMatch = (m_selectedCategory == L"All" || topic.Category == m_selectedCategory);
            
            // 使用WinRT字符串操作进行文本匹配
            bool textMatch = m_searchText.empty();
            if (!textMatch)
            {
                auto searchLower = m_searchText.c_str();
                auto titleLower = topic.Title.c_str();
                auto descLower = topic.Description.c_str();
                
                // 使用简单的包含检查，避免复杂的字符串转换
                textMatch = wcsstr(titleLower, searchLower) != nullptr ||
                           wcsstr(descLower, searchLower) != nullptr;
            }
            
            if (categoryMatch && textMatch)
            {
                m_filteredTopics.push_back(topic);
            }
        }
        
        PopulateTopicsView();
    }

    void FundamentalsPage::NavigateToTopic(hstring const& url)
    {
        // 在WinUI 3中，通过页面的Parent层次查找MainWindow
        auto parent = this->Parent();
        while (parent)
        {
            // 尝试向上查找MainWindow
            auto mainWindow = parent.try_as<MicrosoftDocsGallery::MainWindow>();
            if (mainWindow)
            {
                mainWindow.openWebViewPage(url);
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
}
