#pragma once

#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace internal
{

/** Maps from C++ attribute into OTLP proto attribute. */
void PopulateAttribute(opentelemetry::proto::common::v1::KeyValue *attribute,
                       nostd::string_view key,
                       const opentelemetry::common::AttributeValue &value);

/** Maps from C++ attribute into OTLP proto attribute. */
void PopulateAttribute(opentelemetry::proto::common::v1::KeyValue *attribute,
                       nostd::string_view key,
                       const sdk::common::OwnedAttributeValue &value);

}  // namespace internal
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE