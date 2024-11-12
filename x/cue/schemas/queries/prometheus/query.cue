// Copyright 2023 The Perses Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package model

import (
	"strings"
	prometheus "github.com/perses/perses/cue/schemas/datasources/prometheus:model"
)

kind: "PrometheusTimeSeriesQuery"
spec: close({
	datasource?: {
		kind: prometheus.kind
	}
	query:             strings.MinRunes(1)
	seriesNameFormat?: string
	minStep?:          =~"^(?:(\\d+)y)?(?:(\\d+)w)?(?:(\\d+)d)?(?:(\\d+)h)?(?:(\\d+)m)?(?:(\\d+)s)?(?:(\\d+)ms)?(?:\\$\\w+)?$"
	resolution?:       number
})
