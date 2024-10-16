

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
namespace artifact
{

/**
 * The provenance filename of the built attestation which directly relates to the build artifact
 * filename. This filename SHOULD accompany the artifact at publish time. See the <a
 * href="https://slsa.dev/spec/v1.0/distributing-provenance#relationship-between-artifacts-and-attestations">SLSA
 * Relationship</a> specification for more information.
 */
static const char *kArtifactAttestationFilename = "artifact.attestation.filename";

/**
 * The full <a href="https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-5.pdf">hash value (see
 * glossary)</a>, of the built attestation. Some envelopes in the software attestation space also
 * refer to this as the <a
 * href="https://github.com/in-toto/attestation/blob/main/spec/README.md#in-toto-attestation-framework-spec">digest</a>.
 */
static const char *kArtifactAttestationHash = "artifact.attestation.hash";

/**
 * The id of the build <a href="https://slsa.dev/attestation-model">software attestation</a>.
 */
static const char *kArtifactAttestationId = "artifact.attestation.id";

/**
 * The human readable file name of the artifact, typically generated during build and release
 * processes. Often includes the package name and version in the file name. Note: This file name can
 * also act as the <a href="https://slsa.dev/spec/v1.0/terminology#package-model">Package Name</a>
 * in cases where the package ecosystem maps accordingly.
 * Additionally, the artifact <a
 * href="https://slsa.dev/spec/v1.0/terminology#software-supply-chain">can be published</a> for
 * others, but that is not a guarantee.
 */
static const char *kArtifactFilename = "artifact.filename";

/**
 * The full <a href="https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-5.pdf">hash value (see
 * glossary)</a>, often found in checksum.txt on a release of the artifact and used to verify
 * package integrity. Note: The specific algorithm used to create the cryptographic hash value is
 * not defined. In situations where an artifact has multiple
 * cryptographic hashes, it is up to the implementer to choose which
 * hash value to set here; this should be the most secure hash algorithm
 * that is suitable for the situation and consistent with the
 * corresponding attestation. The implementer can then provide the other
 * hash values through an additional set of attribute extensions as they
 * deem necessary.
 */
static const char *kArtifactHash = "artifact.hash";

/**
 * The <a href="https://github.com/package-url/purl-spec">Package URL</a> of the <a
 * href="https://slsa.dev/spec/v1.0/terminology#package-model">package artifact</a> provides a
 * standard way to identify and locate the packaged artifact.
 */
static const char *kArtifactPurl = "artifact.purl";

/**
 * The version of the artifact.
 */
static const char *kArtifactVersion = "artifact.version";

}  // namespace artifact
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
