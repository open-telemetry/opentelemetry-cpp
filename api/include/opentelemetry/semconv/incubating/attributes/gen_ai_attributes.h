

/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DO NOT EDIT, this is an Auto-generated file from:
 * buildscripts/semantic-convention/templates/registry/semantic_attributes-h.j2
 */

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace semconv
{
namespace gen_ai
{

/**
 * The full response received from the GenAI model.
 * Note: It's RECOMMENDED to format completions as JSON string matching <a
 * href="https://platform.openai.com/docs/guides/text-generation">OpenAI messages format</a>.
 */
static const char *kGenAiCompletion = "gen_ai.completion";

/**
 * The name of the operation being performed.
 * Note: If one of the predefined values applies, but specific system uses a different name it's
 * RECOMMENDED to document it in the semantic conventions for specific GenAI system and use
 * system-specific name in the instrumentation. If a different name is not documented,
 * instrumentation libraries SHOULD use applicable predefined value.
 */
static const char *kGenAiOperationName = "gen_ai.operation.name";

/**
 * The full prompt sent to the GenAI model.
 * Note: It's RECOMMENDED to format prompts as JSON string matching <a
 * href="https://platform.openai.com/docs/guides/text-generation">OpenAI messages format</a>.
 */
static const char *kGenAiPrompt = "gen_ai.prompt";

/**
 * The frequency penalty setting for the GenAI request.
 */
static const char *kGenAiRequestFrequencyPenalty = "gen_ai.request.frequency_penalty";

/**
 * The maximum number of tokens the model generates for a request.
 */
static const char *kGenAiRequestMaxTokens = "gen_ai.request.max_tokens";

/**
 * The name of the GenAI model a request is being made to.
 */
static const char *kGenAiRequestModel = "gen_ai.request.model";

/**
 * The presence penalty setting for the GenAI request.
 */
static const char *kGenAiRequestPresencePenalty = "gen_ai.request.presence_penalty";

/**
 * List of sequences that the model will use to stop generating further tokens.
 */
static const char *kGenAiRequestStopSequences = "gen_ai.request.stop_sequences";

/**
 * The temperature setting for the GenAI request.
 */
static const char *kGenAiRequestTemperature = "gen_ai.request.temperature";

/**
 * The top_k sampling setting for the GenAI request.
 */
static const char *kGenAiRequestTopK = "gen_ai.request.top_k";

/**
 * The top_p sampling setting for the GenAI request.
 */
static const char *kGenAiRequestTopP = "gen_ai.request.top_p";

/**
 * Array of reasons the model stopped generating tokens, corresponding to each generation received.
 */
static const char *kGenAiResponseFinishReasons = "gen_ai.response.finish_reasons";

/**
 * The unique identifier for the completion.
 */
static const char *kGenAiResponseId = "gen_ai.response.id";

/**
 * The name of the model that generated the response.
 */
static const char *kGenAiResponseModel = "gen_ai.response.model";

/**
 * The Generative AI product as identified by the client or server instrumentation.
 * Note: The @code gen_ai.system @endcode describes a family of GenAI models with specific model
 * identified by @code gen_ai.request.model @endcode and @code gen_ai.response.model @endcode
 * attributes. <p> The actual GenAI product may differ from the one identified by the client. For
 * example, when using OpenAI client libraries to communicate with Mistral, the @code gen_ai.system
 * @endcode is set to @code openai @endcode based on the instrumentation's best knowledge. <p> For
 * custom model, a custom friendly name SHOULD be used. If none of these options apply, the @code
 * gen_ai.system @endcode SHOULD be set to @code _OTHER @endcode.
 */
static const char *kGenAiSystem = "gen_ai.system";

/**
 * The type of token being counted.
 */
static const char *kGenAiTokenType = "gen_ai.token.type";

/**
 * @Deprecated: Replaced by @code gen_ai.usage.output_tokens @endcode attribute.
 */
static const char *kGenAiUsageCompletionTokens = "gen_ai.usage.completion_tokens";

/**
 * The number of tokens used in the GenAI input (prompt).
 */
static const char *kGenAiUsageInputTokens = "gen_ai.usage.input_tokens";

/**
 * The number of tokens used in the GenAI response (completion).
 */
static const char *kGenAiUsageOutputTokens = "gen_ai.usage.output_tokens";

/**
 * @Deprecated: Replaced by @code gen_ai.usage.input_tokens @endcode attribute.
 */
static const char *kGenAiUsagePromptTokens = "gen_ai.usage.prompt_tokens";

// DEBUG: {"brief": "The name of the operation being performed.", "name": "gen_ai.operation.name",
// "note": "If one of the predefined values applies, but specific system uses a different name it's
// RECOMMENDED to document it in the semantic conventions for specific GenAI system and use
// system-specific name in the instrumentation. If a different name is not documented,
// instrumentation libraries SHOULD use applicable predefined value.\n", "requirement_level":
// "recommended", "root_namespace": "gen_ai", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": "Chat completion operation such as [OpenAI
// Chat API](https://platform.openai.com/docs/api-reference/chat)", "deprecated": none, "id":
// "chat", "note": none, "stability": "experimental", "value": "chat"}, {"brief": "Text completions
// operation such as [OpenAI Completions API
// (Legacy)](https://platform.openai.com/docs/api-reference/completions)", "deprecated": none, "id":
// "text_completion", "note": none, "stability": "experimental", "value": "text_completion"}]}}
namespace GenAiOperationNameValues
{
/**
 * Chat completion operation such as <a
 * href="https://platform.openai.com/docs/api-reference/chat">OpenAI Chat API</a>.
 */
// DEBUG: {"brief": "Chat completion operation such as [OpenAI Chat
// API](https://platform.openai.com/docs/api-reference/chat)", "deprecated": none, "id": "chat",
// "note": none, "stability": "experimental", "value": "chat"}
static constexpr const char *kChat = "chat";
/**
 * Text completions operation such as <a
 * href="https://platform.openai.com/docs/api-reference/completions">OpenAI Completions API
 * (Legacy)</a>.
 */
// DEBUG: {"brief": "Text completions operation such as [OpenAI Completions API
// (Legacy)](https://platform.openai.com/docs/api-reference/completions)", "deprecated": none, "id":
// "text_completion", "note": none, "stability": "experimental", "value": "text_completion"}
static constexpr const char *kTextCompletion = "text_completion";
}  // namespace GenAiOperationNameValues

// DEBUG: {"brief": "The Generative AI product as identified by the client or server
// instrumentation.", "examples": "openai", "name": "gen_ai.system", "note": "The `gen_ai.system`
// describes a family of GenAI models with specific model identified\nby `gen_ai.request.model` and
// `gen_ai.response.model` attributes.\n\nThe actual GenAI product may differ from the one
// identified by the client.\nFor example, when using OpenAI client libraries to communicate with
// Mistral, the `gen_ai.system`\nis set to `openai` based on the instrumentation's best
// knowledge.\n\nFor custom model, a custom friendly name SHOULD be used.\nIf none of these options
// apply, the `gen_ai.system` SHOULD be set to `_OTHER`.\n", "requirement_level": "recommended",
// "root_namespace": "gen_ai", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": "OpenAI", "deprecated": none, "id": "openai", "note": none, "stability":
// "experimental", "value": "openai"}, {"brief": "Vertex AI", "deprecated": none, "id": "vertex_ai",
// "note": none, "stability": "experimental", "value": "vertex_ai"}, {"brief": "Anthropic",
// "deprecated": none, "id": "anthropic", "note": none, "stability": "experimental", "value":
// "anthropic"}, {"brief": "Cohere", "deprecated": none, "id": "cohere", "note": none, "stability":
// "experimental", "value": "cohere"}]}}
namespace GenAiSystemValues
{
/**
 * OpenAI.
 */
// DEBUG: {"brief": "OpenAI", "deprecated": none, "id": "openai", "note": none, "stability":
// "experimental", "value": "openai"}
static constexpr const char *kOpenai = "openai";
/**
 * Vertex AI.
 */
// DEBUG: {"brief": "Vertex AI", "deprecated": none, "id": "vertex_ai", "note": none, "stability":
// "experimental", "value": "vertex_ai"}
static constexpr const char *kVertexAi = "vertex_ai";
/**
 * Anthropic.
 */
// DEBUG: {"brief": "Anthropic", "deprecated": none, "id": "anthropic", "note": none, "stability":
// "experimental", "value": "anthropic"}
static constexpr const char *kAnthropic = "anthropic";
/**
 * Cohere.
 */
// DEBUG: {"brief": "Cohere", "deprecated": none, "id": "cohere", "note": none, "stability":
// "experimental", "value": "cohere"}
static constexpr const char *kCohere = "cohere";
}  // namespace GenAiSystemValues

// DEBUG: {"brief": "The type of token being counted.", "examples": ["input", "output"], "name":
// "gen_ai.token.type", "requirement_level": "recommended", "root_namespace": "gen_ai", "stability":
// "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "Input tokens
// (prompt, input, etc.)", "deprecated": none, "id": "input", "note": none, "stability":
// "experimental", "value": "input"}, {"brief": "Output tokens (completion, response, etc.)",
// "deprecated": none, "id": "completion", "note": none, "stability": "experimental", "value":
// "output"}]}}
namespace GenAiTokenTypeValues
{
/**
 * Input tokens (prompt, input, etc.).
 */
// DEBUG: {"brief": "Input tokens (prompt, input, etc.)", "deprecated": none, "id": "input", "note":
// none, "stability": "experimental", "value": "input"}
static constexpr const char *kInput = "input";
/**
 * Output tokens (completion, response, etc.).
 */
// DEBUG: {"brief": "Output tokens (completion, response, etc.)", "deprecated": none, "id":
// "completion", "note": none, "stability": "experimental", "value": "output"}
static constexpr const char *kCompletion = "output";
}  // namespace GenAiTokenTypeValues

}  // namespace gen_ai
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
