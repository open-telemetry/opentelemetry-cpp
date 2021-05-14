// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

const char tracez_index[] =
    ""
    "<!doctype html>"
    "<html>"
    "  <head>"
    "    <title>zPages TraceZ</title>"
    "    <script src='/tracez/script.js'></script>"
    "    <link href='/tracez/style.css' rel='stylesheet'>"
    "  </head>"
    "  <body>"
    "    <h1>zPages TraceZ</h1>"
    "    <span  id='top-right'>Data last fetched: <span id='lastUpdateTime'></span><br>"
    "    <button onclick='refreshData()'>Fetch new data</button></span>"
    "    <br><br>"
    "    <div class='table-wrap'>"
    "      <table id='headers'>"
    "        <colgroup>"
    "          <col class='md'>"
    "          <col class='sm'>"
    "          <col class='sm'>"
    "          <col class='lg'>"
    "        </colgroup>"
    "        <tr>"
    "          <th>Span Name</th>"
    "          <th>Error Samples</th>"
    "          <th>Running</th>"
    "          <th>Latency Samples</th>"
    "        </tr>"
    "      </table>"
    "      <table id='overview_table'>"
    "      </table>"
    "      <div class='right'>Row count: <span id='overview_table_count'>0</span></div>"
    "    </div>"
    "    <br>"
    "    <br>"
    "    <span id='name_type_detail_table_header'></span>"
    "    <div class='table-wrap'>"
    "      <table id='name_type_detail_table'>"
    "      </table>"
    "      <div class='right'>Row count: <span id='name_type_detail_table_count'>0</span></div>"
    "    </div>"
    "  </body>"
    "</html>";
