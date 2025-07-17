#pragma once

#include "FundamentalsPage.g.h"

namespace winrt::MicrosoftDocsGallery::implementation
{
    struct FundamentalsPage : FundamentalsPageT<FundamentalsPage>
    {
        FundamentalsPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

    };
}

namespace winrt::MicrosoftDocsGallery::factory_implementation
{
    struct FundamentalsPage : FundamentalsPageT<FundamentalsPage, implementation::FundamentalsPage>
    {
    };
}
