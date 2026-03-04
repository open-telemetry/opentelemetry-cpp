#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

generate_ca() {
  local name=$1
  local org=$2

  openssl genrsa -out ${name}-key.pem 2048

  openssl req -x509 -new -nodes \
    -key ${name}-key.pem \
    -sha256 -days 365 \
    -subj "/O=${org}/CN=${name}" \
    -out ${name}.pem
}

generate_cert() {
  local ca_name=$1
  local cert_name=$2
  local org=$3

  openssl genrsa -out ${cert_name}-key.pem 2048

  cat > ${cert_name}_ext.cnf <<EOF
[req]
distinguished_name=req_distinguished_name
req_extensions=req_ext
prompt=no

[req_distinguished_name]
O=${org}
CN=localhost

[req_ext]
subjectAltName=@alt_names

[alt_names]
DNS.1=localhost
IP.1=127.0.0.1
EOF

  openssl req -new \
    -key ${cert_name}-key.pem \
    -out ${cert_name}.csr \
    -config ${cert_name}_ext.cnf

  openssl x509 -req \
    -in ${cert_name}.csr \
    -CA ${ca_name}.pem \
    -CAkey ${ca_name}-key.pem \
    -CAcreateserial \
    -out ${cert_name}.pem \
    -days 365 -sha256 \
    -extensions req_ext \
    -extfile ${cert_name}_ext.cnf

  rm -f ${cert_name}.csr ${cert_name}_ext.cnf
}

echo "Generating CA"
generate_ca ca "OpenTelemetry CA Example"

echo "Generating CLIENT CERT"
generate_cert ca client_cert "OpenTelemetry Client Example"

echo "Generating SERVER CERT"
generate_cert ca server_cert "OpenTelemetry Server Example"

echo "Generating CA B"
generate_ca ca_b "OpenTelemetry CA B Example"

echo "Generating CLIENT CERT B"
generate_cert ca_b client_cert_b "OpenTelemetry Client B Example"

echo "Generating SERVER CERT B"
generate_cert ca_b server_cert_b "OpenTelemetry Server B Example"

echo "UNREADABLE" > unreadable.pem
chmod 0 unreadable.pem

# Needed to copy this key inside docker (different owner)
chmod +r server_cert-key.pem

# Debug
ls -l

