window.BENCHMARK_DATA = {
  "lastUpdate": 1643217088551,
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
      },
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
          "id": "a605fd9116723cb33769dafb25a9bb88323a7674",
          "message": "importing gsl::span if std::span is not available (#1167)",
          "timestamp": "2022-01-24T19:14:20-08:00",
          "tree_id": "06217abfa84c48eaac7328bb508b450be04ab671",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/a605fd9116723cb33769dafb25a9bb88323a7674"
        },
        "date": 1643081177795,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 2.035417355594864,
            "unit": "ns/iter",
            "extra": "iterations: 68957365\ncpu: 2.0305474259348513 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 2.038682292862635,
            "unit": "ns/iter",
            "extra": "iterations: 68896282\ncpu: 2.0323839826363925 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 10.785231145489012,
            "unit": "ns/iter",
            "extra": "iterations: 13011031\ncpu: 10.759693063524326 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 2.414909333857145,
            "unit": "ns/iter",
            "extra": "iterations: 67771668\ncpu: 2.0696303357916475 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 63.50991624204668,
            "unit": "ns/iter",
            "extra": "iterations: 2204547\ncpu: 63.49494930250978 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 378.06999423791956,
            "unit": "ns/iter",
            "extra": "iterations: 370181\ncpu: 378.0639741099623 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1202.9954670102647,
            "unit": "ns/iter",
            "extra": "iterations: 116557\ncpu: 1201.7390632909226 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 276.80890852182966,
            "unit": "ns/iter",
            "extra": "iterations: 506705\ncpu: 276.69867082424685 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 1136.069193061752,
            "unit": "ns/iter",
            "extra": "iterations: 123206\ncpu: 1136.011233219161 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 4422.375999075199,
            "unit": "ns/iter",
            "extra": "iterations: 31677\ncpu: 4408.447769675158 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.9588533470909715,
            "unit": "ns/iter",
            "extra": "iterations: 72215201\ncpu: 1.940210621306725 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 89800.17535540523,
            "unit": "ns/iter",
            "extra": "iterations: 1568\ncpu: 89376.65816326531 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.9406738168274502,
            "unit": "ns/iter",
            "extra": "iterations: 72231595\ncpu: 1.9392552524971935 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 811.7549718823063,
            "unit": "ns/iter",
            "extra": "iterations: 175601\ncpu: 810.3404878104338 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 20312.35362848978,
            "unit": "ns/iter",
            "extra": "iterations: 6922\ncpu: 20253.49609939322 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 3290.5788329963652,
            "unit": "ns/iter",
            "extra": "iterations: 42655\ncpu: 3273.5576134099174 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 56246.80492336882,
            "unit": "ns/iter",
            "extra": "iterations: 2494\ncpu: 55999.157979150004 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.1306999741109697,
            "unit": "ms/iter",
            "extra": "iterations: 1049\ncpu: 0.12661877979027647 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.26043438820089876,
            "unit": "ms/iter",
            "extra": "iterations: 522\ncpu: 0.2510716475095785 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.12984431067178415,
            "unit": "ms/iter",
            "extra": "iterations: 1075\ncpu: 0.12671823255813955 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.2552961736536113,
            "unit": "ms/iter",
            "extra": "iterations: 547\ncpu: 0.24882157221206588 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.13323357577697473,
            "unit": "ms/iter",
            "extra": "iterations: 1085\ncpu: 0.12646792626728112 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.25457830677245186,
            "unit": "ms/iter",
            "extra": "iterations: 538\ncpu: 0.24799739776951682 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 7.3459524857370475,
            "unit": "ms/iter",
            "extra": "iterations: 19\ncpu: 7.285089473684202 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 30.41224479675293,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 29.33347999999998 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "lalit_fin@yahoo.com",
            "name": "Lalit Kumar Bhasin",
            "username": "lalitb"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "16a9c53680079a7a5f6728dd4bf4938efa214b30",
          "message": "Add @esigo as approver (#1183)",
          "timestamp": "2022-01-26T08:58:05-08:00",
          "tree_id": "ce15f948274aaca62ea8d3f05725540721d025fc",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/16a9c53680079a7a5f6728dd4bf4938efa214b30"
        },
        "date": 1643217084900,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 2.829701319634693,
            "unit": "ns/iter",
            "extra": "iterations: 49172324\ncpu: 2.812887184262432 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 2.403808459117884,
            "unit": "ns/iter",
            "extra": "iterations: 61647123\ncpu: 2.3925852955051936 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 10.575232914833586,
            "unit": "ns/iter",
            "extra": "iterations: 13275553\ncpu: 10.544796137682551 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 2.3710225319448957,
            "unit": "ns/iter",
            "extra": "iterations: 60337025\ncpu: 2.3356720023899107 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 88.22113193950163,
            "unit": "ns/iter",
            "extra": "iterations: 2237244\ncpu: 61.79942822508408 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 361.4483495910835,
            "unit": "ns/iter",
            "extra": "iterations: 389472\ncpu: 361.3828465204174 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1169.648386164583,
            "unit": "ns/iter",
            "extra": "iterations: 122319\ncpu: 1167.8725300239537 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 276.9173606044883,
            "unit": "ns/iter",
            "extra": "iterations: 469235\ncpu: 276.90602789646977 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 1086.7818530986826,
            "unit": "ns/iter",
            "extra": "iterations: 132339\ncpu: 1073.225579761068 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 4251.039431895317,
            "unit": "ns/iter",
            "extra": "iterations: 33159\ncpu: 4240.2002472933455 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 2.620136679150258,
            "unit": "ns/iter",
            "extra": "iterations: 54638411\ncpu: 2.6081596699435496 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 83254.45140066322,
            "unit": "ns/iter",
            "extra": "iterations: 1630\ncpu: 83091.77914110426 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 2.423414392180966,
            "unit": "ns/iter",
            "extra": "iterations: 60129450\ncpu: 2.417261425141922 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 748.2573400043215,
            "unit": "ns/iter",
            "extra": "iterations: 184108\ncpu: 746.3472526995026 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 17949.473770288892,
            "unit": "ns/iter",
            "extra": "iterations: 6941\ncpu: 17432.51692839647 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 3063.1555530764563,
            "unit": "ns/iter",
            "extra": "iterations: 46664\ncpu: 3058.58477627293 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 61596.11305905954,
            "unit": "ns/iter",
            "extra": "iterations: 2497\ncpu: 61477.092511013274 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.2855494327503792,
            "unit": "ms/iter",
            "extra": "iterations: 461\ncpu: 0.1526236442516269 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.4627482318655872,
            "unit": "ms/iter",
            "extra": "iterations: 429\ncpu: 0.3408344988344989 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.3618071065146542,
            "unit": "ms/iter",
            "extra": "iterations: 845\ncpu: 0.15467408284023673 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.35205437986419613,
            "unit": "ms/iter",
            "extra": "iterations: 459\ncpu: 0.30811220043572984 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.27893180634650017,
            "unit": "ms/iter",
            "extra": "iterations: 359\ncpu: 0.1540782729805012 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.33621149738942546,
            "unit": "ms/iter",
            "extra": "iterations: 381\ncpu: 0.29416115485564287 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 10.01443862915039,
            "unit": "ms/iter",
            "extra": "iterations: 10\ncpu: 7.703540000000008 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 30.318641662597656,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 28.310159999999975 ms\nthreads: 1"
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
      },
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
          "id": "a605fd9116723cb33769dafb25a9bb88323a7674",
          "message": "importing gsl::span if std::span is not available (#1167)",
          "timestamp": "2022-01-24T19:14:20-08:00",
          "tree_id": "06217abfa84c48eaac7328bb508b450be04ab671",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/a605fd9116723cb33769dafb25a9bb88323a7674"
        },
        "date": 1643081180520,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.9495525575436436,
            "unit": "ns/iter",
            "extra": "iterations: 71664039\ncpu: 1.949361240998432 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.9520363720376295,
            "unit": "ns/iter",
            "extra": "iterations: 71844610\ncpu: 1.9498720920052324 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 101.93446023733772,
            "unit": "ns/iter",
            "extra": "iterations: 1379881\ncpu: 101.52382705465183 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 415.10401042244536,
            "unit": "ns/iter",
            "extra": "iterations: 354323\ncpu: 395.585101729213 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 48.621242460338294,
            "unit": "ns/iter",
            "extra": "iterations: 3138542\ncpu: 44.65130624347227 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 88.78922163992982,
            "unit": "ns/iter",
            "extra": "iterations: 3135695\ncpu: 44.65357759603534 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 61.250796372663146,
            "unit": "ns/iter",
            "extra": "iterations: 2898887\ncpu: 48.28339290217251 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 30.138203077310617,
            "unit": "ns/iter",
            "extra": "iterations: 5698702\ncpu: 24.516302133362995 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 1027.1754457650509,
            "unit": "ns/iter",
            "extra": "iterations: 144966\ncpu: 961.5454658333682 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 981.6639092377249,
            "unit": "ns/iter",
            "extra": "iterations: 145532\ncpu: 972.6355715581457 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 520958.66203308105,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 20034.5 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 7512216.567993164,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 70034 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 12050595.2835083,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 329811 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 397578.52218996646,
            "unit": "ns/iter",
            "extra": "iterations: 6911\ncpu: 20438.894515989006 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 5858781.337738037,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 67155.99999999988 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 7451670.169830322,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 304702.00000000006 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 3.528644502171626,
            "unit": "ns/iter",
            "extra": "iterations: 46661845\ncpu: 2.9992084539306156 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 11.461957363503585,
            "unit": "ns/iter",
            "extra": "iterations: 15615955\ncpu: 8.973725910455046 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "lalit_fin@yahoo.com",
            "name": "Lalit Kumar Bhasin",
            "username": "lalitb"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "16a9c53680079a7a5f6728dd4bf4938efa214b30",
          "message": "Add @esigo as approver (#1183)",
          "timestamp": "2022-01-26T08:58:05-08:00",
          "tree_id": "ce15f948274aaca62ea8d3f05725540721d025fc",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/16a9c53680079a7a5f6728dd4bf4938efa214b30"
        },
        "date": 1643217084570,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 2.4801477784085297,
            "unit": "ns/iter",
            "extra": "iterations: 51491980\ncpu: 2.471328544755902 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 2.54699428047278,
            "unit": "ns/iter",
            "extra": "iterations: 50160873\ncpu: 2.528008234625423 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 98.8380651873106,
            "unit": "ns/iter",
            "extra": "iterations: 1404297\ncpu: 98.36993171672378 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 381.64588309826837,
            "unit": "ns/iter",
            "extra": "iterations: 364348\ncpu: 380.8183385115328 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 44.65236502607283,
            "unit": "ns/iter",
            "extra": "iterations: 3205356\ncpu: 44.10000012479114 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 44.7058684382855,
            "unit": "ns/iter",
            "extra": "iterations: 3361554\ncpu: 44.086782482149644 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 47.164607351833915,
            "unit": "ns/iter",
            "extra": "iterations: 2986361\ncpu: 46.85756343590075 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 23.91560659880561,
            "unit": "ns/iter",
            "extra": "iterations: 6024096\ncpu: 23.87749132815947 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 921.1970567184735,
            "unit": "ns/iter",
            "extra": "iterations: 142542\ncpu: 919.5710737887778 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 953.2004164572869,
            "unit": "ns/iter",
            "extra": "iterations: 147408\ncpu: 935.4146314989689 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 8270242.214202881,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 96902 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 2721480.308399256,
            "unit": "ns/iter",
            "extra": "iterations: 686\ncpu: 204605.5393586006 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 11638143.062591553,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 488702 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 374176.55309148144,
            "unit": "ns/iter",
            "extra": "iterations: 5953\ncpu: 24970.03191668067 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 3790826.9223526926,
            "unit": "ns/iter",
            "extra": "iterations: 611\ncpu: 223659.08346972178 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 8300862.3123168945,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 480365.9999999998 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 4.076539737265613,
            "unit": "ns/iter",
            "extra": "iterations: 41378862\ncpu: 3.360522577928799 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 8.340692533361482,
            "unit": "ns/iter",
            "extra": "iterations: 16949153\ncpu: 8.264643076854638 ns\nthreads: 1"
          }
        ]
      }
    ],
    "OpenTelemetry-cpp exporters Benchmark": [
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
        "date": 1642789435314,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 31.96105513145793,
            "unit": "ns/iter",
            "extra": "iterations: 4211000\ncpu: 31.935620992638327 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 85.41997873558188,
            "unit": "ns/iter",
            "extra": "iterations: 1590000\ncpu: 85.41339622641512 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 727.9445143306957,
            "unit": "ns/iter",
            "extra": "iterations: 204000\ncpu: 727.8313725490198 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 1716770.5279242373,
            "unit": "ns/iter",
            "extra": "iterations: 327\ncpu: 367312.23241590225 ns\nthreads: 1"
          }
        ]
      },
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
          "id": "a605fd9116723cb33769dafb25a9bb88323a7674",
          "message": "importing gsl::span if std::span is not available (#1167)",
          "timestamp": "2022-01-24T19:14:20-08:00",
          "tree_id": "06217abfa84c48eaac7328bb508b450be04ab671",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/a605fd9116723cb33769dafb25a9bb88323a7674"
        },
        "date": 1643081179157,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 34.817587283307745,
            "unit": "ns/iter",
            "extra": "iterations: 4023000\ncpu: 34.799751429281635 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 96.02210827029694,
            "unit": "ns/iter",
            "extra": "iterations: 1468000\ncpu: 95.49032697547686 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 796.1067286404697,
            "unit": "ns/iter",
            "extra": "iterations: 176000\ncpu: 795.7551136363638 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 1950522.3157200427,
            "unit": "ns/iter",
            "extra": "iterations: 383\ncpu: 360248.82506527426 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "lalit_fin@yahoo.com",
            "name": "Lalit Kumar Bhasin",
            "username": "lalitb"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "16a9c53680079a7a5f6728dd4bf4938efa214b30",
          "message": "Add @esigo as approver (#1183)",
          "timestamp": "2022-01-26T08:58:05-08:00",
          "tree_id": "ce15f948274aaca62ea8d3f05725540721d025fc",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/16a9c53680079a7a5f6728dd4bf4938efa214b30"
        },
        "date": 1643217086792,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 33.49761652752636,
            "unit": "ns/iter",
            "extra": "iterations: 4305000\ncpu: 33.498536585365855 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 82.40480551924304,
            "unit": "ns/iter",
            "extra": "iterations: 1701000\ncpu: 82.38106995884776 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 711.5954562172371,
            "unit": "ns/iter",
            "extra": "iterations: 193000\ncpu: 711.6072538860102 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 2111882.7556308946,
            "unit": "ns/iter",
            "extra": "iterations: 304\ncpu: 468960.8552631578 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}