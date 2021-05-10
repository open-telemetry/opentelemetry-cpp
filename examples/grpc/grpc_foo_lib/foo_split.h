#pragma once
#include <sstream>
#include <string>
#include <vector>
#include "../tracer_common.h"
#include "grpc_map_carrier.h"
#include "opentelemetry/trace/provider.h"

std::vector<std::string> split(std::string s, char delim);
