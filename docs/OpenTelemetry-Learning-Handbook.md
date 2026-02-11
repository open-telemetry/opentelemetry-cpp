# OpenTelemetry End-to-End Learning Handbook

**Purpose:** A step-by-step handbook so you can learn OpenTelemetry from zero—no prior Docker, Kubernetes, Git, or OpenTelemetry knowledge required. No paid licenses. Every step has exact values and commands; you can follow it without asking for clarification.

**Your machine:** Windows 10/11. If you use another OS, the only differences are in the “Install on Windows” sections; the rest (Git commands, Docker Compose, demo URLs, data model) is the same.

---

## Table of Contents

1. [Part 0: Glossary](#part-0-glossary)
2. [Part 1: Install Prerequisites (Windows)](#part-1-install-prerequisites-windows)
3. [Part 2: GitHub Basics (No Account Required for Demo)](#part-2-github-basics-no-account-required-for-demo)
4. [Part 3: Run the OpenTelemetry Demo (Default Backend, No Token)](#part-3-run-the-opentelemetry-demo-default-backend-no-token)
5. [Part 4: Optional—Send Data to Your Own Backend (Tokens)](#part-4-optional-send-data-to-your-own-backend-tokens)
6. [Part 5: Verification and Where to Look](#part-5-verification-and-where-to-look)
7. [Part 6: OpenTelemetry Data Model—Fields and Values](#part-6-opentelemetry-data-modelfields-and-values)
8. [Part 7: Demo Scenarios (Hands-On Problem-Solving)](#part-7-demo-scenarios-hands-on-problem-solving)
9. [Part 8: Next Steps Toward Expertise](#part-8-next-steps-toward-expertise)

---

## Part 0: Glossary

Use this when you see a term you don’t know. Every term below is used in this handbook or in the OpenTelemetry Demo.

| Term | Meaning in one sentence |
|------|-------------------------|
| **Docker** | A tool that runs applications in isolated “containers” so they behave the same on your PC and on servers. |
| **Docker Compose** | A tool that starts and connects many containers at once using a file (e.g. `docker-compose.yaml`). |
| **Container** | A running instance of an image; like a lightweight virtual machine for one app. |
| **Image** | A packaged application (and its dependencies) that Docker uses to create containers. |
| **Git** | A version-control tool that tracks changes in files and lets you clone (download) projects from the internet. |
| **GitHub** | A website that hosts Git repositories. “Clone from GitHub” means “download this project from GitHub.” |
| **Repository (repo)** | A project folder (and its history) stored on GitHub or on your PC. |
| **Clone** | Download a copy of a repository to your computer. |
| **Secret / API key / Token** | A string that proves identity or permission (e.g. to send data to a vendor). You never commit real secrets into Git. |
| **OpenTelemetry (OTel)** | An open standard and set of tools for generating, sending, and storing **traces**, **metrics**, and **logs**. |
| **Trace** | A record of one request as it flows through multiple services; made of **spans**. |
| **Span** | One unit of work in a trace (e.g. “handle HTTP request,” “call database”). |
| **Metric** | A measured value over time (e.g. request count, CPU %, memory bytes). |
| **Log** | A timestamped message or structured event (e.g. error, info). |
| **OTLP** | OpenTelemetry Protocol—the standard way to send traces, metrics, and logs to a backend. |
| **OpenTelemetry Collector (Collector)** | A program that receives telemetry (often via OTLP), can process it, and sends it to one or more backends. |
| **Backend** | Where telemetry is stored and viewed (e.g. Jaeger, Grafana, a vendor). |
| **Jaeger** | Open-source UI to view traces. The demo runs it for you; no license. |
| **Grafana** | Open-source UI to view dashboards and metrics. The demo runs it; no license. |
| **Kubernetes (K8s)** | A system to run and manage containers at scale. Not required for this handbook. |
| **Helm** | A package manager for Kubernetes (charts = packaged apps). Not required for this handbook. |
| **Terraform** | A tool to define and create cloud/infrastructure in code. Not required for this handbook. |
| **Ansible** | A tool to automate configuration and deployment on servers. Not required for this handbook. |
| **YAML** | A file format used for configuration (e.g. Collector config, Docker Compose). Indentation matters. |
| **Feature flag** | A switch (on/off or variant) that changes app behavior without redeploying; the demo uses it for “scenarios.” |
| **Load generator** | A part of the demo that sends fake user traffic to create traces and metrics. |

---

## Part 1: Install Prerequisites (Windows)

You need: **Docker** (which includes Docker Compose on Windows), and **Git**. All are free.

### 1.1 Install Docker Desktop (includes Docker + Docker Compose)

1. Go to: **https://www.docker.com/products/docker-desktop/**
2. Click **Download for Windows**.
3. Run the installer. Use default options unless you know you need something else.
4. When asked, ensure **Use WSL 2** is selected (recommended).
5. Restart the PC if prompted.
6. Open **Docker Desktop** from the Start menu. Wait until it says “Docker Desktop is running” (green).
7. **Check:** Open **PowerShell** or **Command Prompt** and run:
   ```powershell
   docker --version
   docker compose version
   ```
   You should see version numbers (e.g. `Docker version 24.x.x`, `Docker Compose version v2.x.x`). If you see “not recognized,” add Docker to your PATH or reinstall.

**Minimum:** 6 GB RAM for the app, 14 GB free disk space.

### 1.2 Install Git

1. Go to: **https://git-scm.com/download/win**
2. Download and run the installer. Defaults are fine; keep “Git from the command line and also from 3rd-party software.”
3. **Check:** Open a **new** PowerShell window and run:
   ```powershell
   git --version
   ```
   You should see something like `git version 2.43.0.windows.1`.

You do **not** need a GitHub account to clone the public OpenTelemetry Demo repo. You only need an account (and optionally a personal access token or SSH key) if you want to push your own changes or use private repos.

---

## Part 2: GitHub Basics (No Account Required for Demo)

- **GitHub** = website that hosts repositories (projects).
- **Clone** = download a copy of a repo to your PC.
- **Repository URL** for the demo: `https://github.com/open-telemetry/opentelemetry-demo`

To clone, you run one command in a folder where you want the project; Git will create a subfolder with the repo name. No login is required for this public repo.

**Secrets / tokens:** For just cloning a public repo you do **not** need a token. You need a token or SSH key only when:
- You push to GitHub (e.g. your own fork), or
- You use a backend that requires an API key/token to send telemetry (see Part 4).

---

## Part 3: Run the OpenTelemetry Demo (Default Backend, No Token)

The demo runs with a **built-in backend** (Jaeger + Grafana). You do **not** need to buy anything or configure tokens for this.

### Step 1: Choose a folder and clone the demo

Open **PowerShell**. Pick a folder (e.g. your user folder or `C:\Projects`). Example:

```powershell
cd $HOME
# Or: cd C:\Projects
```

Then run (one line):

```powershell
git clone https://github.com/open-telemetry/opentelemetry-demo.git
```

Result: a new folder `opentelemetry-demo` is created.

### Step 2: Go into the demo folder

```powershell
cd opentelemetry-demo
```

Check you’re in the right place:

```powershell
dir
```

You should see files/folders such as `docker-compose.yaml`, `src`, `README.md`.

### Step 3: Start the demo

Ensure **Docker Desktop is running**, then run:

```powershell
docker compose up --force-recreate --remove-orphans --detach
```

- **Meaning:** Start all services defined in `docker-compose.yaml`, recreate containers, remove old ones, run in background (`--detach`).
- First run can take several minutes (downloading images and building). Wait until the command returns and no errors are shown.

### Step 4: Confirm it’s running

- **Astronomy Shop (main app):**  
  Open a browser and go to: **http://localhost:8080/**
- **Load generator (traffic simulation):**  
  **http://localhost:8080/loadgen/**
- **Jaeger (traces):**  
  **http://localhost:8080/jaeger/ui/**
- **Grafana (dashboards/metrics):**  
  **http://localhost:8080/grafana/**  
  (Demo often has no login or default login; if it asks, try admin/admin and change password if prompted.)
- **Feature flags (scenarios):**  
  **http://localhost:8080/feature**

If these pages load, deployment succeeded. No token or license is required for this.

### Stopping the demo

From the same folder (`opentelemetry-demo`):

```powershell
docker compose down
```

---

## Part 4: Optional—Send Data to Your Own Backend (Tokens)

If you later use a **vendor or your own backend** (e.g. Grafana Cloud, Datadog, Jaeger in the cloud), they often give you:

- An **endpoint URL** (where to send data).
- A **data shipping token** or **API key** (secret string for authentication).

You never put real tokens in Git. You put them in environment variables or in a local file that is not committed.

### How the demo sends telemetry

The demo uses the **OpenTelemetry Collector**. Its configuration is built from two files:

- `src/otel-collector/otelcol-config.yml` — base config (do not change for adding a backend).
- `src/otel-collector/otelcol-config-extras.yml` — **you edit this** to add your backend.

### Example: Adding an OTLP HTTP backend with a token

1. Open `src/otel-collector/otelcol-config-extras.yml` in an editor.
2. Add an exporter and (if needed) a header for the token. Example for an OTLP HTTP endpoint:

```yaml
exporters:
  otlphttp/mybackend:
    endpoint: https://your-backend.example.com/v1/traces
    headers:
      Authorization: "Bearer YOUR_DATA_SHIPPING_TOKEN"
```

Replace:

- `https://your-backend.example.com/v1/traces` → your vendor’s OTLP endpoint.
- `YOUR_DATA_SHIPPING_TOKEN` → the token they gave you.

3. Tell the Collector to use this exporter. Under `service.pipelines`, **override** the pipelines you need. Example for traces (you must keep `spanmetrics` if the base config uses it):

```yaml
service:
  pipelines:
    traces:
      exporters: [spanmetrics, otlphttp/mybackend]
```

4. Save the file, then restart the demo:

```powershell
docker compose up --force-recreate --remove-orphans --detach
```

**Note:** Different backends use different exporter names and auth (e.g. headers, other keys). Check your backend’s docs for the exact YAML. The demo docs and [OpenTelemetry Collector exporters](https://github.com/open-telemetry/opentelemetry-collector-contrib/tree/main/exporter) list options.

---

## Part 5: Verification and Where to Look

| What to verify | Where to look | What you should see |
|----------------|---------------|----------------------|
| App is up | http://localhost:8080/ | Astronomy Shop storefront |
| Traffic is generated | http://localhost:8080/loadgen/ | Load generator UI; start/stop traffic |
| Traces are produced | http://localhost:8080/jaeger/ui/ | Select service “frontend” (or any), click “Find Traces” — list of traces |
| Metrics/dashboards | http://localhost:8080/grafana/ | Dashboards for services and Collector |
| Enable a scenario | http://localhost:8080/feature | Toggle a feature flag (e.g. Product Catalog Failure) |

**Traffic:** Use the load generator for a few minutes so enough traces and metrics are available in Jaeger and Grafana.

---

## Part 6: OpenTelemetry Data Model—Fields and Values

This section gives you the **exact field names and value types** you will see in backends and docs. Use it as a reference so “data, value, step, field” are clear.

### Traces and spans

A **trace** is one request end-to-end. A **span** is one step in that request.

| Field | Type | Meaning |
|-------|------|--------|
| `trace_id` | string (32 hex chars) | Same for every span in one trace. |
| `span_id` | string (16 hex chars) | Unique for this span. |
| `parent_span_id` | string (16 hex) or empty | `span_id` of the parent span. Root span has none. |
| `name` | string | Operation name (e.g. `GET /api/products`, `grpc.client.call`). |
| `kind` | enum | `SPAN_KIND_INTERNAL`, `SERVER`, `CLIENT`, `PRODUCER`, `CONSUMER`. |
| `start_time_unix_nano` | int64 | Start time in nanoseconds since Unix epoch. |
| `end_time_unix_nano` | int64 | End time in nanoseconds since Unix epoch. |
| `attributes` | key-value map | Custom or semantic attributes (see below). |
| `events` | list | Timestamped events (e.g. “exception,” “log”). |
| `status` | object | `code` (OK, ERROR) and optional `message`. |

**Common span attributes (examples):**

| Attribute key | Type | Meaning |
|---------------|------|--------|
| `http.method` | string | e.g. GET, POST |
| `http.url` / `http.scheme`, `http.host`, `http.target` | string | URL parts |
| `http.status_code` | int64 | HTTP status (200, 500, etc.) |
| `service.name` | string | Service identifier (e.g. `frontend`, `productcatalogservice`) |
| `error` | bool | true if this span represents an error |
| Demo-specific (e.g. `app.cache_hit`, `app.products.count`) | bool / int | Used in demo scenarios |

Duration of a span = `(end_time_unix_nano - start_time_unix_nano)` in nanoseconds.

### Metrics

| Concept | Field / idea | Type / meaning |
|--------|---------------|----------------|
| **Name** | Metric name (e.g. `http.server.request.duration`) | string |
| **Value** | Number for the data point | int64, float64 |
| **Unit** | Optional unit (e.g. `ms`, `bytes`, `1`) | string |
| **Time** | When the point was collected | Unix nanoseconds or equivalent |
| **Attributes** | Dimensions (e.g. `service.name`, `http.method`) | key-value map |

Metric **types** you’ll see:

- **Counter:** Only goes up (e.g. request count).
- **Gauge:** Can go up or down (e.g. memory in use).
- **Histogram:** Buckets + count/sum (e.g. latency distribution).

In Grafana/Prometheus-style UIs, you’ll see these as series with names, labels (attributes), and numeric values.

### Logs

| Field | Type | Meaning |
|-------|------|--------|
| `time_unix_nano` | int64 | Timestamp (nanoseconds since Unix epoch). |
| `body` | string or map | Message or structured body. |
| `severity_number` / `severity_text` | number / string | e.g. INFO, WARN, ERROR. |
| `attributes` | key-value map | Extra context (e.g. `service.name`, custom keys). |
| `trace_id` / `span_id` | string | Optional link to a trace/span. |

Using these names in queries (e.g. “filter where `http.status_code` = 500” or “group by `service.name`”) will work in any OTLP-capable backend.

---

## Part 7: Demo Scenarios (Hands-On Problem-Solving)

The demo uses **feature flags** to simulate failures and issues. You enable them at **http://localhost:8080/feature**, then use Jaeger and Grafana to find the cause. All free, no license.

### Scenario 1: Product Catalog Error

- **Goal:** See how errors show up in traces and how to find the failing request/span.
- **Feature flag:** `productCatalogFailure`
- **Effect:** `GetProduct` requests for product ID **`OLJCESPC7Z`** return an error.
- **Steps:**
  1. Enable `productCatalogFailure` in the Feature UI (http://localhost:8080/feature).
  2. In the Astronomy Shop, trigger requests that use that product (e.g. search or open a product that resolves to this ID).
  3. In **Jaeger**, select service `productcatalogservice` (or `frontend`), click “Find Traces,” look for traces with errors (red or status error).
  4. Open a trace and find the span with `error=true` or failed status; check attributes for product ID or `GetProduct`.

### Scenario 2: Service Failures (Ad and Cart)

- **Ad service:** Enable `adServiceFailure` — `GetAds` fails about 1/10 of the time.
- **Cart service:** Enable `cartServiceFailure` — `EmptyCart` always fails.
- **Goal:** Correlate failed user actions with trace spans and see which service and operation failed.
- **Steps:**
  1. Enable one or both flags at http://localhost:8080/feature.
  2. Generate traffic via http://localhost:8080/loadgen/.
  3. In **Jaeger**, filter by service `adservice` or `cartservice`, or search for traces with errors; inspect spans and status/attributes.

### Scenario 3: Memory Leak (Recommendation Service)

- **Goal:** Use **metrics** (memory, CPU) and **traces** (latency, cache behavior) to diagnose a memory issue.
- **Feature flag:** `recommendationServiceCacheFailure`
- **Effect:** Cache grows over time (memory leak simulation); some requests miss cache and are slow.
- **Steps:**
  1. Enable `recommendationServiceCacheFailure` at http://localhost:8080/feature.
  2. Let the demo run for ~10 minutes and generate traffic (loadgen).
  3. In **Grafana**, open the demo dashboards: look at **Recommendation Service** memory and CPU; note growth and latency (e.g. p95, p99).
  4. In **Jaeger**, select service `recommendationservice`, find slow traces. In span details, look for attributes like `app.cache_hit` (false = cache miss) and `app.products.count` (high when cache is ineffective).
  5. Compare traces with `app.cache_hit=true` (faster) vs `app.cache_hit=false` (slower, more work). This mirrors the [official memory-leak walkthrough](https://opentelemetry.io/docs/demo/feature-flags/recommendation-cache/).

### Other flags (for practice)

- `emailMemoryLeak` — memory leak in email service.
- `paymentServiceFailure` — payment charge fails.
- `llmRateLimitError` — 429 from mock LLM.

Use the same pattern: enable flag → generate traffic → use Jaeger (traces) and Grafana (metrics) to find the failing or slow operation and attributes.

---

## Part 8: Next Steps Toward Expertise

Once the demo runs and you’ve done the scenarios above, you can go deeper without paying for anything:

1. **OpenTelemetry concepts:**  
   [opentelemetry.io/docs](https://opentelemetry.io/docs/) — signals (traces, metrics, logs), SDK, Collector, semantic conventions.

2. **Collector config:**  
   [opentelemetry.io/docs/collector/configuration](https://opentelemetry.io/docs/collector/configuration) — receivers, processors, exporters. Experiment with `otelcol-config-extras.yml`.

3. **Docker:**  
   [docs.docker.com](https://docs.docker.com/) — images, containers, `docker compose`. Run a single service from the demo’s `docker-compose.yaml` to see how it’s wired.

4. **Kubernetes (when you’re ready):**  
   Demo docs: [Kubernetes deployment](https://opentelemetry.io/docs/demo/kubernetes-deployment/). Learn pods, services, and then Helm if you want to deploy the demo on K8s.

5. **Git:**  
   Practice: fork the demo repo on GitHub, clone your fork, make a small change (e.g. in a comment or config), commit, push. You’ll need a GitHub account and a personal access token or SSH key for push.

6. **Secrets:**  
   Never put tokens in `otelcol-config-extras.yml` if that file is committed. Use environment variables or a secret store; reference them in the config (e.g. `headers: Authorization: ${env:MY_TOKEN}` if your Collector supports it).

7. **This repo (opentelemetry-cpp):**  
   You’re in the C++ SDK repo. For app instrumentation in C++, use this SDK to create spans, metrics, and logs and export them via OTLP to the same Collector or backend you used in the demo.

---

## Quick Reference Card

| Task | Command or URL |
|------|-----------------|
| Clone demo | `git clone https://github.com/open-telemetry/opentelemetry-demo.git` |
| Start demo | `cd opentelemetry-demo` then `docker compose up --force-recreate --remove-orphans --detach` |
| Stop demo | `docker compose down` |
| Shop | http://localhost:8080/ |
| Load generator | http://localhost:8080/loadgen/ |
| Jaeger | http://localhost:8080/jaeger/ui/ |
| Grafana | http://localhost:8080/grafana/ |
| Feature flags | http://localhost:8080/feature |
| Collector extras config | `src/otel-collector/otelcol-config-extras.yml` |

| Trace fields | `trace_id`, `span_id`, `parent_span_id`, `name`, `kind`, `start_time_unix_nano`, `end_time_unix_nano`, `attributes`, `status` |
| Metric idea | name, value, unit, timestamp, attributes (dimensions) |
| Log fields | `time_unix_nano`, `body`, `severity_number`/`severity_text`, `attributes` |

---

*End of handbook. If you follow the steps in order and use this glossary and data model section when something is unclear, you can run the demo, verify telemetry, and complete the scenarios without needing Docker/Kubernetes/Git/Helm/Terraform/Ansible experience beforehand. For “bring your own backend,” only Part 4 and your vendor’s docs are needed for tokens and exporter config.*
