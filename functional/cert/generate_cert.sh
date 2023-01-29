#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

which cfssl
which cfssljson

cfssl version
cfssljson -version

cfssl genkey -initca ca_csr.json | cfssljson -bare ca
cfssl gencert -ca ca.pem -ca-key ca-key.pem client_csr.json | cfssljson -bare client_cert
cfssl gencert -ca ca.pem -ca-key ca-key.pem server_csr.json | cfssljson -bare server_cert

cfssl genkey -initca ca_csr_b.json | cfssljson -bare ca
cfssl gencert -ca ca_b.pem -ca-key ca-key_b.pem client_csr_b.json | cfssljson -bare client_cert_b
cfssl gencert -ca ca_b.pem -ca-key ca-key_b.pem server_csr_b.json | cfssljson -bare server_cert_b

echo "UNREADABLE" > unreadable.pem
chmod 0 unreadable.pem

ls -l

