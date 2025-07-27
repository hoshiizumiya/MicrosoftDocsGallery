#pragma once

#include "FundamentalsPage.g.h"
#include <vector>

namespace winrt::MicrosoftDocsGallery::implementation
{
    // 学习主题数据结构
    struct LearningTopic
    {
        winrt::hstring Title;
        winrt::hstring Description;
        winrt::hstring Url;
        winrt::hstring Icon;
        winrt::hstring Category;
        
        LearningTopic() = default;
        LearningTopic(winrt::hstring title, winrt::hstring desc, winrt::hstring url, winrt::hstring icon, winrt::hstring category)
            : Title(title), Description(desc), Url(url), Icon(icon), Category(category) {}
    };

    struct FundamentalsPage : FundamentalsPageT<FundamentalsPage>
    {
        FundamentalsPage();

        // 事件处理器
        void TopicCard_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void CategoryFilter_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void SearchBox_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);

    private:
        // 辅助方法
        void InitializeTopics();
        void PopulateTopicsView();
        void FilterTopics();
        void NavigateToTopic(winrt::hstring const& url);

        // 数据
        std::vector<LearningTopic> m_allTopics;
        std::vector<LearningTopic> m_filteredTopics;
        winrt::hstring m_selectedCategory = L"All";
        winrt::hstring m_searchText = L"";
    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct FundamentalsPage : FundamentalsPageT<FundamentalsPage, implementation::FundamentalsPage>
    {
    };
}
