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
 * Deprecated, use Event API to report completions contents.
 * <p>
 * @deprecated
 * Removed, no replacement at this time.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kGenAiCompletion = "gen_ai.completion";

/**
 * The response format that is requested.
 */
static constexpr const char *kGenAiOpenaiRequestResponseFormat =
    "gen_ai.openai.request.response_format";

/**
 * Requests with same seed value more likely to return same result.
 */
static constexpr const char *kGenAiOpenaiRequestSeed = "gen_ai.openai.request.seed";

/**
 * The service tier requested. May be a specific tier, detault, or auto.
 */
static constexpr const char *kGenAiOpenaiRequestServiceTier = "gen_ai.openai.request.service_tier";

/**
 * The service tier used for the response.
 */
static constexpr const char *kGenAiOpenaiResponseServiceTier =
    "gen_ai.openai.response.service_tier";

/**
 * The name of the operation being performed.
 * <p>
 * If one of the predefined values applies, but specific system uses a different name it's
 * RECOMMENDED to document it in the semantic conventions for specific GenAI system and use
 * system-specific name in the instrumentation. If a different name is not documented,
 * instrumentation libraries SHOULD use applicable predefined value.
 */
static constexpr const char *kGenAiOperationName = "gen_ai.operation.name";

/**
 * Deprecated, use Event API to report prompt contents.
 * <p>
 * @deprecated
 * Removed, no replacement at this time.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kGenAiPrompt = "gen_ai.prompt";

/**
 * The frequency penalty setting for the GenAI request.
 */
static constexpr const char *kGenAiRequestFrequencyPenalty = "gen_ai.request.frequency_penalty";

/**
 * The maximum number of tokens the model generates for a request.
 */
static constexpr const char *kGenAiRequestMaxTokens = "gen_ai.request.max_tokens";

/**
 * The name of the GenAI model a request is being made to.
 */
static constexpr const char *kGenAiRequestModel = "gen_ai.request.model";

/**
 * The presence penalty setting for the GenAI request.
 */
static constexpr const char *kGenAiRequestPresencePenalty = "gen_ai.request.presence_penalty";

/**
 * List of sequences that the model will use to stop generating further tokens.
 */
static constexpr const char *kGenAiRequestStopSequences = "gen_ai.request.stop_sequences";

/**
 * The temperature setting for the GenAI request.
 */
static constexpr const char *kGenAiRequestTemperature = "gen_ai.request.temperature";

/**
 * The top_k sampling setting for the GenAI request.
 */
static constexpr const char *kGenAiRequestTopK = "gen_ai.request.top_k";

/**
 * The top_p sampling setting for the GenAI request.
 */
static constexpr const char *kGenAiRequestTopP = "gen_ai.request.top_p";

/**
 * Array of reasons the model stopped generating tokens, corresponding to each generation received.
 */
static constexpr const char *kGenAiResponseFinishReasons = "gen_ai.response.finish_reasons";

/**
 * The unique identifier for the completion.
 */
static constexpr const char *kGenAiResponseId = "gen_ai.response.id";

/**
 * The name of the model that generated the response.
 */
static constexpr const char *kGenAiResponseModel = "gen_ai.response.model";

/**
 * The Generative AI product as identified by the client or server instrumentation.
 * <p>
 * The @code gen_ai.system @endcode describes a family of GenAI models with specific model
 * identified by @code gen_ai.request.model @endcode and @code gen_ai.response.model @endcode
 * attributes. <p> The actual GenAI product may differ from the one identified by the client. For
 * example, when using OpenAI client libraries to communicate with Mistral, the @code gen_ai.system
 * @endcode is set to @code openai @endcode based on the instrumentation's best knowledge. <p> For
 * custom model, a custom friendly name SHOULD be used. If none of these options apply, the @code
 * gen_ai.system @endcode SHOULD be set to @code _OTHER @endcode.
 */
static constexpr const char *kGenAiSystem = "gen_ai.system";

/**
 * The type of token being counted.
 */
static constexpr const char *kGenAiTokenType = "gen_ai.token.type";

/**
 * Deprecated, use @code gen_ai.usage.output_tokens @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code gen_ai.usage.output_tokens @endcode attribute.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kGenAiUsageCompletionTokens = "gen_ai.usage.completion_tokens";

/**
 * The number of tokens used in the GenAI input (prompt).
 */
static constexpr const char *kGenAiUsageInputTokens = "gen_ai.usage.input_tokens";

/**
 * The number of tokens used in the GenAI response (completion).
 */
static constexpr const char *kGenAiUsageOutputTokens = "gen_ai.usage.output_tokens";

/**
 * Deprecated, use @code gen_ai.usage.input_tokens @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code gen_ai.usage.input_tokens @endcode attribute.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kGenAiUsagePromptTokens = "gen_ai.usage.prompt_tokens";

namespace GenAiOpenaiRequestResponseFormatValues
{
/**
 * Text response format
 */
static constexpr const char *kText = "text";

/**
 * JSON object response format
 */
static constexpr const char *kJsonObject = "json_object";

/**
 * JSON schema response format
 */
static constexpr const char *kJsonSchema = "json_schema";

}  // namespace GenAiOpenaiRequestResponseFormatValues

namespace GenAiOpenaiRequestServiceTierValues
{
/**
 * The system will utilize scale tier credits until they are exhausted.
 */
static constexpr const char *kAuto = "auto";

/**
 * The system will utilize the default scale tier.
 */
static constexpr const char *kDefault = "default";

}  // namespace GenAiOpenaiRequestServiceTierValues

namespace GenAiOperationNameValues
{
/**
 * Chat completion operation such as <a
 * href="https://platform.openai.com/docs/api-reference/chat">OpenAI Chat API</a>
 */
static constexpr const char *kChat = "chat";

/**
 * Text completions operation such as <a
 * href="https://platform.openai.com/docs/api-reference/completions">OpenAI Completions API
 * (Legacy)</a>
 */
static constexpr const char *kTextCompletion = "text_completion";

}  // namespace GenAiOperationNameValues

namespace GenAiSystemValues
{
/**
 * OpenAI
 */
static constexpr const char *kOpenai = "openai";

/**
 * Vertex AI
 */
static constexpr const char *kVertexAi = "vertex_ai";

/**
 * Anthropic
 */
static constexpr const char *kAnthropic = "anthropic";

/**
 * Cohere
 */
static constexpr const char *kCohere = "cohere";

}  // namespace GenAiSystemValues

namespace GenAiTokenTypeValues
{
/**
 * Input tokens (prompt, input, etc.)
 */
static constexpr const char *kInput = "input";

/**
 * Output tokens (completion, response, etc.)
 */
static constexpr const char *kCompletion = "output";

}  // namespace GenAiTokenTypeValues

}  // namespace gen_ai
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
