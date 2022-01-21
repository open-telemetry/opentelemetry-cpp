window.BENCHMARK_DATA = {
  "lastUpdate": 1642789437387,
  "repoUrl": "https://github.com/open-telemetry/opentelemetry-cpp",
  "entries": {
    "OpenTelemetry-cpp api Benchmark": [
      {
        "commit": {
          "author": {
            "email": "71217171+esigo@users.noreply.github.com",
            "name": "Ehsan Saei",
            "username": "esigo"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "2a821fdfa5a7ef3850319483df002f42f6d8b691",
          "message": "Continuous benchmark tests as part of the CI (#1174)",
          "timestamp": "2022-01-21T10:12:39-08:00",
          "tree_id": "be73c679916ac58ef57ef857fd51e17edd33b0c8",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/2a821fdfa5a7ef3850319483df002f42f6d8b691"
        },
        "date": 1642789435066,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 1.9713371225275436,
            "unit": "ns/iter",
            "extra": "iterations: 73631893\ncpu: 1.969711684582114 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 1.9849431718590052,
            "unit": "ns/iter",
            "extra": "iterations: 70701000\ncpu: 1.982293036873595 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 10.455751419067383,
            "unit": "ns/iter",
            "extra": "iterations: 10000000\ncpu: 10.303239999999997 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 1.9770779038620534,
            "unit": "ns/iter",
            "extra": "iterations: 71351395\ncpu: 1.9682264095887683 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 60.024459759060775,
            "unit": "ns/iter",
            "extra": "iterations: 2323050\ncpu: 59.86625341684424 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 345.8781927771802,
            "unit": "ns/iter",
            "extra": "iterations: 404643\ncpu: 345.7282098046921 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1118.4829147396417,
            "unit": "ns/iter",
            "extra": "iterations: 117985\ncpu: 1116.8411238716787 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 273.3894446032108,
            "unit": "ns/iter",
            "extra": "iterations: 584319\ncpu: 253.6340594777854 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 1118.3607307388927,
            "unit": "ns/iter",
            "extra": "iterations: 130436\ncpu: 1114.0229691189547 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.41814499500534297,
            "unit": "ms/iter",
            "extra": "iterations: 382\ncpu: 0.12470994764397908 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.24549543422504064,
            "unit": "ms/iter",
            "extra": "iterations: 548\ncpu: 0.24040200729927005 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.11842958047532996,
            "unit": "ms/iter",
            "extra": "iterations: 1217\ncpu: 0.11616318816762534 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.24462128024932753,
            "unit": "ms/iter",
            "extra": "iterations: 562\ncpu: 0.23970320284697513 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.12417002871331674,
            "unit": "ms/iter",
            "extra": "iterations: 1139\ncpu: 0.12166707638279192 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.24542747954491478,
            "unit": "ms/iter",
            "extra": "iterations: 551\ncpu: 0.24044936479128864 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 6.99993371963501,
            "unit": "ms/iter",
            "extra": "iterations: 20\ncpu: 6.977584999999992 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 28.528785705566406,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 28.46837999999998 ms\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 4174.50993858881,
            "unit": "ns/iter",
            "extra": "iterations: 32956\ncpu: 4168.4458065299195 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.8549879147370685,
            "unit": "ns/iter",
            "extra": "iterations: 76820509\ncpu: 1.8509262936542115 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 85279.2461110723,
            "unit": "ns/iter",
            "extra": "iterations: 1649\ncpu: 85127.41055184967 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.8639457452441763,
            "unit": "ns/iter",
            "extra": "iterations: 76300515\ncpu: 1.8615510000161863 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 761.6070541831026,
            "unit": "ns/iter",
            "extra": "iterations: 180514\ncpu: 759.6075650642056 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 18769.334008566573,
            "unit": "ns/iter",
            "extra": "iterations: 7352\ncpu: 18746.368335146894 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 3131.4073063655583,
            "unit": "ns/iter",
            "extra": "iterations: 44453\ncpu: 3127.9598677254635 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 53809.26044305898,
            "unit": "ns/iter",
            "extra": "iterations: 2604\ncpu: 53724.92319508448 ns\nthreads: 1"
          }
        ]
      }
    ],
    "OpenTelemetry-cpp sdk Benchmark": [
      {
        "commit": {
          "author": {
            "email": "71217171+esigo@users.noreply.github.com",
            "name": "Ehsan Saei",
            "username": "esigo"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "2a821fdfa5a7ef3850319483df002f42f6d8b691",
          "message": "Continuous benchmark tests as part of the CI (#1174)",
          "timestamp": "2022-01-21T10:12:39-08:00",
          "tree_id": "be73c679916ac58ef57ef857fd51e17edd33b0c8",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/2a821fdfa5a7ef3850319483df002f42f6d8b691"
        },
        "date": 1642789435730,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.9117067037663351,
            "unit": "ns/iter",
            "extra": "iterations: 72271493\ncpu: 1.9066909272235462 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.8227035603326738,
            "unit": "ns/iter",
            "extra": "iterations: 78565616\ncpu: 1.8199297260012577 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 112.56228621339717,
            "unit": "ns/iter",
            "extra": "iterations: 1443818\ncpu: 92.91524277990717 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 438.8554565278875,
            "unit": "ns/iter",
            "extra": "iterations: 387711\ncpu: 370.43700075571775 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 51.876289852983184,
            "unit": "ns/iter",
            "extra": "iterations: 3328475\ncpu: 40.835157241679745 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 53.87171705168634,
            "unit": "ns/iter",
            "extra": "iterations: 3428927\ncpu: 41.210326145759325 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 60.987092738232214,
            "unit": "ns/iter",
            "extra": "iterations: 2978318\ncpu: 45.85611744615579 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 32.56641605784322,
            "unit": "ns/iter",
            "extra": "iterations: 5834987\ncpu: 23.808741990342032 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 1165.6672925212822,
            "unit": "ns/iter",
            "extra": "iterations: 156067\ncpu: 895.3000954718168 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 1195.5183568188015,
            "unit": "ns/iter",
            "extra": "iterations: 154725\ncpu: 873.373404427208 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 548949.7184753418,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 20026.000000000004 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 7803626.06048584,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 356358.00000000006 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 12077944.278717041,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 670090 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 350474.14826183784,
            "unit": "ns/iter",
            "extra": "iterations: 7175\ncpu: 19817.142857142862 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 5015068.054199219,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 358349 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 7476060.390472412,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 691420.0000000003 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.8738786763042286,
            "unit": "ns/iter",
            "extra": "iterations: 48762308\ncpu: 2.6293320652500696 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 7.5740599959019885,
            "unit": "ns/iter",
            "extra": "iterations: 18602105\ncpu: 7.566606037327498 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}