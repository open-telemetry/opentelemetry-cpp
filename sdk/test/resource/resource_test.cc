#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/sdk/trace/attribute_utils.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/string_view.h"

#include <map>
#include <string>

#include <gtest/gtest.h>

TEST(ResourceTest, create)
{

    std::map<std::string, std::string> expected_attributes = {{"service","backend"},{"version", "1"},{"cost", "234.23"},{"telemetry.sdk.language","cpp"},{"telemetry.sdk.name", "opentelemetry"},{"telemetry.sdk.version", OPENTELEMETRY_SDK_VERSION}};
    auto resource = opentelemetry::sdk::resource::Resource::Create({{"service","backend"}, {"version", "1"}, {"cost", "234.23"}});
    auto received_attributes = resource->GetAttributes().GetAttributes();

    for (auto &e: received_attributes)
    {
        EXPECT_EQ(expected_attributes[e.first], opentelemetry::nostd::get<std::string>(e.second));
        //std::cout << e.first << ":" << opentelemetry::nostd::get<std::string>(e.second) << "\n";

    }
    EXPECT_EQ(received_attributes.size(), expected_attributes.size());

    std::map<opentelemetry::nostd::string_view, std::string> attributes = {{"service","backend"}, {"version", "1"}, {"cost", "234.23"}};
    auto resource2 = opentelemetry::sdk::resource::Resource::Create<std::map<opentelemetry::nostd::string_view, std::string>> (attributes);
    auto received_attributes2 = resource2->GetAttributes().GetAttributes();
    for (auto &e: received_attributes2)
    {
        EXPECT_EQ(expected_attributes[e.first], opentelemetry::nostd::get<std::string>(e.second));
        //std::cout << e.first << ":" << opentelemetry::nostd::get<std::string>(e.second) << "\n";

    }

    //opentelemetry::sdk::resource::Resource resource5(attributes);
}