#include "pch.h"
#include "FavoritesPage.xaml.h"
#if __has_include("FavoritesPage.g.cpp")
#include "FavoritesPage.g.cpp"
#endif
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Data.Json.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Data::Json;

namespace winrt::MicrosoftDocsGallery::implementation
{
    FavoritesPage::FavoritesPage()
    {
        InitializeComponent();
        
        // 初始化收藏夹集合
        m_favoritesCollection = single_threaded_observable_vector<IInspectable>();
        
        // 加载收藏夹数据
        LoadFavorites();
        
        // 初始显示
        UpdateFavoritesDisplay();
    }

    void FavoritesPage::AddToFavorites(hstring const& url, hstring const& title)
    {
        if (!IsInFavorites(url))
        {
            auto category = DetermineCategoryFromUrl(url);
            FavoriteItem item(title, url, category);
            m_allFavorites.push_back(item);
            
            SaveFavorites();
            FilterAndSortFavorites();
            UpdateFavoritesDisplay();
        }
    }

    void FavoritesPage::RemoveFromFavorites(hstring const& url)
    {
        auto it = std::find_if(m_allFavorites.begin(), m_allFavorites.end(),
            [&url](const FavoriteItem& item) { return item.Url == url; });
        
        if (it != m_allFavorites.end())
        {
            m_allFavorites.erase(it);
            SaveFavorites();
            FilterAndSortFavorites();
            UpdateFavoritesDisplay();
        }
    }

    void FavoritesPage::ClearAllFavorites(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        m_allFavorites.clear();
        SaveFavorites();
        UpdateFavoritesDisplay();
    }

    bool FavoritesPage::IsInFavorites(hstring const& url)
    {
        return std::find_if(m_allFavorites.begin(), m_allFavorites.end(),
            [&url](const FavoriteItem& item) { return item.Url == url; }) != m_allFavorites.end();
    }

    void FavoritesPage::FavoriteItem_Click(IInspectable const& sender, RoutedEventArgs const& /*e*/)
    {
        auto button = sender.try_as<Button>();
        if (button)
        {
            auto url = button.Tag().try_as<hstring>();
            if (url)
            {
                // 在WinUI 3中，通过页面的Parent层次查找MainWindow
                auto parent = this->Parent();
                while (parent)
                {
                    // 尝试向上查找MainWindow
                    auto mainWindow = parent.try_as<MicrosoftDocsGallery::MainWindow>();
                    if (mainWindow)
                    {
                        mainWindow.openWebViewPage(url.value());
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
    }

    void FavoritesPage::RemoveFavorite_Click(IInspectable const& sender, RoutedEventArgs const& /*e*/)
    {
        auto button = sender.try_as<Button>();
        if (button)
        {
            auto url = button.Tag().try_as<hstring>();
            if (url)
            {
                RemoveFromFavorites(url.value());
            }
        }
    }

    void FavoritesPage::SearchBox_TextChanged(IInspectable const& sender, TextChangedEventArgs const& /*e*/)
    {
        auto textBox = sender.try_as<TextBox>();
        if (textBox)
        {
            m_searchText = textBox.Text();
            FilterAndSortFavorites();
            UpdateFavoritesDisplay();
        }
    }

    void FavoritesPage::CategoryFilter_SelectionChanged(IInspectable const& sender, SelectionChangedEventArgs const& /*e*/)
    {
        auto comboBox = sender.try_as<ComboBox>();
        if (comboBox && comboBox.SelectedItem())
        {
            auto selectedItem = comboBox.SelectedItem().try_as<ComboBoxItem>();
            if (selectedItem)
            {
                m_selectedCategory = selectedItem.Tag().try_as<hstring>().value_or(L"All");
                FilterAndSortFavorites();
                UpdateFavoritesDisplay();
            }
        }
    }

    void FavoritesPage::SortBy_SelectionChanged(IInspectable const& sender, SelectionChangedEventArgs const& /*e*/)
    {
        auto comboBox = sender.try_as<ComboBox>();
        if (comboBox && comboBox.SelectedItem())
        {
            auto selectedItem = comboBox.SelectedItem().try_as<ComboBoxItem>();
            if (selectedItem)
            {
                m_sortBy = selectedItem.Tag().try_as<hstring>().value_or(L"DateAdded");
                FilterAndSortFavorites();
                UpdateFavoritesDisplay();
            }
        }
    }

    void FavoritesPage::BrowseDocs_Click(IInspectable const& sender, RoutedEventArgs const& /*e*/)
    {
        // 在WinUI 3中，通过页面的Parent层次查找MainWindow
        auto button = sender.try_as<Button>();
        if (button)
        {
            auto parent = this->Parent();
            while (parent)
            {
                // 尝试向上查找MainWindow
                auto mainWindow = parent.try_as<MicrosoftDocsGallery::MainWindow>();
                if (mainWindow)
                {
                    mainWindow.openFundamentalsPage();
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

    void FavoritesPage::LoadFavorites()
    {
        try
        {
            auto localSettings = ApplicationData::Current().LocalSettings();
            auto values = localSettings.Values();
            
            if (values.HasKey(L"Favorites"))
            {
                auto favoritesJson = values.Lookup(L"Favorites").try_as<hstring>();
                if (favoritesJson)
                {
                    JsonArray jsonArray;
                    if (JsonArray::TryParse(favoritesJson.value(), jsonArray))
                    {
                        m_allFavorites.clear();
                        for (uint32_t i = 0; i < jsonArray.Size(); ++i)
                        {
                            auto item = jsonArray.GetAt(i);
                            if (item.ValueType() == JsonValueType::Object)
                            {
                                auto obj = item.GetObject();
                                FavoriteItem fav;
                                fav.Title = obj.GetNamedString(L"title", L"");
                                fav.Url = obj.GetNamedString(L"url", L"");
                                fav.Category = obj.GetNamedString(L"category", L"Documentation");
                                fav.DateAdded = obj.GetNamedString(L"dateAdded", L"");
                                
                                if (!fav.Title.empty() && !fav.Url.empty())
                                {
                                    m_allFavorites.push_back(fav);
                                }
                            }
                        }
                    }
                }
            }
            
            // 如果没有收藏夹，添加一些示例数据
            if (m_allFavorites.empty())
            {
                m_allFavorites.emplace_back(L"WinUI 3 入门指南", L"https://learn.microsoft.com/windows/apps/winui/", L"Tutorial");
                m_allFavorites.emplace_back(L"C# 编程指南", L"https://learn.microsoft.com/dotnet/csharp/", L"Documentation");
                m_allFavorites.emplace_back(L"Azure 快速开始", L"https://learn.microsoft.com/azure/", L"Tutorial");
            }
        }
        catch (...)
        {
            // 加载失败时使用默认数据
            m_allFavorites.clear();
            m_allFavorites.emplace_back(L"WinUI 3 入门指南", L"https://learn.microsoft.com/windows/apps/winui/", L"Tutorial");
        }
        
        FilterAndSortFavorites();
    }

    void FavoritesPage::SaveFavorites()
    {
        try
        {
            JsonArray jsonArray;
            
            for (const auto& fav : m_allFavorites)
            {
                JsonObject obj;
                obj.SetNamedValue(L"title", JsonValue::CreateStringValue(fav.Title));
                obj.SetNamedValue(L"url", JsonValue::CreateStringValue(fav.Url));
                obj.SetNamedValue(L"category", JsonValue::CreateStringValue(fav.Category));
                obj.SetNamedValue(L"dateAdded", JsonValue::CreateStringValue(fav.DateAdded));
                jsonArray.Append(obj);
            }
            
            auto localSettings = ApplicationData::Current().LocalSettings();
            auto values = localSettings.Values();
            values.Insert(L"Favorites", box_value(jsonArray.Stringify()));
        }
        catch (...)
        {
            // 忽略保存错误
        }
    }

    void FavoritesPage::UpdateFavoritesDisplay()
    {
        // 更新清空按钮状态
        ClearAllButton().IsEnabled(!m_allFavorites.empty());
        
        if (m_filteredFavorites.empty())
        {
            // 显示空状态
            EmptyStatePanel().Visibility(Visibility::Visible);
            FavoritesListView().Visibility(Visibility::Collapsed);
        }
        else
        {
            // 显示收藏夹列表
            EmptyStatePanel().Visibility(Visibility::Collapsed);
            FavoritesListView().Visibility(Visibility::Visible);
            
            // 更新列表数据
            m_favoritesCollection.Clear();
            for (const auto& fav : m_filteredFavorites)
            {
                // 创建数据对象（简化版本，实际应该使用proper数据绑定）
                auto dataObj = single_threaded_map<hstring, IInspectable>();
                dataObj.Insert(L"Title", box_value(fav.Title));
                dataObj.Insert(L"Url", box_value(fav.Url));
                dataObj.Insert(L"Category", box_value(fav.Category));
                dataObj.Insert(L"DateAdded", box_value(fav.DateAdded));
                
                m_favoritesCollection.Append(dataObj.as<IInspectable>());
            }
            
            FavoritesListView().ItemsSource(m_favoritesCollection);
        }
    }

    void FavoritesPage::FilterAndSortFavorites()
    {
        m_filteredFavorites.clear();
        
        // 过滤
        for (auto const& fav : m_allFavorites)
        {
            bool categoryMatch = (m_selectedCategory == L"All" || fav.Category == m_selectedCategory);
            
            bool textMatch = m_searchText.empty();
            if (!textMatch)
            {
                auto searchStr = m_searchText.c_str();
                auto titleStr = fav.Title.c_str();
                
                // 使用简单的包含检查
                textMatch = wcsstr(titleStr, searchStr) != nullptr;
            }
            
            if (categoryMatch && textMatch)
            {
                m_filteredFavorites.push_back(fav);
            }
        }
        
        // 排序 - 使用简化的排序逻辑
        if (m_sortBy == L"Title")
        {
            // 简单的冒泡排序，避免使用std::sort
            for (size_t i = 0; i < m_filteredFavorites.size(); ++i)
            {
                for (size_t j = i + 1; j < m_filteredFavorites.size(); ++j)
                {
                    if (m_filteredFavorites[i].Title > m_filteredFavorites[j].Title)
                    {
                        auto temp = m_filteredFavorites[i];
                        m_filteredFavorites[i] = m_filteredFavorites[j];
                        m_filteredFavorites[j] = temp;
                    }
                }
            }
        }
        else if (m_sortBy == L"Category")
        {
            for (size_t i = 0; i < m_filteredFavorites.size(); ++i)
            {
                for (size_t j = i + 1; j < m_filteredFavorites.size(); ++j)
                {
                    if (m_filteredFavorites[i].Category > m_filteredFavorites[j].Category)
                    {
                        auto temp = m_filteredFavorites[i];
                        m_filteredFavorites[i] = m_filteredFavorites[j];
                        m_filteredFavorites[j] = temp;
                    }
                }
            }
        }
        else // DateAdded - 默认按添加时间倒序
        {
            for (size_t i = 0; i < m_filteredFavorites.size(); ++i)
            {
                for (size_t j = i + 1; j < m_filteredFavorites.size(); ++j)
                {
                    if (m_filteredFavorites[i].DateAdded < m_filteredFavorites[j].DateAdded)
                    {
                        auto temp = m_filteredFavorites[i];
                        m_filteredFavorites[i] = m_filteredFavorites[j];
                        m_filteredFavorites[j] = temp;
                    }
                }
            }
        }
    }

    hstring FavoritesPage::DetermineCategoryFromUrl(hstring const& url)
    {
        auto urlStr = url.c_str();
        
        // 使用简单的字符串查找，避免std::transform
        if (wcsstr(urlStr, L"tutorial") != nullptr || 
            wcsstr(urlStr, L"quickstart") != nullptr ||
            wcsstr(urlStr, L"get-started") != nullptr)
        {
            return L"Tutorial";
        }
        else if (wcsstr(urlStr, L"api") != nullptr || 
                 wcsstr(urlStr, L"reference") != nullptr)
        {
            return L"API";
        }
        else if (wcsstr(urlStr, L"sample") != nullptr || 
                 wcsstr(urlStr, L"example") != nullptr)
        {
            return L"Sample";
        }
        else
        {
            return L"Documentation";
        }
    }
}