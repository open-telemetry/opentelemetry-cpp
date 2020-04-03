#pragma once

#ifndef __has_include
#  define __has_include(x) 0
#endif

#if !defined(__GLIBCXX__) || __has_include(<codecvt>)  // >= libstdc++-5
#  define OPENTELEMETRY_TRIVIALITY_TYPE_TRAITS
#endif
