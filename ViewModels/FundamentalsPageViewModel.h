#pragma once
#include "BindableBase.h"
#include "..\Models\Models.h"
#include "..\Services\Services.h"
#include <vector>
#include <algorithm>

namespace winrt::MicrosoftDocsGallery::ViewModels
{
    using namespace Models;
    using namespace Services;

    struct FundamentalsPageViewModel : public BindableBase
    {
    public:
        FundamentalsPageViewModel();

        // 数据属性
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> Topics() const
        {
            return m_filteredTopics.GetVector();
        }

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> Categories() const
        {
            return m_categories.GetVector();
        }

        // 筛选属性
        winrt::hstring SelectedCategory() const { return m_selectedCategory; }
        void SelectedCategory(winrt::hstring const& value)
        {
            if (SetProperty(m_selectedCategory, value, L"SelectedCategory"))
            {
                FilterTopicsAsync();
            }
        }

        winrt::hstring SearchText() const { return m_searchText; }
        void SearchText(winrt::hstring const& value)
        {
            if (SetProperty(m_searchText, value, L"SearchText"))
            {
                FilterTopicsAsync();
            }
        }

        // 状态属性
        bool IsLoading() const { return m_isLoading; }
        void IsLoading(bool value) { SetProperty(m_isLoading, value, L"IsLoading"); }

        int TotalTopicsCount() const { return m_totalTopicsCount; }
        void TotalTopicsCount(int value) { SetProperty(m_totalTopicsCount, value, L"TotalTopicsCount"); }

        int FilteredTopicsCount() const { return m_filteredTopicsCount; }
        void FilteredTopicsCount(int value) { SetProperty(m_filteredTopicsCount, value, L"FilteredTopicsCount"); }

        // 命令
        winrt::Microsoft::UI::Xaml::Input::ICommand TopicSelectedCommand() const { return *m_topicSelectedCommand; }
        winrt::Microsoft::UI::Xaml::Input::ICommand RefreshCommand() const { return *m_refreshCommand; }
        winrt::Microsoft::UI::Xaml::Input::ICommand ClearFiltersCommand() const { return *m_clearFiltersCommand; }

        // 公共方法
        winrt::Windows::Foundation::IAsyncAction InitializeAsync();
        winrt::Windows::Foundation::IAsyncAction RefreshTopicsAsync();

        // 事件
        winrt::event_token TopicNavigationRequested(winrt::Windows::Foundation::EventHandler<winrt::hstring> const& handler);
        void TopicNavigationRequested(winrt::event_token const& token) noexcept;

    private:
        // 数据
        ObservableVector<winrt::Windows::Foundation::IInspectable> m_allTopics;
        ObservableVector<winrt::Windows::Foundation::IInspectable> m_filteredTopics;
        ObservableVector<winrt::Windows::Foundation::IInspectable> m_categories;
        
        // 筛选状态
        winrt::hstring m_selectedCategory{ L"All" };
        winrt::hstring m_searchText;
        
        // UI 状态
        bool m_isLoading{ false };
        int m_totalTopicsCount{ 0 };
        int m_filteredTopicsCount{ 0 };

        // 服务
        std::shared_ptr<IDataService> m_dataService;

        // 命令
        std::shared_ptr<RelayCommand> m_topicSelectedCommand;
        std::shared_ptr<AsyncRelayCommand> m_refreshCommand;
        std::shared_ptr<RelayCommand> m_clearFiltersCommand;

        // 事件
        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::hstring>> m_topicNavigationRequested;

        // 私有方法
        winrt::Windows::Foundation::IAsyncAction LoadTopicsAsync();
        winrt::Windows::Foundation::IAsyncAction LoadCategoriesAsync();
        winrt::fire_and_forget FilterTopicsAsync();
        void InitializeCommands();
        void UpdateCounts();

        // 包装器类用于绑定复杂对象
        struct TopicWrapper
        {
            LearningTopic Topic;
            winrt::hstring DisplayTitle;
            winrt::hstring DisplayDescription;
            winrt::hstring DisplayCategory;
            winrt::hstring DisplayIcon;

            TopicWrapper(LearningTopic const& topic)
                : Topic(topic)
                , DisplayTitle(topic.Title)
                , DisplayDescription(topic.Description)
                , DisplayCategory(topic.Category)
                , DisplayIcon(topic.Icon)
            {
            }
        };

        // 命令实现
        void OnTopicSelected(winrt::Windows::Foundation::IInspectable const& parameter);
        winrt::Windows::Foundation::IAsyncAction OnRefreshAsync(winrt::Windows::Foundation::IInspectable const& parameter);
        void OnClearFilters(winrt::Windows::Foundation::IInspectable const& parameter);
    };
}