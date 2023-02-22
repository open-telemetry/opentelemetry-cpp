# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

docker run -p 9090:9090 -v $(pwd):/etc/prometheus --network="host" prom/prometheus
