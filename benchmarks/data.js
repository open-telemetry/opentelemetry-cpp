window.BENCHMARK_DATA = {
  "lastUpdate": 1645086276423,
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
      },
      {
        "commit": {
          "author": {
            "email": "Tom.Tan@microsoft.com",
            "name": "Tom Tan",
            "username": "ThomsonTan"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "033b16fe19f2ec13f4b96a74b3b33a04e1bc83a2",
          "message": "Prepare for 1.2.0 release (#1188)",
          "timestamp": "2022-02-01T11:58:47-08:00",
          "tree_id": "0dac4978a6de140a9ef209e3b12c97501a37329d",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/033b16fe19f2ec13f4b96a74b3b33a04e1bc83a2"
        },
        "date": 1643746256468,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 2.0344050585992126,
            "unit": "ns/iter",
            "extra": "iterations: 68918666\ncpu: 2.015697750156685 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 2.0077066671082604,
            "unit": "ns/iter",
            "extra": "iterations: 70410139\ncpu: 2.005955420710077 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 10.722611983141238,
            "unit": "ns/iter",
            "extra": "iterations: 13032227\ncpu: 10.6320968779933 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 2.0708184874913083,
            "unit": "ns/iter",
            "extra": "iterations: 68863071\ncpu: 2.011291363988109 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 65.90791150928014,
            "unit": "ns/iter",
            "extra": "iterations: 2214990\ncpu: 63.61437297685317 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 382.8578927323167,
            "unit": "ns/iter",
            "extra": "iterations: 366997\ncpu: 379.75215056253876 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1210.4802018357932,
            "unit": "ns/iter",
            "extra": "iterations: 116037\ncpu: 1201.7804665753165 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 288.7292784087512,
            "unit": "ns/iter",
            "extra": "iterations: 490000\ncpu: 285.7214285714287 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 1149.8364145535486,
            "unit": "ns/iter",
            "extra": "iterations: 122802\ncpu: 1131.7217960619541 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 4380.663284674274,
            "unit": "ns/iter",
            "extra": "iterations: 31827\ncpu: 4373.8021176988095 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.9385083224551196,
            "unit": "ns/iter",
            "extra": "iterations: 72131857\ncpu: 1.9366657924805684 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 88116.30171616771,
            "unit": "ns/iter",
            "extra": "iterations: 1577\ncpu: 87834.24223208624 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.9494731555336147,
            "unit": "ns/iter",
            "extra": "iterations: 72911350\ncpu: 1.923865077247919 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 784.0471214561861,
            "unit": "ns/iter",
            "extra": "iterations: 180188\ncpu: 782.8234954602967 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 20313.910105102434,
            "unit": "ns/iter",
            "extra": "iterations: 6926\ncpu: 20087.265376840893 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 3266.195384104937,
            "unit": "ns/iter",
            "extra": "iterations: 44377\ncpu: 3261.0856975460233 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 65647.65704976262,
            "unit": "ns/iter",
            "extra": "iterations: 2507\ncpu: 55130.55444754694 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.8355248181117063,
            "unit": "ms/iter",
            "extra": "iterations: 173\ncpu: 0.13651098265895956 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.5826679435936181,
            "unit": "ms/iter",
            "extra": "iterations: 370\ncpu: 0.25519945945945943 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.5571181957538311,
            "unit": "ms/iter",
            "extra": "iterations: 260\ncpu: 0.13453153846153842 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 1.19476318359375,
            "unit": "ms/iter",
            "extra": "iterations: 100\ncpu: 0.36604 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.49034542507595485,
            "unit": "ms/iter",
            "extra": "iterations: 225\ncpu: 0.1328222222222222 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 1.3277220726013184,
            "unit": "ms/iter",
            "extra": "iterations: 100\ncpu: 0.4752380000000001 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 33.80290667215983,
            "unit": "ms/iter",
            "extra": "iterations: 6\ncpu: 7.697600000000008 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 58.45087766647339,
            "unit": "ms/iter",
            "extra": "iterations: 4\ncpu: 29.798200000000012 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "hamed0381@gmail.com",
            "name": "Hamed Mansouri",
            "username": "hamedprog"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a320739a46000dfedba7d7a8ba60119d2a6dc5f7",
          "message": "Update GettingStarted.rst (#1189)",
          "timestamp": "2022-02-02T13:58:45-08:00",
          "tree_id": "9a560ec05ec795317cff1ee6e6f328f6923fdf3d",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/a320739a46000dfedba7d7a8ba60119d2a6dc5f7"
        },
        "date": 1643839937664,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 3.5093154789474905,
            "unit": "ns/iter",
            "extra": "iterations: 50564517\ncpu: 2.8529195680836823 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 3.581691591186516,
            "unit": "ns/iter",
            "extra": "iterations: 49045367\ncpu: 2.588162914552153 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 15.055647518180631,
            "unit": "ns/iter",
            "extra": "iterations: 13364390\ncpu: 10.825080680824188 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 3.1572477051898797,
            "unit": "ns/iter",
            "extra": "iterations: 58952581\ncpu: 2.29578922083157 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 60.89867675780558,
            "unit": "ns/iter",
            "extra": "iterations: 2116972\ncpu: 60.78313742458569 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 383.22753111860226,
            "unit": "ns/iter",
            "extra": "iterations: 377633\ncpu: 381.7521244170928 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1173.9135904751486,
            "unit": "ns/iter",
            "extra": "iterations: 120145\ncpu: 1172.5207041491535 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 267.4728520797355,
            "unit": "ns/iter",
            "extra": "iterations: 527283\ncpu: 266.8381116023085 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 1127.6354971402736,
            "unit": "ns/iter",
            "extra": "iterations: 128587\ncpu: 1125.5725695443546 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 5586.831925339821,
            "unit": "ns/iter",
            "extra": "iterations: 30963\ncpu: 4314.226657623615 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 3.2049225754952646,
            "unit": "ns/iter",
            "extra": "iterations: 51885674\ncpu: 2.743524156590893 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 118610.02931026829,
            "unit": "ns/iter",
            "extra": "iterations: 1595\ncpu: 87436.23824451411 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 3.573087348228322,
            "unit": "ns/iter",
            "extra": "iterations: 50535496\ncpu: 2.6991424008186264 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 1031.013874296247,
            "unit": "ns/iter",
            "extra": "iterations: 186081\ncpu: 754.5284042970537 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 23230.67273459243,
            "unit": "ns/iter",
            "extra": "iterations: 7276\ncpu: 18067.413413963706 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 3161.7332457120315,
            "unit": "ns/iter",
            "extra": "iterations: 44364\ncpu: 3146.652691371381 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 56070.188918375454,
            "unit": "ns/iter",
            "extra": "iterations: 2514\ncpu: 55966.86555290374 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.1590362616947719,
            "unit": "ms/iter",
            "extra": "iterations: 840\ncpu: 0.15561500000000003 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.32970854335612754,
            "unit": "ms/iter",
            "extra": "iterations: 421\ncpu: 0.31823111638954876 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.18764373201590318,
            "unit": "ms/iter",
            "extra": "iterations: 832\ncpu: 0.17722680288461545 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.3310418823390331,
            "unit": "ms/iter",
            "extra": "iterations: 412\ncpu: 0.323079854368932 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.163787722853085,
            "unit": "ms/iter",
            "extra": "iterations: 898\ncpu: 0.16027082405345217 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.30809187553298306,
            "unit": "ms/iter",
            "extra": "iterations: 355\ncpu: 0.30238253521126773 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 6.9709062576293945,
            "unit": "ms/iter",
            "extra": "iterations: 20\ncpu: 6.939695000000002 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 27.850055694580078,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 27.791140000000023 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "dupadhya@redhat.com",
            "name": "Deepika Upadhyay",
            "username": "ideepika"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e9127fc49dbe63b6f37ead0e158bdb0b78a6b984",
          "message": "cmake: thrift requires boost headers, include them as Boost_INCLUDE_DIRS (#1100)",
          "timestamp": "2022-02-04T10:43:33-08:00",
          "tree_id": "0e7d81535f83b70283f422d9e187c9f58e5cf975",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e9127fc49dbe63b6f37ead0e158bdb0b78a6b984"
        },
        "date": 1644000903651,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 3684.464684806684,
            "unit": "ns/iter",
            "extra": "iterations: 38088\ncpu: 3682.3015122873353 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.617021671432585,
            "unit": "ns/iter",
            "extra": "iterations: 86624551\ncpu: 1.6159102515867585 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 74438.73492522351,
            "unit": "ns/iter",
            "extra": "iterations: 1884\ncpu: 74326.8577494692 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.6169463354277098,
            "unit": "ns/iter",
            "extra": "iterations: 86659404\ncpu: 1.615552306360197 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 672.8967867079785,
            "unit": "ns/iter",
            "extra": "iterations: 211931\ncpu: 672.062605281908 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 16938.721728201683,
            "unit": "ns/iter",
            "extra": "iterations: 8321\ncpu: 16890.313664223046 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 2718.995133117253,
            "unit": "ns/iter",
            "extra": "iterations: 51396\ncpu: 2718.157055023737 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 46350.42587995291,
            "unit": "ns/iter",
            "extra": "iterations: 3009\ncpu: 46325.157859754065 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 55.33962965668708,
            "unit": "ns/iter",
            "extra": "iterations: 2533992\ncpu: 55.245833451723605 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 317.4356172283638,
            "unit": "ns/iter",
            "extra": "iterations: 442328\ncpu: 316.7441355736015 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1042.4418281427045,
            "unit": "ns/iter",
            "extra": "iterations: 138559\ncpu: 1010.7932360943711 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 232.53638511876906,
            "unit": "ns/iter",
            "extra": "iterations: 605714\ncpu: 230.93572213949142 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 952.945296580664,
            "unit": "ns/iter",
            "extra": "iterations: 147495\ncpu: 948.9040306451062 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 1.7048012770976169,
            "unit": "ns/iter",
            "extra": "iterations: 83145760\ncpu: 1.684641525917858 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 1.74442913330324,
            "unit": "ns/iter",
            "extra": "iterations: 82647068\ncpu: 1.6928586020740626 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 8.993289804735605,
            "unit": "ns/iter",
            "extra": "iterations: 15630879\ncpu: 8.960084714365712 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 1.701385362281182,
            "unit": "ns/iter",
            "extra": "iterations: 81403395\ncpu: 1.695049696637345 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.1096113814109283,
            "unit": "ms/iter",
            "extra": "iterations: 1146\ncpu: 0.10659432809773126 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.21455467941213646,
            "unit": "ms/iter",
            "extra": "iterations: 649\ncpu: 0.2093620955315871 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.10957754859628603,
            "unit": "ms/iter",
            "extra": "iterations: 1290\ncpu: 0.10706116279069766 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.21528204282124838,
            "unit": "ms/iter",
            "extra": "iterations: 648\ncpu: 0.21015848765432096 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.10891370677283159,
            "unit": "ms/iter",
            "extra": "iterations: 1291\ncpu: 0.10643524399690157 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.2145194125653048,
            "unit": "ms/iter",
            "extra": "iterations: 649\ncpu: 0.20954391371340533 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 10.680447925220836,
            "unit": "ms/iter",
            "extra": "iterations: 22\ncpu: 6.556872727272724 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 25.57826042175293,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 25.554900000000025 ms\nthreads: 1"
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
          "id": "e1b4a49f9b673cbdce4b992ae8fc712e49c39c50",
          "message": "disable failing test (#1193)",
          "timestamp": "2022-02-04T11:52:45-08:00",
          "tree_id": "ed584794fbdf5a6eb267184582f6fa13e64001f2",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e1b4a49f9b673cbdce4b992ae8fc712e49c39c50"
        },
        "date": 1644005084327,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 4454.390666069996,
            "unit": "ns/iter",
            "extra": "iterations: 38024\ncpu: 3557.0139911634756 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 2.5935084875968184,
            "unit": "ns/iter",
            "extra": "iterations: 68437905\ncpu: 2.126216458554657 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 89253.48363918373,
            "unit": "ns/iter",
            "extra": "iterations: 1906\ncpu: 71717.78593913955 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 2.5899803687761187,
            "unit": "ns/iter",
            "extra": "iterations: 66934084\ncpu: 2.0871922292983047 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 733.1112875949027,
            "unit": "ns/iter",
            "extra": "iterations: 225566\ncpu: 596.162985556334 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 19702.69225292353,
            "unit": "ns/iter",
            "extra": "iterations: 9836\ncpu: 14974.217161447725 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 3022.2746862528684,
            "unit": "ns/iter",
            "extra": "iterations: 55796\ncpu: 2527.3119936913067 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 63172.4561134948,
            "unit": "ns/iter",
            "extra": "iterations: 3041\ncpu: 46429.49687602763 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 58.592755282387685,
            "unit": "ns/iter",
            "extra": "iterations: 2564281\ncpu: 58.1717448282774 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 325.5605435480375,
            "unit": "ns/iter",
            "extra": "iterations: 422903\ncpu: 324.3653982118831 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1019.7405524852344,
            "unit": "ns/iter",
            "extra": "iterations: 137841\ncpu: 1018.8347443793934 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 229.80113574470755,
            "unit": "ns/iter",
            "extra": "iterations: 600650\ncpu: 228.9176725214352 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 954.4182604871754,
            "unit": "ns/iter",
            "extra": "iterations: 138447\ncpu: 953.4110526049677 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 2.0803943169644903,
            "unit": "ns/iter",
            "extra": "iterations: 62187950\ncpu: 2.073818159305782 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 2.1401175423466827,
            "unit": "ns/iter",
            "extra": "iterations: 64173966\ncpu: 2.136003250913307 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 9.02476527364041,
            "unit": "ns/iter",
            "extra": "iterations: 16553707\ncpu: 9.01224722655777 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 1.8462228878727789,
            "unit": "ns/iter",
            "extra": "iterations: 75550711\ncpu: 1.8427635975523782 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.12480520725703943,
            "unit": "ms/iter",
            "extra": "iterations: 1051\ncpu: 0.1223418648905804 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.2742754785638106,
            "unit": "ms/iter",
            "extra": "iterations: 532\ncpu: 0.26298214285714283 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.12890917371468716,
            "unit": "ms/iter",
            "extra": "iterations: 1004\ncpu: 0.12661643426294825 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.25248124111323994,
            "unit": "ms/iter",
            "extra": "iterations: 591\ncpu: 0.24663502538071072 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.13191056397794945,
            "unit": "ms/iter",
            "extra": "iterations: 978\ncpu: 0.12950971370143144 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.24916807236654234,
            "unit": "ms/iter",
            "extra": "iterations: 554\ncpu: 0.2443380866425992 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 5.978238582611084,
            "unit": "ms/iter",
            "extra": "iterations: 20\ncpu: 5.960779999999999 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 25.475223859151203,
            "unit": "ms/iter",
            "extra": "iterations: 6\ncpu: 25.358733333333348 ms\nthreads: 1"
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
          "id": "b6a28df5c46810728c1eb17ca797391695628e64",
          "message": "Metrics SDK: Filtering metrics attributes (#1191)",
          "timestamp": "2022-02-04T18:46:06-08:00",
          "tree_id": "ddcd8770b365e437fd35a082e33825bf256b751f",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/b6a28df5c46810728c1eb17ca797391695628e64"
        },
        "date": 1644030046760,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 4575.7352352345115,
            "unit": "ns/iter",
            "extra": "iterations: 30569\ncpu: 4570.028460204782 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 2.772525497579765,
            "unit": "ns/iter",
            "extra": "iterations: 51107014\ncpu: 2.7547510406301576 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 95697.6781038343,
            "unit": "ns/iter",
            "extra": "iterations: 1428\ncpu: 91839.07563025213 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 2.717370612744186,
            "unit": "ns/iter",
            "extra": "iterations: 52174172\ncpu: 2.684776674558438 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 797.4833581281193,
            "unit": "ns/iter",
            "extra": "iterations: 184895\ncpu: 794.0360745287857 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 18936.214072242445,
            "unit": "ns/iter",
            "extra": "iterations: 7583\ncpu: 18893.30080443098 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 3289.6118573635517,
            "unit": "ns/iter",
            "extra": "iterations: 41726\ncpu: 3286.588697694484 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 60579.799160195864,
            "unit": "ns/iter",
            "extra": "iterations: 2342\ncpu: 60075.619128949664 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 66.59572354861722,
            "unit": "ns/iter",
            "extra": "iterations: 1914216\ncpu: 66.44203162025603 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 413.83696692227187,
            "unit": "ns/iter",
            "extra": "iterations: 350777\ncpu: 395.69156472630766 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1251.3893709299268,
            "unit": "ns/iter",
            "extra": "iterations: 111794\ncpu: 1247.575004025261 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 304.7212662230927,
            "unit": "ns/iter",
            "extra": "iterations: 441844\ncpu: 304.0276658730231 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 1242.2698723887231,
            "unit": "ns/iter",
            "extra": "iterations: 121597\ncpu: 1240.091449624579 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 2.8008025499325058,
            "unit": "ns/iter",
            "extra": "iterations: 47881418\ncpu: 2.7882340493758977 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 3.0655690263710857,
            "unit": "ns/iter",
            "extra": "iterations: 46757532\ncpu: 3.041169923168742 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 11.031276583970092,
            "unit": "ns/iter",
            "extra": "iterations: 12682768\ncpu: 11.017681629120709 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 2.3642816988798074,
            "unit": "ns/iter",
            "extra": "iterations: 59337371\ncpu: 2.3468498461113163 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.17892227971495078,
            "unit": "ms/iter",
            "extra": "iterations: 812\ncpu: 0.17352475369458126 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.34110524535718545,
            "unit": "ms/iter",
            "extra": "iterations: 442\ncpu: 0.3339576923076923 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.167197422172784,
            "unit": "ms/iter",
            "extra": "iterations: 867\ncpu: 0.16246078431372551 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.3211546917350925,
            "unit": "ms/iter",
            "extra": "iterations: 392\ncpu: 0.3151811224489798 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.1757673794131199,
            "unit": "ms/iter",
            "extra": "iterations: 834\ncpu: 0.1732622302158274 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.33209198399593953,
            "unit": "ms/iter",
            "extra": "iterations: 437\ncpu: 0.3233016018306634 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 7.75048467848036,
            "unit": "ms/iter",
            "extra": "iterations: 18\ncpu: 7.685961111111109 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 27.466440200805664,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 27.466999999999995 ms\nthreads: 1"
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
          "id": "44795b6d21738a8478d620c6f4cde6f12ea5ccac",
          "message": "Add Aggregation as part of metrics SDK. (#1178)",
          "timestamp": "2022-02-04T21:53:10-08:00",
          "tree_id": "4e8e0f917db86d65af01d12113128c0637c8e104",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/44795b6d21738a8478d620c6f4cde6f12ea5ccac"
        },
        "date": 1644041055994,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 3695.8751929811497,
            "unit": "ns/iter",
            "extra": "iterations: 37867\ncpu: 3690.366281986954 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.6209962082898073,
            "unit": "ns/iter",
            "extra": "iterations: 86626159\ncpu: 1.6168822630124928 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 81161.82742088335,
            "unit": "ns/iter",
            "extra": "iterations: 1876\ncpu: 75301.27931769722 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 3.240383938466136,
            "unit": "ns/iter",
            "extra": "iterations: 83756603\ncpu: 1.689638726155118 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 1851.2268069502627,
            "unit": "ns/iter",
            "extra": "iterations: 197118\ncpu: 755.4728639698051 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 17101.97491444982,
            "unit": "ns/iter",
            "extra": "iterations: 7859\ncpu: 16984.183738389114 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 2720.6507040608913,
            "unit": "ns/iter",
            "extra": "iterations: 51192\ncpu: 2719.4073292701996 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 46678.65543799547,
            "unit": "ns/iter",
            "extra": "iterations: 2988\ncpu: 46626.90763052206 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 53.14558318284267,
            "unit": "ns/iter",
            "extra": "iterations: 2644293\ncpu: 52.918606221020134 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 315.70388376722883,
            "unit": "ns/iter",
            "extra": "iterations: 443293\ncpu: 315.6846600329804 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 999.7443136236994,
            "unit": "ns/iter",
            "extra": "iterations: 140003\ncpu: 999.7357199488584 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 230.425433611312,
            "unit": "ns/iter",
            "extra": "iterations: 607057\ncpu: 230.4086766152109 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 946.0300716727521,
            "unit": "ns/iter",
            "extra": "iterations: 147962\ncpu: 945.743501709898 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 1.727372434427684,
            "unit": "ns/iter",
            "extra": "iterations: 81292316\ncpu: 1.718280482991775 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 1.7199886324516493,
            "unit": "ns/iter",
            "extra": "iterations: 81470662\ncpu: 1.7164436444618554 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 9.096976825161134,
            "unit": "ns/iter",
            "extra": "iterations: 15593118\ncpu: 8.96333882678243 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 1.7048537608654644,
            "unit": "ns/iter",
            "extra": "iterations: 81995057\ncpu: 1.7017135557330005 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.11017736514664923,
            "unit": "ms/iter",
            "extra": "iterations: 1244\ncpu: 0.10633987138263666 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.3407339967915123,
            "unit": "ms/iter",
            "extra": "iterations: 641\ncpu: 0.21260499219968798 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.1948637032658799,
            "unit": "ms/iter",
            "extra": "iterations: 1272\ncpu: 0.10824481132075474 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.22983402013778687,
            "unit": "ms/iter",
            "extra": "iterations: 640\ncpu: 0.21426343749999996 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.1104881010752629,
            "unit": "ms/iter",
            "extra": "iterations: 1286\ncpu: 0.10710847589424577 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.2912494623772452,
            "unit": "ms/iter",
            "extra": "iterations: 642\ncpu: 0.2133420560747664 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 6.40150633725253,
            "unit": "ms/iter",
            "extra": "iterations: 22\ncpu: 6.392636363636365 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 25.6960391998291,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 25.533939999999998 ms\nthreads: 1"
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
          "id": "e1c85570329bb775d5294a1eb2d3f1d2b9f7e4b0",
          "message": "fix errors in SDK documentation (#1201)",
          "timestamp": "2022-02-09T12:35:54-08:00",
          "tree_id": "6f0fa081aec644ec46538fc807881e46f6abc82d",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e1c85570329bb775d5294a1eb2d3f1d2b9f7e4b0"
        },
        "date": 1644439713211,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 4588.333479710169,
            "unit": "ns/iter",
            "extra": "iterations: 31405\ncpu: 4549.6449609934725 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.9478813299315976,
            "unit": "ns/iter",
            "extra": "iterations: 71763958\ncpu: 1.9448885469778576 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 118982.35548897745,
            "unit": "ns/iter",
            "extra": "iterations: 1557\ncpu: 91595.43994861915 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.9460726728386162,
            "unit": "ns/iter",
            "extra": "iterations: 73322614\ncpu: 1.9406918580398675 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 804.846012958373,
            "unit": "ns/iter",
            "extra": "iterations: 170384\ncpu: 801.2360315522586 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 24897.607032763608,
            "unit": "ns/iter",
            "extra": "iterations: 6839\ncpu: 20524.360286591615 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 3908.828155555918,
            "unit": "ns/iter",
            "extra": "iterations: 42187\ncpu: 3281.072368265102 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 69240.92190936931,
            "unit": "ns/iter",
            "extra": "iterations: 2472\ncpu: 56318.32524271841 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 64.0785724100856,
            "unit": "ns/iter",
            "extra": "iterations: 2128216\ncpu: 63.96864791919617 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 376.3878852848128,
            "unit": "ns/iter",
            "extra": "iterations: 366797\ncpu: 375.74543957556904 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1186.9486651981692,
            "unit": "ns/iter",
            "extra": "iterations: 117072\ncpu: 1185.2441232745664 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 274.3390619476565,
            "unit": "ns/iter",
            "extra": "iterations: 518653\ncpu: 272.3337183049169 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 1125.2365717960854,
            "unit": "ns/iter",
            "extra": "iterations: 122190\ncpu: 1123.5125624028155 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 2.5061694086085233,
            "unit": "ns/iter",
            "extra": "iterations: 70413680\ncpu: 2.0249559460604813 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 2.8799397234802933,
            "unit": "ns/iter",
            "extra": "iterations: 68574675\ncpu: 2.0519470197999485 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 14.095063620754992,
            "unit": "ns/iter",
            "extra": "iterations: 13041818\ncpu: 10.724049361829772 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 2.882850581621454,
            "unit": "ns/iter",
            "extra": "iterations: 68700529\ncpu: 2.019863631617742 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.4438584524102019,
            "unit": "ms/iter",
            "extra": "iterations: 471\ncpu: 0.1377588110403397 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.46155637480675954,
            "unit": "ms/iter",
            "extra": "iterations: 271\ncpu: 0.2550055350553506 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.45503040708868625,
            "unit": "ms/iter",
            "extra": "iterations: 502\ncpu: 0.14003545816733062 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.3252195275348166,
            "unit": "ms/iter",
            "extra": "iterations: 460\ncpu: 0.25425304347826105 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.5566352426749954,
            "unit": "ms/iter",
            "extra": "iterations: 466\ncpu: 0.13647467811158778 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.5106142589024135,
            "unit": "ms/iter",
            "extra": "iterations: 280\ncpu: 0.27831642857142863 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 26.73490047454834,
            "unit": "ms/iter",
            "extra": "iterations: 10\ncpu: 9.930309999999997 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 51.793416341145836,
            "unit": "ms/iter",
            "extra": "iterations: 3\ncpu: 34.49356666666666 ms\nthreads: 1"
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
          "id": "04e3a68263a90ac0fef822fc1f87196b9c59c545",
          "message": "Sync and Async Instruments SDK (#1184)",
          "timestamp": "2022-02-09T22:27:55-08:00",
          "tree_id": "790993870b48f156a444c412c9953d597451c9e8",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/04e3a68263a90ac0fef822fc1f87196b9c59c545"
        },
        "date": 1644475124504,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 4422.258192945055,
            "unit": "ns/iter",
            "extra": "iterations: 37571\ncpu: 3709.8826222352345 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.8548254035250067,
            "unit": "ns/iter",
            "extra": "iterations: 86661550\ncpu: 1.6161031045486725 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 104538.31900754289,
            "unit": "ns/iter",
            "extra": "iterations: 1869\ncpu: 74883.25307651151 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 2.1379976761681587,
            "unit": "ns/iter",
            "extra": "iterations: 86394155\ncpu: 1.6204556893924131 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 856.6055318908584,
            "unit": "ns/iter",
            "extra": "iterations: 210146\ncpu: 662.1553586554112 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 22119.044678179347,
            "unit": "ns/iter",
            "extra": "iterations: 8230\ncpu: 16985.151883353574 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 2755.2935602994053,
            "unit": "ns/iter",
            "extra": "iterations: 50965\ncpu: 2734.8042774453033 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 62538.00500378271,
            "unit": "ns/iter",
            "extra": "iterations: 2996\ncpu: 46457.51001335106 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 53.02364275919308,
            "unit": "ns/iter",
            "extra": "iterations: 2641714\ncpu: 52.96890579373847 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 316.46158494185755,
            "unit": "ns/iter",
            "extra": "iterations: 443037\ncpu: 315.8257662452571 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1010.3472777567838,
            "unit": "ns/iter",
            "extra": "iterations: 139838\ncpu: 1000.0829531314805 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 230.71218342031074,
            "unit": "ns/iter",
            "extra": "iterations: 607370\ncpu: 230.36682746925274 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 954.6512768236765,
            "unit": "ns/iter",
            "extra": "iterations: 148133\ncpu: 945.06355774878 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 1.6912439973909918,
            "unit": "ns/iter",
            "extra": "iterations: 83132924\ncpu: 1.686758906735916 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 1.7365025134577672,
            "unit": "ns/iter",
            "extra": "iterations: 82742317\ncpu: 1.6941983870236557 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 8.980247232374087,
            "unit": "ns/iter",
            "extra": "iterations: 15636483\ncpu: 8.95348397718336 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 1.6887545853616732,
            "unit": "ns/iter",
            "extra": "iterations: 83216929\ncpu: 1.6853746189071694 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.1100771010868133,
            "unit": "ms/iter",
            "extra": "iterations: 1260\ncpu: 0.10743396825396827 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.2160931526219789,
            "unit": "ms/iter",
            "extra": "iterations: 641\ncpu: 0.21036349453978156 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.11400987231542194,
            "unit": "ms/iter",
            "extra": "iterations: 1260\ncpu: 0.10798523809523809 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.21891902994226525,
            "unit": "ms/iter",
            "extra": "iterations: 648\ncpu: 0.21170864197530875 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.11049690575951497,
            "unit": "ms/iter",
            "extra": "iterations: 1274\ncpu: 0.10753908948194667 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.2172659208745132,
            "unit": "ms/iter",
            "extra": "iterations: 648\ncpu: 0.2113697530864199 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 6.437355821782893,
            "unit": "ms/iter",
            "extra": "iterations: 22\ncpu: 6.412990909090905 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 25.667095184326172,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 25.604419999999983 ms\nthreads: 1"
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
          "id": "e89e5b3dd65b0e73435c12b9fdeb54f9420f4bb4",
          "message": "Benchmark documentation (#1205)",
          "timestamp": "2022-02-11T21:23:30-08:00",
          "tree_id": "ec08c9f6ed7b2efa77f3ed004f3b8b7e0c50c6f7",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e89e5b3dd65b0e73435c12b9fdeb54f9420f4bb4"
        },
        "date": 1644644007216,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 3673.959647518051,
            "unit": "ns/iter",
            "extra": "iterations: 38130\ncpu: 3671.5106215578285 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.6229496404608104,
            "unit": "ns/iter",
            "extra": "iterations: 86794254\ncpu: 1.613107936845681 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 74401.5374084894,
            "unit": "ns/iter",
            "extra": "iterations: 1883\ncpu: 74305.78863515668 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.665315628007329,
            "unit": "ns/iter",
            "extra": "iterations: 86719524\ncpu: 1.615019242956176 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 660.822102136091,
            "unit": "ns/iter",
            "extra": "iterations: 211803\ncpu: 659.5307904042908 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 16946.239847584147,
            "unit": "ns/iter",
            "extra": "iterations: 8316\ncpu: 16903.451178451178 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 2736.2663290243195,
            "unit": "ns/iter",
            "extra": "iterations: 51102\ncpu: 2733.705138742123 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 46506.24105827843,
            "unit": "ns/iter",
            "extra": "iterations: 3001\ncpu: 46449.75008330554 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 52.869787123922656,
            "unit": "ns/iter",
            "extra": "iterations: 2645803\ncpu: 52.86958250481991 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 318.31064479863096,
            "unit": "ns/iter",
            "extra": "iterations: 440014\ncpu: 318.3094174276273 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1004.2282365505556,
            "unit": "ns/iter",
            "extra": "iterations: 139602\ncpu: 1003.9591123336348 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 230.4998467875993,
            "unit": "ns/iter",
            "extra": "iterations: 607312\ncpu: 230.49009405379766 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 949.4590830420523,
            "unit": "ns/iter",
            "extra": "iterations: 147455\ncpu: 949.3296259875892 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 2.1666976136763814,
            "unit": "ns/iter",
            "extra": "iterations: 83203081\ncpu: 1.7000824765130997 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 2.3042878045101403,
            "unit": "ns/iter",
            "extra": "iterations: 82286144\ncpu: 1.7071598348319732 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 11.937917596446171,
            "unit": "ns/iter",
            "extra": "iterations: 15484413\ncpu: 9.026257566237739 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 2.307846870107449,
            "unit": "ns/iter",
            "extra": "iterations: 80477808\ncpu: 1.7043083479609686 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.11048883343202154,
            "unit": "ms/iter",
            "extra": "iterations: 1246\ncpu: 0.11621115569823436 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.21247183607272205,
            "unit": "ms/iter",
            "extra": "iterations: 659\ncpu: 0.20717936267071319 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.10803685827277411,
            "unit": "ms/iter",
            "extra": "iterations: 1291\ncpu: 0.10555786212238578 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.21257574699785067,
            "unit": "ms/iter",
            "extra": "iterations: 657\ncpu: 0.20730213089802144 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.10758863218570325,
            "unit": "ms/iter",
            "extra": "iterations: 1299\ncpu: 0.10507490377213241 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.21195115049371083,
            "unit": "ms/iter",
            "extra": "iterations: 643\ncpu: 0.20664867807153953 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 6.510371253603981,
            "unit": "ms/iter",
            "extra": "iterations: 21\ncpu: 6.503933333333329 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 25.670480728149414,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 25.636139999999983 ms\nthreads: 1"
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
          "id": "9ed312b29ca0af1d92bbf0cf63ab8a6d88272bfb",
          "message": "fix ostream_log_test Mac (#1208)",
          "timestamp": "2022-02-14T13:05:25-08:00",
          "tree_id": "1c45c26c34a4c593b49c6a31c05a8d7e9bb60312",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/9ed312b29ca0af1d92bbf0cf63ab8a6d88272bfb"
        },
        "date": 1644873480680,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 3818.9046909258864,
            "unit": "ns/iter",
            "extra": "iterations: 36323\ncpu: 3806.5605814497703 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.6161564002370228,
            "unit": "ns/iter",
            "extra": "iterations: 86541552\ncpu: 1.6160629982693173 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 74747.53238348213,
            "unit": "ns/iter",
            "extra": "iterations: 1874\ncpu: 74740.12806830312 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.6192066793798052,
            "unit": "ns/iter",
            "extra": "iterations: 86628839\ncpu: 1.6156213290587913 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 672.7080901701763,
            "unit": "ns/iter",
            "extra": "iterations: 212095\ncpu: 672.6971404323538 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 16846.977362111837,
            "unit": "ns/iter",
            "extra": "iterations: 8330\ncpu: 16846.71068427371 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 2730.165118779945,
            "unit": "ns/iter",
            "extra": "iterations: 51292\ncpu: 2729.661545660143 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 46482.486591383604,
            "unit": "ns/iter",
            "extra": "iterations: 3001\ncpu: 46476.10796401204 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 53.122653060832896,
            "unit": "ns/iter",
            "extra": "iterations: 2640125\ncpu: 52.926736423464796 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 316.2368911244096,
            "unit": "ns/iter",
            "extra": "iterations: 442720\ncpu: 315.3964130827611 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1013.0478924636071,
            "unit": "ns/iter",
            "extra": "iterations: 139810\ncpu: 1002.0649452828837 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 231.6971281873619,
            "unit": "ns/iter",
            "extra": "iterations: 606772\ncpu: 230.46663326587256 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 973.6926581422563,
            "unit": "ns/iter",
            "extra": "iterations: 147827\ncpu: 947.4284129421553 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 1.696250487502862,
            "unit": "ns/iter",
            "extra": "iterations: 82712986\ncpu: 1.6936155587467245 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 1.6973107647724295,
            "unit": "ns/iter",
            "extra": "iterations: 82667076\ncpu: 1.6940770954569628 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 8.98036881746025,
            "unit": "ns/iter",
            "extra": "iterations: 15607041\ncpu: 8.972809131468292 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 1.6976284972751352,
            "unit": "ns/iter",
            "extra": "iterations: 82450912\ncpu: 1.6956440700134405 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.10982514182223549,
            "unit": "ms/iter",
            "extra": "iterations: 1264\ncpu: 0.10648291139240507 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.21670942948271224,
            "unit": "ms/iter",
            "extra": "iterations: 639\ncpu: 0.21141377151799692 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.10889801324582567,
            "unit": "ms/iter",
            "extra": "iterations: 1275\ncpu: 0.10674149019607847 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.2136573674667849,
            "unit": "ms/iter",
            "extra": "iterations: 653\ncpu: 0.20921822358346098 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.10836436678705672,
            "unit": "ms/iter",
            "extra": "iterations: 1298\ncpu: 0.10616040061633275 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.21316006278408411,
            "unit": "ms/iter",
            "extra": "iterations: 654\ncpu: 0.20857201834862382 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 6.4213384281505235,
            "unit": "ms/iter",
            "extra": "iterations: 22\ncpu: 6.419150000000005 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 25.72166919708252,
            "unit": "ms/iter",
            "extra": "iterations: 6\ncpu: 25.548250000000014 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "admin@owent.net",
            "name": "WenTao Ou",
            "username": "owent"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "15c6f33631d8cdf09b85911f35263449294d31a4",
          "message": "Update grpc to v1.43.2 to support VS2022/MSVC 19.30 and bazel 5.0 (#1207)\n\nSigned-off-by: owent <admin@owent.net>",
          "timestamp": "2022-02-14T20:24:58-08:00",
          "tree_id": "dff039d78807ee53fdc13748fd26ef7b48546fa8",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/15c6f33631d8cdf09b85911f35263449294d31a4"
        },
        "date": 1644899780717,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 3246.5005026282724,
            "unit": "ns/iter",
            "extra": "iterations: 43010\ncpu: 3244.8174843059755 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.4260311381384319,
            "unit": "ns/iter",
            "extra": "iterations: 98231827\ncpu: 1.4252081456247372 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 65579.63334631451,
            "unit": "ns/iter",
            "extra": "iterations: 2134\ncpu: 65575.72633552016 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.4455116887061863,
            "unit": "ns/iter",
            "extra": "iterations: 97924011\ncpu: 1.4252091859268314 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 595.5061206708248,
            "unit": "ns/iter",
            "extra": "iterations: 239598\ncpu: 594.6969507257988 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 14895.739717624048,
            "unit": "ns/iter",
            "extra": "iterations: 9433\ncpu: 14879.338492526243 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 2443.9722808222336,
            "unit": "ns/iter",
            "extra": "iterations: 57361\ncpu: 2418.3295270305607 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 43953.877023619345,
            "unit": "ns/iter",
            "extra": "iterations: 3394\ncpu: 41123.040659988226 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 49.08131738390329,
            "unit": "ns/iter",
            "extra": "iterations: 2958905\ncpu: 46.7223516807738 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 281.16913071073577,
            "unit": "ns/iter",
            "extra": "iterations: 503132\ncpu: 278.18624138397087 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 884.4189161137906,
            "unit": "ns/iter",
            "extra": "iterations: 158573\ncpu: 882.9031424012913 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 203.13241086364582,
            "unit": "ns/iter",
            "extra": "iterations: 689516\ncpu: 203.04140875628698 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 835.7440171087883,
            "unit": "ns/iter",
            "extra": "iterations: 167584\ncpu: 835.0600295970974 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 1.4864158536254166,
            "unit": "ns/iter",
            "extra": "iterations: 94448455\ncpu: 1.4847971838184122 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 1.528935118134695,
            "unit": "ns/iter",
            "extra": "iterations: 93852022\ncpu: 1.492876733119293 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 7.945889123704839,
            "unit": "ns/iter",
            "extra": "iterations: 17716944\ncpu: 7.897727734534806 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 1.5306459376074664,
            "unit": "ns/iter",
            "extra": "iterations: 89370639\ncpu: 1.5263737792005716 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.09657937896326133,
            "unit": "ms/iter",
            "extra": "iterations: 1441\ncpu: 0.09335433726578767 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.18999923844965233,
            "unit": "ms/iter",
            "extra": "iterations: 714\ncpu: 0.1849337535014006 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.09526518134091316,
            "unit": "ms/iter",
            "extra": "iterations: 1441\ncpu: 0.09327966689798753 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.18901568931221163,
            "unit": "ms/iter",
            "extra": "iterations: 745\ncpu: 0.18409140939597315 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.09509142822725794,
            "unit": "ms/iter",
            "extra": "iterations: 1459\ncpu: 0.0928088416723783 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.18736944403699649,
            "unit": "ms/iter",
            "extra": "iterations: 744\ncpu: 0.18263494623655915 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 5.783945322036743,
            "unit": "ms/iter",
            "extra": "iterations: 24\ncpu: 5.769720833333334 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 22.792259852091473,
            "unit": "ms/iter",
            "extra": "iterations: 6\ncpu: 22.740033333333333 ms\nthreads: 1"
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
          "id": "ef3a623a3bedd3fded167bade625d7fa392d2b88",
          "message": "Add Aggregation Storage (#1213)",
          "timestamp": "2022-02-16T14:58:32-08:00",
          "tree_id": "6f402b660d2b4da4a41caf6a91b28f6e4601ec2e",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/ef3a623a3bedd3fded167bade625d7fa392d2b88"
        },
        "date": 1645053075591,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 3722.914705242119,
            "unit": "ns/iter",
            "extra": "iterations: 37736\ncpu: 3715.873436506254 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 1.7501033618009587,
            "unit": "ns/iter",
            "extra": "iterations: 86707171\ncpu: 1.6188026708886625 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 74852.37121582031,
            "unit": "ns/iter",
            "extra": "iterations: 1875\ncpu: 74599.94666666671 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 1.6200372385031865,
            "unit": "ns/iter",
            "extra": "iterations: 85606491\ncpu: 1.616062034361389 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 660.9792829169438,
            "unit": "ns/iter",
            "extra": "iterations: 211797\ncpu: 659.9640221532882 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 17023.212801522044,
            "unit": "ns/iter",
            "extra": "iterations: 8309\ncpu: 16923.227825249716 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 2725.253006761378,
            "unit": "ns/iter",
            "extra": "iterations: 51317\ncpu: 2723.6977999493347 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 46578.30613068218,
            "unit": "ns/iter",
            "extra": "iterations: 2998\ncpu: 46535.42361574387 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 52.973786221823815,
            "unit": "ns/iter",
            "extra": "iterations: 2645753\ncpu: 52.86506336759328 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 324.6974537425533,
            "unit": "ns/iter",
            "extra": "iterations: 431635\ncpu: 324.4998667855944 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1009.0455245134852,
            "unit": "ns/iter",
            "extra": "iterations: 139035\ncpu: 1007.8944150753407 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 231.7284737820966,
            "unit": "ns/iter",
            "extra": "iterations: 605481\ncpu: 231.42922734156807 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 974.7251872392336,
            "unit": "ns/iter",
            "extra": "iterations: 147417\ncpu: 950.815034900995 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 1.686770026346726,
            "unit": "ns/iter",
            "extra": "iterations: 82894191\ncpu: 1.686026467162217 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 1.6925161127591941,
            "unit": "ns/iter",
            "extra": "iterations: 82781946\ncpu: 1.691197256947789 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 8.982894643646404,
            "unit": "ns/iter",
            "extra": "iterations: 15639784\ncpu: 8.952080156605742 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 1.6912830277733835,
            "unit": "ns/iter",
            "extra": "iterations: 81351835\ncpu: 1.6903232729784152 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.10965351059502763,
            "unit": "ms/iter",
            "extra": "iterations: 1265\ncpu: 0.10624395256916999 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.2211678135502446,
            "unit": "ms/iter",
            "extra": "iterations: 555\ncpu: 0.2109873873873874 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.10856539241313197,
            "unit": "ms/iter",
            "extra": "iterations: 1294\ncpu: 0.10637302936630604 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.21343282316946874,
            "unit": "ms/iter",
            "extra": "iterations: 653\ncpu: 0.20872205206738123 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.10943412780761719,
            "unit": "ms/iter",
            "extra": "iterations: 1303\ncpu: 0.10618127398311587 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.21214266769758616,
            "unit": "ms/iter",
            "extra": "iterations: 655\ncpu: 0.20767358778625952 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 6.418401544744318,
            "unit": "ms/iter",
            "extra": "iterations: 22\ncpu: 6.4079727272727265 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 25.64253807067871,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 25.546939999999996 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "Tom.Tan@microsoft.com",
            "name": "Tom Tan",
            "username": "ThomsonTan"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3a9d0870bc846f79f97c81796ce70440d944aa88",
          "message": "Detect vs2022 as build environment (#1212)",
          "timestamp": "2022-02-17T08:10:19Z",
          "tree_id": "361c175d162ec52518c7ad2e0eba088e6987fb3a",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/3a9d0870bc846f79f97c81796ce70440d944aa88"
        },
        "date": 1645086274798,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CreateBaggageFromTenEntries",
            "value": 6039.736089177541,
            "unit": "ns/iter",
            "extra": "iterations: 31592\ncpu: 4521.252215750823 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageHavingTenEntries",
            "value": 2.6549192148967684,
            "unit": "ns/iter",
            "extra": "iterations: 71013361\ncpu: 1.9730920777007024 ns\nthreads: 1"
          },
          {
            "name": "BM_CreateBaggageFrom180Entries",
            "value": 127365.426441503,
            "unit": "ns/iter",
            "extra": "iterations: 1524\ncpu: 92163.71391076113 ns\nthreads: 1"
          },
          {
            "name": "BM_ExtractBaggageWith180Entries",
            "value": 2.735883633620769,
            "unit": "ns/iter",
            "extra": "iterations: 71373585\ncpu: 1.9752825922923722 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWithTenEntries",
            "value": 1151.922619468661,
            "unit": "ns/iter",
            "extra": "iterations: 172008\ncpu: 835.0669736291331 ns\nthreads: 1"
          },
          {
            "name": "BM_SetValueBaggageWith180Entries",
            "value": 28057.951048987496,
            "unit": "ns/iter",
            "extra": "iterations: 6722\ncpu: 20816.944361797097 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeaderTenEntries",
            "value": 4327.903651630663,
            "unit": "ns/iter",
            "extra": "iterations: 41516\ncpu: 3351.539165622891 ns\nthreads: 1"
          },
          {
            "name": "BM_BaggageToHeader180Entries",
            "value": 75963.15159971415,
            "unit": "ns/iter",
            "extra": "iterations: 2470\ncpu: 56528.54251012153 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 63.789806584555485,
            "unit": "ns/iter",
            "extra": "iterations: 2203250\ncpu: 63.56723022807217 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithScope",
            "value": 392.5332221972951,
            "unit": "ns/iter",
            "extra": "iterations: 365066\ncpu: 383.34410764081014 ns\nthreads: 1"
          },
          {
            "name": "BM_NestedSpanCreationWithScope",
            "value": 1203.7096825593035,
            "unit": "ns/iter",
            "extra": "iterations: 116410\ncpu: 1201.6433296108582 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWithManualSpanContextPropagation",
            "value": 276.4428669478885,
            "unit": "ns/iter",
            "extra": "iterations: 506435\ncpu: 276.3357587844443 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreationWitContextPropagation",
            "value": 1138.769865934768,
            "unit": "ns/iter",
            "extra": "iterations: 123088\ncpu: 1137.0864747172752 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdDefaultConstructor",
            "value": 2.2429779999757726,
            "unit": "ns/iter",
            "extra": "iterations: 64929042\ncpu: 2.134008076077882 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdConstructor",
            "value": 2.3064913902312156,
            "unit": "ns/iter",
            "extra": "iterations: 65751469\ncpu: 2.1700838349330263 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdToLowerBase16",
            "value": 11.747964631356774,
            "unit": "ns/iter",
            "extra": "iterations: 12666015\ncpu: 11.732482552720805 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanIdIsValid",
            "value": 3.371320416045701,
            "unit": "ns/iter",
            "extra": "iterations: 60879359\ncpu: 2.3530602547901327 ns\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/1/process_time/real_time",
            "value": 0.13059090079286806,
            "unit": "ms/iter",
            "extra": "iterations: 1057\ncpu: 0.12739479659413436 ms\nthreads: 1"
          },
          {
            "name": "BM_SpinLockThrashing/2/process_time/real_time",
            "value": 0.2578348198860495,
            "unit": "ms/iter",
            "extra": "iterations: 537\ncpu: 0.25181452513966485 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/1/process_time/real_time",
            "value": 0.13048786212963687,
            "unit": "ms/iter",
            "extra": "iterations: 1076\ncpu: 0.12785566914498142 ms\nthreads: 1"
          },
          {
            "name": "BM_ProcYieldSpinLockThrashing/2/process_time/real_time",
            "value": 0.25676283644232556,
            "unit": "ms/iter",
            "extra": "iterations: 546\ncpu: 0.2511758241758242 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/1/process_time/real_time",
            "value": 0.13000261048823183,
            "unit": "ms/iter",
            "extra": "iterations: 1087\ncpu: 0.12731545538178474 ms\nthreads: 1"
          },
          {
            "name": "BM_NaiveSpinLockThrashing/2/process_time/real_time",
            "value": 0.25679999535236886,
            "unit": "ms/iter",
            "extra": "iterations: 545\ncpu: 0.2505449541284403 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/1/process_time/real_time",
            "value": 7.2679645136783,
            "unit": "ms/iter",
            "extra": "iterations: 19\ncpu: 7.250131578947365 ms\nthreads: 1"
          },
          {
            "name": "BM_ThreadYieldSpinLockThrashing/2/process_time/real_time",
            "value": 29.311513900756836,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 29.211279999999995 ms\nthreads: 1"
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
      },
      {
        "commit": {
          "author": {
            "email": "reyang@microsoft.com",
            "name": "Reiley Yang",
            "username": "reyang"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "43ad7b9fea54d6ff5400778e14e0b4b4ac41b7da",
          "message": "move non-active members to Emeritus (#1186)",
          "timestamp": "2022-01-31T17:54:10Z",
          "tree_id": "0daef00e22b53e70822c388c416989b402414274",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/43ad7b9fea54d6ff5400778e14e0b4b4ac41b7da"
        },
        "date": 1643652451269,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.6359144076477121,
            "unit": "ns/iter",
            "extra": "iterations: 86084978\ncpu: 1.6251813411626823 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.6270064328434573,
            "unit": "ns/iter",
            "extra": "iterations: 86186730\ncpu: 1.6248533852021074 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 85.27705272568552,
            "unit": "ns/iter",
            "extra": "iterations: 1652389\ncpu: 84.6884117480811 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 330.53871770062676,
            "unit": "ns/iter",
            "extra": "iterations: 424313\ncpu: 330.2887255398727 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 37.17184357599766,
            "unit": "ns/iter",
            "extra": "iterations: 3766955\ncpu: 37.15162511896214 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 37.38570697316035,
            "unit": "ns/iter",
            "extra": "iterations: 3767816\ncpu: 37.1569896194506 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 40.32775382624712,
            "unit": "ns/iter",
            "extra": "iterations: 3479367\ncpu: 40.21645316518785 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 20.422644573819174,
            "unit": "ns/iter",
            "extra": "iterations: 6855386\ncpu: 20.413102923744923 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 801.6797763524033,
            "unit": "ns/iter",
            "extra": "iterations: 175771\ncpu: 800.2059497869382 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 799.7252493341736,
            "unit": "ns/iter",
            "extra": "iterations: 174862\ncpu: 798.7778934245282 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 9839586.76946486,
            "unit": "ns/iter",
            "extra": "iterations: 8013\ncpu: 33031.17434169474 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 9455063.343048096,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 77646.99999999958 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 7703664.302825928,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 282834.0000000002 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 3725769.281387329,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 33440.29999999998 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 3838767.7669525146,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 73014.00000000003 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 4645850.737750426,
            "unit": "ns/iter",
            "extra": "iterations: 571\ncpu: 247614.53590192646 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.529437000981499,
            "unit": "ns/iter",
            "extra": "iterations: 55999552\ncpu: 2.50037714587431 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 7.477778471813116,
            "unit": "ns/iter",
            "extra": "iterations: 18732004\ncpu: 7.476343694993874 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "Tom.Tan@microsoft.com",
            "name": "Tom Tan",
            "username": "ThomsonTan"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "033b16fe19f2ec13f4b96a74b3b33a04e1bc83a2",
          "message": "Prepare for 1.2.0 release (#1188)",
          "timestamp": "2022-02-01T11:58:47-08:00",
          "tree_id": "0dac4978a6de140a9ef209e3b12c97501a37329d",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/033b16fe19f2ec13f4b96a74b3b33a04e1bc83a2"
        },
        "date": 1643746257487,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 2.516515904598685,
            "unit": "ns/iter",
            "extra": "iterations: 70405182\ncpu: 2.068657389451816 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.9850857637844692,
            "unit": "ns/iter",
            "extra": "iterations: 64697401\ncpu: 1.9758598958248725 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 105.28194992778494,
            "unit": "ns/iter",
            "extra": "iterations: 1341947\ncpu: 105.06987235710498 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 406.5154568802614,
            "unit": "ns/iter",
            "extra": "iterations: 342159\ncpu: 402.9167726115637 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 65.22820901677795,
            "unit": "ns/iter",
            "extra": "iterations: 2891929\ncpu: 47.25963189276087 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 65.05397798398317,
            "unit": "ns/iter",
            "extra": "iterations: 2484199\ncpu: 47.00078375363644 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 70.78087085076018,
            "unit": "ns/iter",
            "extra": "iterations: 2863349\ncpu: 49.851240627670634 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 35.70915767257885,
            "unit": "ns/iter",
            "extra": "iterations: 5582004\ncpu: 25.779218359571253 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 1417.2884651758807,
            "unit": "ns/iter",
            "extra": "iterations: 142081\ncpu: 998.6169860854018 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 1492.07540781234,
            "unit": "ns/iter",
            "extra": "iterations: 134220\ncpu: 1072.4236328416027 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 521171.80824279785,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 21135 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 19500207.901000977,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 92223.99999999999 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 13164856.433868408,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 407324 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 488634.07350936026,
            "unit": "ns/iter",
            "extra": "iterations: 6625\ncpu: 21591.094339622647 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 1352503.776550293,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 63996 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 2975355.0349274008,
            "unit": "ns/iter",
            "extra": "iterations: 471\ncpu: 299895.96602972405 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.9935038832867327,
            "unit": "ns/iter",
            "extra": "iterations: 46595221\ncpu: 2.988463130156631 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 8.981502902274213,
            "unit": "ns/iter",
            "extra": "iterations: 15714252\ncpu: 8.952516479944446 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "hamed0381@gmail.com",
            "name": "Hamed Mansouri",
            "username": "hamedprog"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a320739a46000dfedba7d7a8ba60119d2a6dc5f7",
          "message": "Update GettingStarted.rst (#1189)",
          "timestamp": "2022-02-02T13:58:45-08:00",
          "tree_id": "9a560ec05ec795317cff1ee6e6f328f6923fdf3d",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/a320739a46000dfedba7d7a8ba60119d2a6dc5f7"
        },
        "date": 1643839938066,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 3.130245047731095,
            "unit": "ns/iter",
            "extra": "iterations: 38578971\ncpu: 3.1203657557377573 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 3.4178307984969414,
            "unit": "ns/iter",
            "extra": "iterations: 43230958\ncpu: 2.82174871072716 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 103.3149628313267,
            "unit": "ns/iter",
            "extra": "iterations: 1361166\ncpu: 103.1622153359693 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 399.4536687533431,
            "unit": "ns/iter",
            "extra": "iterations: 351878\ncpu: 398.73592551964015 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 44.164811415094356,
            "unit": "ns/iter",
            "extra": "iterations: 3208853\ncpu: 43.980045206184265 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 82.3762840248214,
            "unit": "ns/iter",
            "extra": "iterations: 3100713\ncpu: 44.3574429494119 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 50.66509878914213,
            "unit": "ns/iter",
            "extra": "iterations: 2949853\ncpu: 47.44260137708555 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 24.217413476366154,
            "unit": "ns/iter",
            "extra": "iterations: 5841121\ncpu: 24.17155884974819 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 956.9030274963177,
            "unit": "ns/iter",
            "extra": "iterations: 143699\ncpu: 955.0149966248897 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 1132.7767372131348,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 1016.6960000000014 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 1155283.9279174805,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 32685.8 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 3523110.3897094727,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 79212.50000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 11245126.724243164,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 180422.99999999985 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 2985191.942370872,
            "unit": "ns/iter",
            "extra": "iterations: 5909\ncpu: 45568.945676087336 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 3106907.844543457,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 101881.99999999997 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 3925024.3331583757,
            "unit": "ns/iter",
            "extra": "iterations: 844\ncpu: 170783.88625592412 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 3.3969410173900467,
            "unit": "ns/iter",
            "extra": "iterations: 41002812\ncpu: 3.3876359504318874 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 8.993965271871128,
            "unit": "ns/iter",
            "extra": "iterations: 16528457\ncpu: 8.521587949800763 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "dupadhya@redhat.com",
            "name": "Deepika Upadhyay",
            "username": "ideepika"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e9127fc49dbe63b6f37ead0e158bdb0b78a6b984",
          "message": "cmake: thrift requires boost headers, include them as Boost_INCLUDE_DIRS (#1100)",
          "timestamp": "2022-02-04T10:43:33-08:00",
          "tree_id": "0e7d81535f83b70283f422d9e187c9f58e5cf975",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e9127fc49dbe63b6f37ead0e158bdb0b78a6b984"
        },
        "date": 1644000901790,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.625465031498291,
            "unit": "ns/iter",
            "extra": "iterations: 85837962\ncpu: 1.6248941231852638 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.6353190307622611,
            "unit": "ns/iter",
            "extra": "iterations: 86179302\ncpu: 1.6250270859701323 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 85.17103754015281,
            "unit": "ns/iter",
            "extra": "iterations: 1652797\ncpu: 84.67912272348026 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 359.4149475776143,
            "unit": "ns/iter",
            "extra": "iterations: 423251\ncpu: 330.3701586056499 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 39.61726986102017,
            "unit": "ns/iter",
            "extra": "iterations: 3768892\ncpu: 37.24659661247921 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 37.51144899111505,
            "unit": "ns/iter",
            "extra": "iterations: 3767421\ncpu: 37.23717099840973 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 40.25923811720531,
            "unit": "ns/iter",
            "extra": "iterations: 3481747\ncpu: 40.209354671663355 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 20.423985536764633,
            "unit": "ns/iter",
            "extra": "iterations: 6858543\ncpu: 20.41200295747944 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 795.4782769025833,
            "unit": "ns/iter",
            "extra": "iterations: 175551\ncpu: 794.953603226413 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 808.5005472123945,
            "unit": "ns/iter",
            "extra": "iterations: 175156\ncpu: 800.8044257690291 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 3.9867671714452615,
            "unit": "ns/iter",
            "extra": "iterations: 55965078\ncpu: 2.498570626489612 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 7.879289649599929,
            "unit": "ns/iter",
            "extra": "iterations: 18735338\ncpu: 7.475979349825451 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 7408616.64232007,
            "unit": "ns/iter",
            "extra": "iterations: 7931\ncpu: 28332.921447484554 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 9180319.30923462,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 76137.00000000029 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 7059586.048126221,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 248643.0000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 3891934.394836426,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 31636.300000000007 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 3809795.141220093,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 70364.29999999999 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 4820804.272667836,
            "unit": "ns/iter",
            "extra": "iterations: 590\ncpu: 241890.33898305095 ns\nthreads: 1"
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
          "id": "e1b4a49f9b673cbdce4b992ae8fc712e49c39c50",
          "message": "disable failing test (#1193)",
          "timestamp": "2022-02-04T11:52:45-08:00",
          "tree_id": "ed584794fbdf5a6eb267184582f6fa13e64001f2",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e1b4a49f9b673cbdce4b992ae8fc712e49c39c50"
        },
        "date": 1644005085943,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.9900228029322358,
            "unit": "ns/iter",
            "extra": "iterations: 63436826\ncpu: 1.986418740433199 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 2.087778949843262,
            "unit": "ns/iter",
            "extra": "iterations: 60774968\ncpu: 2.0822997389319893 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 79.46914569801581,
            "unit": "ns/iter",
            "extra": "iterations: 1754650\ncpu: 79.1311087681304 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 347.8628569010873,
            "unit": "ns/iter",
            "extra": "iterations: 420815\ncpu: 344.3380107648254 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 36.702772221307214,
            "unit": "ns/iter",
            "extra": "iterations: 3595256\ncpu: 36.53350415102567 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 36.851196873930675,
            "unit": "ns/iter",
            "extra": "iterations: 3878954\ncpu: 36.73619228276487 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 38.92596425807225,
            "unit": "ns/iter",
            "extra": "iterations: 3573881\ncpu: 38.821214248599816 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 20.528213487264907,
            "unit": "ns/iter",
            "extra": "iterations: 7218951\ncpu: 20.477490427625813 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 847.0654916206197,
            "unit": "ns/iter",
            "extra": "iterations: 178287\ncpu: 845.2983111499988 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 1001.635183579156,
            "unit": "ns/iter",
            "extra": "iterations: 177727\ncpu: 796.1581526723566 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.8395757387660905,
            "unit": "ns/iter",
            "extra": "iterations: 52537170\ncpu: 2.834745381222476 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 6.865978209983595,
            "unit": "ns/iter",
            "extra": "iterations: 19899507\ncpu: 6.854350713311643 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 564463.8753465419,
            "unit": "ns/iter",
            "extra": "iterations: 7404\ncpu: 21768.287412209622 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 3120694.875717163,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 62020.09999999999 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 11840434.074401855,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 258744.00000000017 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 453277.5417680796,
            "unit": "ns/iter",
            "extra": "iterations: 7361\ncpu: 21529.30308382013 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 3414003.1337738037,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 90303.19999999997 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 4380549.834324763,
            "unit": "ns/iter",
            "extra": "iterations: 520\ncpu: 269226.153846154 ns\nthreads: 1"
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
          "id": "b6a28df5c46810728c1eb17ca797391695628e64",
          "message": "Metrics SDK: Filtering metrics attributes (#1191)",
          "timestamp": "2022-02-04T18:46:06-08:00",
          "tree_id": "ddcd8770b365e437fd35a082e33825bf256b751f",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/b6a28df5c46810728c1eb17ca797391695628e64"
        },
        "date": 1644030044984,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 271.2943574855041,
            "unit": "ns/iter",
            "extra": "iterations: 504848\ncpu: 270.96353754001206 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 2.833132377414446,
            "unit": "ns/iter",
            "extra": "iterations: 50003036\ncpu: 2.831384078358762 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 2.8918825760525015,
            "unit": "ns/iter",
            "extra": "iterations: 48821484\ncpu: 2.868507643069597 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 107.31084901259976,
            "unit": "ns/iter",
            "extra": "iterations: 1377709\ncpu: 106.96569449716885 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 422.2625367339573,
            "unit": "ns/iter",
            "extra": "iterations: 334531\ncpu: 422.06342610998706 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 48.11647818196079,
            "unit": "ns/iter",
            "extra": "iterations: 2892454\ncpu: 48.088232345268075 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 48.32689870549836,
            "unit": "ns/iter",
            "extra": "iterations: 2871330\ncpu: 47.40280636499461 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 52.396270789617475,
            "unit": "ns/iter",
            "extra": "iterations: 2843171\ncpu: 50.1154520779791 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 32.89342509033455,
            "unit": "ns/iter",
            "extra": "iterations: 5620323\ncpu: 26.06277254883749 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 1027.0822274457014,
            "unit": "ns/iter",
            "extra": "iterations: 117592\ncpu: 1026.1743996190198 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 1094.716530630298,
            "unit": "ns/iter",
            "extra": "iterations: 133253\ncpu: 1092.0699721582264 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 204.89671113664602,
            "unit": "ns/iter",
            "extra": "iterations: 726937\ncpu: 204.6349270982217 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 3.874898247910803,
            "unit": "ns/iter",
            "extra": "iterations: 38898505\ncpu: 3.845178625759525 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 9.710196666586585,
            "unit": "ns/iter",
            "extra": "iterations: 15496771\ncpu: 9.672421435407415 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 11502797.603607178,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 63187.99999999999 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 7753102.779388428,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 125703 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 6988024.711608887,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 424817.00000000006 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 3398835.8974456787,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 50350.20000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 3850839.0667430586,
            "unit": "ns/iter",
            "extra": "iterations: 1204\ncpu: 114784.55149501661 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 4918647.741342519,
            "unit": "ns/iter",
            "extra": "iterations: 385\ncpu: 370156.103896104 ns\nthreads: 1"
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
          "id": "44795b6d21738a8478d620c6f4cde6f12ea5ccac",
          "message": "Add Aggregation as part of metrics SDK. (#1178)",
          "timestamp": "2022-02-04T21:53:10-08:00",
          "tree_id": "4e8e0f917db86d65af01d12113128c0637c8e104",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/44795b6d21738a8478d620c6f4cde6f12ea5ccac"
        },
        "date": 1644041052607,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 205.26817353958708,
            "unit": "ns/iter",
            "extra": "iterations: 683487\ncpu: 204.35385018295887 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.6627469319658674,
            "unit": "ns/iter",
            "extra": "iterations: 84395816\ncpu: 1.655948204825699 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.6625013769459698,
            "unit": "ns/iter",
            "extra": "iterations: 84524730\ncpu: 1.6559059106133787 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 85.1441049936839,
            "unit": "ns/iter",
            "extra": "iterations: 1653191\ncpu: 84.70412674639535 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 331.95546455351916,
            "unit": "ns/iter",
            "extra": "iterations: 422956\ncpu: 330.48945989653777 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 37.294135277569126,
            "unit": "ns/iter",
            "extra": "iterations: 3767005\ncpu: 37.17098862358823 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 37.301051291643496,
            "unit": "ns/iter",
            "extra": "iterations: 3766428\ncpu: 37.167337328630715 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 40.38166371520005,
            "unit": "ns/iter",
            "extra": "iterations: 3478849\ncpu: 40.2357216424168 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 20.471614178681996,
            "unit": "ns/iter",
            "extra": "iterations: 6852567\ncpu: 20.42371858604228 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 798.9286773164364,
            "unit": "ns/iter",
            "extra": "iterations: 174099\ncpu: 796.4118116703721 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 807.054375328072,
            "unit": "ns/iter",
            "extra": "iterations: 174552\ncpu: 804.4181676520463 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 228.51877558966873,
            "unit": "ns/iter",
            "extra": "iterations: 847053\ncpu: 167.5873882744055 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 4.372662910872636,
            "unit": "ns/iter",
            "extra": "iterations: 55687925\ncpu: 2.5012729420246855 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 8.691382583165314,
            "unit": "ns/iter",
            "extra": "iterations: 18713375\ncpu: 7.432715905067901 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 555124.9980926514,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 19552.700000000004 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 5202386.856079102,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 77053.40000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 6168870.92590332,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 355034.00000000006 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 3587420.2251434326,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 43065.3 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 3782730.3409576416,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 83058.79999999999 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 4912969.340448794,
            "unit": "ns/iter",
            "extra": "iterations: 460\ncpu: 317002.1739130435 ns\nthreads: 1"
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
          "id": "e1c85570329bb775d5294a1eb2d3f1d2b9f7e4b0",
          "message": "fix errors in SDK documentation (#1201)",
          "timestamp": "2022-02-09T12:35:54-08:00",
          "tree_id": "6f0fa081aec644ec46538fc807881e46f6abc82d",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e1c85570329bb775d5294a1eb2d3f1d2b9f7e4b0"
        },
        "date": 1644439716057,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 247.6713440837077,
            "unit": "ns/iter",
            "extra": "iterations: 574868\ncpu: 247.03984218985923 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.9374729433129783,
            "unit": "ns/iter",
            "extra": "iterations: 74674234\ncpu: 1.9320720450912159 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.9845365481752326,
            "unit": "ns/iter",
            "extra": "iterations: 73083770\ncpu: 1.952521059053193 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 100.23730487407694,
            "unit": "ns/iter",
            "extra": "iterations: 1387691\ncpu: 100.00648559369483 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 396.7458422976379,
            "unit": "ns/iter",
            "extra": "iterations: 353252\ncpu: 396.26895247585287 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 45.16966622565387,
            "unit": "ns/iter",
            "extra": "iterations: 2970690\ncpu: 45.07525187750991 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 46.26860434629529,
            "unit": "ns/iter",
            "extra": "iterations: 2988809\ncpu: 46.20984479101873 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 49.84166288948918,
            "unit": "ns/iter",
            "extra": "iterations: 2766099\ncpu: 49.69796814936852 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 25.090401229401007,
            "unit": "ns/iter",
            "extra": "iterations: 5742318\ncpu: 24.96606770993877 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 956.7866767650876,
            "unit": "ns/iter",
            "extra": "iterations: 145777\ncpu: 956.5487010982534 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 969.0924955458609,
            "unit": "ns/iter",
            "extra": "iterations: 138429\ncpu: 964.6194077830513 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 234.0549599833093,
            "unit": "ns/iter",
            "extra": "iterations: 698599\ncpu: 205.7077092867296 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.989197853753454,
            "unit": "ns/iter",
            "extra": "iterations: 47959002\ncpu: 2.982265977928398 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 8.908281815490232,
            "unit": "ns/iter",
            "extra": "iterations: 15926248\ncpu: 8.837875687983761 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 573135.8528137207,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 22681.600000000002 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 5519599.914550781,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 76501 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 3843520.1387147647,
            "unit": "ns/iter",
            "extra": "iterations: 444\ncpu: 309956.75675675675 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 378364.1727168177,
            "unit": "ns/iter",
            "extra": "iterations: 6241\ncpu: 22237.926614324628 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 1475775.957107544,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 61536.099999999984 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 7166068.553924561,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 322308.00000000006 ns\nthreads: 1"
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
          "id": "04e3a68263a90ac0fef822fc1f87196b9c59c545",
          "message": "Sync and Async Instruments SDK (#1184)",
          "timestamp": "2022-02-09T22:27:55-08:00",
          "tree_id": "790993870b48f156a444c412c9953d597451c9e8",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/04e3a68263a90ac0fef822fc1f87196b9c59c545"
        },
        "date": 1644475125908,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 204.42978584787195,
            "unit": "ns/iter",
            "extra": "iterations: 684038\ncpu: 204.41218177937483 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.6316212263975431,
            "unit": "ns/iter",
            "extra": "iterations: 86030492\ncpu: 1.6251191496149993 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.6417757226613126,
            "unit": "ns/iter",
            "extra": "iterations: 86186730\ncpu: 1.6255669521282452 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 85.84352232400097,
            "unit": "ns/iter",
            "extra": "iterations: 1652639\ncpu: 84.75958754452724 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 346.9189420513982,
            "unit": "ns/iter",
            "extra": "iterations: 423920\ncpu: 331.3766748443102 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 37.703775280467624,
            "unit": "ns/iter",
            "extra": "iterations: 3743366\ncpu: 37.23117109040368 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 37.59736298415143,
            "unit": "ns/iter",
            "extra": "iterations: 3759833\ncpu: 37.228568396521844 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 40.61177030086749,
            "unit": "ns/iter",
            "extra": "iterations: 3469752\ncpu: 40.270197985331535 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 20.630319632150094,
            "unit": "ns/iter",
            "extra": "iterations: 6843790\ncpu: 20.457962620127148 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 832.6760762044165,
            "unit": "ns/iter",
            "extra": "iterations: 175587\ncpu: 797.6057453000506 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 1024.6520052066166,
            "unit": "ns/iter",
            "extra": "iterations: 173499\ncpu: 812.0709629450328 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 183.30890827948477,
            "unit": "ns/iter",
            "extra": "iterations: 849149\ncpu: 166.18426212596376 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.503680287023096,
            "unit": "ns/iter",
            "extra": "iterations: 55970671\ncpu: 2.500736501801095 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 7.521851658276658,
            "unit": "ns/iter",
            "extra": "iterations: 18692138\ncpu: 7.484745725716343 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 8119761.943817139,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 50085.00000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 2248693.7046051025,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 51242.00000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 10742192.268371582,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 338378 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 334205.93099821784,
            "unit": "ns/iter",
            "extra": "iterations: 7540\ncpu: 19084.827586206902 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 1255850.076675415,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 49371.1 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 8280262.9470825195,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 315998.0000000001 ns\nthreads: 1"
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
          "id": "7485102bfaa68f8a5d4d70312c60cb928e70f00c",
          "message": "Change Monday's community meeting time (13:00 PT - 14:00 PT) (#1204)",
          "timestamp": "2022-02-11T09:47:47-08:00",
          "tree_id": "5402daaa08f470822751ac029072af80d68fefaa",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/7485102bfaa68f8a5d4d70312c60cb928e70f00c"
        },
        "date": 1644602496354,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 205.2670053753319,
            "unit": "ns/iter",
            "extra": "iterations: 681816\ncpu: 204.18412005585085 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.6274105558045027,
            "unit": "ns/iter",
            "extra": "iterations: 85931218\ncpu: 1.6246400696892256 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.6269111423499971,
            "unit": "ns/iter",
            "extra": "iterations: 86193097\ncpu: 1.6247391597960565 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 85.63894793175457,
            "unit": "ns/iter",
            "extra": "iterations: 1650398\ncpu: 84.8841915707605 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 331.2450836120128,
            "unit": "ns/iter",
            "extra": "iterations: 423686\ncpu: 330.61323716148274 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 37.211110523598336,
            "unit": "ns/iter",
            "extra": "iterations: 3761974\ncpu: 37.17029942259039 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 41.80952233605203,
            "unit": "ns/iter",
            "extra": "iterations: 3763886\ncpu: 37.20997394713869 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 40.24403918381166,
            "unit": "ns/iter",
            "extra": "iterations: 3478909\ncpu: 40.221230276503356 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 20.47404857192645,
            "unit": "ns/iter",
            "extra": "iterations: 6854547\ncpu: 20.432626692909096 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 808.5633228070968,
            "unit": "ns/iter",
            "extra": "iterations: 173040\ncpu: 806.8307905686542 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 813.6016238570988,
            "unit": "ns/iter",
            "extra": "iterations: 172711\ncpu: 813.1207624297226 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 167.2053881606971,
            "unit": "ns/iter",
            "extra": "iterations: 841609\ncpu: 166.31642484811834 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.5180667516272637,
            "unit": "ns/iter",
            "extra": "iterations: 56045060\ncpu: 2.4982719261965287 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 7.52444991146353,
            "unit": "ns/iter",
            "extra": "iterations: 18747154\ncpu: 7.468082888741406 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 16589915.752410889,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 49916.00000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 8381655.216217041,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 94284.00000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 5308029.651641846,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 352731 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 4046501.398086548,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 47719.6 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 4119355.2017211914,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 87039.89999999998 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 5436952.114105225,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 343661.9999999998 ns\nthreads: 1"
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
          "id": "e89e5b3dd65b0e73435c12b9fdeb54f9420f4bb4",
          "message": "Benchmark documentation (#1205)",
          "timestamp": "2022-02-11T21:23:30-08:00",
          "tree_id": "ec08c9f6ed7b2efa77f3ed004f3b8b7e0c50c6f7",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e89e5b3dd65b0e73435c12b9fdeb54f9420f4bb4"
        },
        "date": 1644644004542,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 175.85029539785643,
            "unit": "ns/iter",
            "extra": "iterations: 792774\ncpu: 175.12090457053338 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.8309369963587387,
            "unit": "ns/iter",
            "extra": "iterations: 84954549\ncpu: 1.6293618367628553 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.6279813434828199,
            "unit": "ns/iter",
            "extra": "iterations: 86204242\ncpu: 1.6249977582309694 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 84.73047034372098,
            "unit": "ns/iter",
            "extra": "iterations: 1652910\ncpu: 84.66631577036863 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 331.85389706930073,
            "unit": "ns/iter",
            "extra": "iterations: 424291\ncpu: 329.98248843364587 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 37.384962655203566,
            "unit": "ns/iter",
            "extra": "iterations: 3768516\ncpu: 37.15247593482421 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 37.2310611956182,
            "unit": "ns/iter",
            "extra": "iterations: 3768648\ncpu: 37.16537071119402 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 40.36338879527397,
            "unit": "ns/iter",
            "extra": "iterations: 3480873\ncpu: 40.220197634329104 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 20.429271715320514,
            "unit": "ns/iter",
            "extra": "iterations: 6855788\ncpu: 20.413729246003548 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 815.9365870292348,
            "unit": "ns/iter",
            "extra": "iterations: 171533\ncpu: 814.8263016445811 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 815.5766816469103,
            "unit": "ns/iter",
            "extra": "iterations: 171376\ncpu: 815.0149379142929 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 145.80035022369105,
            "unit": "ns/iter",
            "extra": "iterations: 974747\ncpu: 145.43794441019054 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.6660201103812406,
            "unit": "ns/iter",
            "extra": "iterations: 55998432\ncpu: 2.4980645886656254 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 9.628434144643238,
            "unit": "ns/iter",
            "extra": "iterations: 18723186\ncpu: 6.635692237421559 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 439155.6957810903,
            "unit": "ns/iter",
            "extra": "iterations: 9051\ncpu: 16796.309800022096 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 9861683.84552002,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 170528.0000000001 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 10827100.276947021,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 362425.9999999998 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 265987.30270599056,
            "unit": "ns/iter",
            "extra": "iterations: 9156\ncpu: 15360.615989515069 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 2492783.2325052773,
            "unit": "ns/iter",
            "extra": "iterations: 805\ncpu: 163755.15527950306 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 8110666.275024414,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 349873.0000000005 ns\nthreads: 1"
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
          "id": "9ed312b29ca0af1d92bbf0cf63ab8a6d88272bfb",
          "message": "fix ostream_log_test Mac (#1208)",
          "timestamp": "2022-02-14T13:05:25-08:00",
          "tree_id": "1c45c26c34a4c593b49c6a31c05a8d7e9bb60312",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/9ed312b29ca0af1d92bbf0cf63ab8a6d88272bfb"
        },
        "date": 1644873378604,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 204.58212665357377,
            "unit": "ns/iter",
            "extra": "iterations: 682890\ncpu: 204.53923765174483 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.6814293131031235,
            "unit": "ns/iter",
            "extra": "iterations: 86016220\ncpu: 1.626696685811118 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.6523710302785508,
            "unit": "ns/iter",
            "extra": "iterations: 86136354\ncpu: 1.6262831370828628 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 84.81894670233777,
            "unit": "ns/iter",
            "extra": "iterations: 1642011\ncpu: 84.67421960023414 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 330.8613899701691,
            "unit": "ns/iter",
            "extra": "iterations: 423095\ncpu: 330.5640577175341 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 38.934452400434616,
            "unit": "ns/iter",
            "extra": "iterations: 3594167\ncpu: 38.84226859798112 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 38.939804273718906,
            "unit": "ns/iter",
            "extra": "iterations: 3604596\ncpu: 38.83422719217357 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 42.27265247043529,
            "unit": "ns/iter",
            "extra": "iterations: 3341249\ncpu: 41.903940711991225 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 25.097569572420593,
            "unit": "ns/iter",
            "extra": "iterations: 6309689\ncpu: 22.163105027838927 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 825.7535601571891,
            "unit": "ns/iter",
            "extra": "iterations: 172407\ncpu: 811.337706705644 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 822.1882068135483,
            "unit": "ns/iter",
            "extra": "iterations: 171801\ncpu: 813.5849034638903 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 165.53541288758169,
            "unit": "ns/iter",
            "extra": "iterations: 838544\ncpu: 165.47754202522466 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.5001786845938407,
            "unit": "ns/iter",
            "extra": "iterations: 55811803\ncpu: 2.4997113961718815 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 7.981268933736777,
            "unit": "ns/iter",
            "extra": "iterations: 18693436\ncpu: 7.47567220921825 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 570165.9033868861,
            "unit": "ns/iter",
            "extra": "iterations: 7637\ncpu: 19186.565405263846 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 6860911.60774231,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 62501.59999999999 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 12148878.574371338,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 380836.00000000023 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 465301.2148246238,
            "unit": "ns/iter",
            "extra": "iterations: 7595\ncpu: 19424.20013166557 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 1395286.7984771729,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 52434.399999999994 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 7750217.914581299,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 354613.0000000003 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "admin@owent.net",
            "name": "WenTao Ou",
            "username": "owent"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "15c6f33631d8cdf09b85911f35263449294d31a4",
          "message": "Update grpc to v1.43.2 to support VS2022/MSVC 19.30 and bazel 5.0 (#1207)\n\nSigned-off-by: owent <admin@owent.net>",
          "timestamp": "2022-02-14T20:24:58-08:00",
          "tree_id": "dff039d78807ee53fdc13748fd26ef7b48546fa8",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/15c6f33631d8cdf09b85911f35263449294d31a4"
        },
        "date": 1644899780582,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 204.36883825624804,
            "unit": "ns/iter",
            "extra": "iterations: 684165\ncpu: 204.10529623701885 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.4369875099171987,
            "unit": "ns/iter",
            "extra": "iterations: 97589538\ncpu: 1.433105462595796 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.4369247177243005,
            "unit": "ns/iter",
            "extra": "iterations: 97704639\ncpu: 1.4328736223056924 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 82.36161500628243,
            "unit": "ns/iter",
            "extra": "iterations: 1872351\ncpu: 74.7219404908588 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 293.37292024112594,
            "unit": "ns/iter",
            "extra": "iterations: 480036\ncpu: 291.6293777966653 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 33.47403193517049,
            "unit": "ns/iter",
            "extra": "iterations: 4271509\ncpu: 32.782934555446346 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 32.85736910722298,
            "unit": "ns/iter",
            "extra": "iterations: 4271822\ncpu: 32.76798518290321 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 35.57613664577301,
            "unit": "ns/iter",
            "extra": "iterations: 3945941\ncpu: 35.47551775355994 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 18.08438466260349,
            "unit": "ns/iter",
            "extra": "iterations: 7774279\ncpu: 18.008924042988404 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 702.9742995171016,
            "unit": "ns/iter",
            "extra": "iterations: 199526\ncpu: 701.6459007848608 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 734.443919365987,
            "unit": "ns/iter",
            "extra": "iterations: 198059\ncpu: 708.4691935231416 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 147.69524269918887,
            "unit": "ns/iter",
            "extra": "iterations: 950441\ncpu: 147.0108086667137 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.203272199728561,
            "unit": "ns/iter",
            "extra": "iterations: 63467885\ncpu: 2.2025611850780913 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 6.600791832637828,
            "unit": "ns/iter",
            "extra": "iterations: 21220609\ncpu: 6.594862569684027 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 422531.06745382573,
            "unit": "ns/iter",
            "extra": "iterations: 8894\ncpu: 15682.167753541713 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 5084738.731384277,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 48423.99999999996 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 10798838.138580322,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 115038.00000000009 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 265128.42802303017,
            "unit": "ns/iter",
            "extra": "iterations: 9033\ncpu: 15314.17026458541 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 965532.0644378662,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 42343.99999999999 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 7995738.983154297,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 109754.00000000025 ns\nthreads: 1"
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
          "id": "ef3a623a3bedd3fded167bade625d7fa392d2b88",
          "message": "Add Aggregation Storage (#1213)",
          "timestamp": "2022-02-16T14:58:32-08:00",
          "tree_id": "6f402b660d2b4da4a41caf6a91b28f6e4601ec2e",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/ef3a623a3bedd3fded167bade625d7fa392d2b88"
        },
        "date": 1645053074592,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_AttributseHashMap",
            "value": 15899896.621704102,
            "unit": "ns/iter",
            "extra": "iterations: 12\ncpu: 10692766.666666668 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributseProcessorFilter",
            "value": 204.47389051441127,
            "unit": "ns/iter",
            "extra": "iterations: 684105\ncpu: 203.91051081339853 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerConstruction",
            "value": 1.6342723736875475,
            "unit": "ns/iter",
            "extra": "iterations: 85975546\ncpu: 1.624493318134903 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerConstruction",
            "value": 1.6302856888321018,
            "unit": "ns/iter",
            "extra": "iterations: 86197343\ncpu: 1.6243853363322351 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerConstruction",
            "value": 84.85934066120608,
            "unit": "ns/iter",
            "extra": "iterations: 1654381\ncpu: 84.6375774383289 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerConstruction",
            "value": 330.7760594843413,
            "unit": "ns/iter",
            "extra": "iterations: 423584\ncpu: 330.20817594621144 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOffSamplerShouldSample",
            "value": 37.257423606962675,
            "unit": "ns/iter",
            "extra": "iterations: 3769277\ncpu: 37.14500154804223 ns\nthreads: 1"
          },
          {
            "name": "BM_AlwaysOnSamplerShouldSample",
            "value": 38.406228882781626,
            "unit": "ns/iter",
            "extra": "iterations: 3770059\ncpu: 37.17941814703695 ns\nthreads: 1"
          },
          {
            "name": "BM_ParentBasedSamplerShouldSample",
            "value": 40.401911302152975,
            "unit": "ns/iter",
            "extra": "iterations: 3479826\ncpu: 40.21353366518898 ns\nthreads: 1"
          },
          {
            "name": "BM_TraceIdRatioBasedSamplerShouldSample",
            "value": 20.435372655910005,
            "unit": "ns/iter",
            "extra": "iterations: 6858408\ncpu: 20.411967325361825 ns\nthreads: 1"
          },
          {
            "name": "BM_SpanCreation",
            "value": 802.5454589342921,
            "unit": "ns/iter",
            "extra": "iterations: 175978\ncpu: 795.9869983747967 ns\nthreads: 1"
          },
          {
            "name": "BM_NoopSpanCreation",
            "value": 841.0482633048797,
            "unit": "ns/iter",
            "extra": "iterations: 173692\ncpu: 799.9337908481679 ns\nthreads: 1"
          },
          {
            "name": "BM_AttributeMapHash",
            "value": 166.9500736631387,
            "unit": "ns/iter",
            "extra": "iterations: 838062\ncpu: 166.68229796840802 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdGeneration",
            "value": 2.504161699716387,
            "unit": "ns/iter",
            "extra": "iterations: 55901167\ncpu: 2.500335994774492 ns\nthreads: 1"
          },
          {
            "name": "BM_RandomIdStdGeneration",
            "value": 7.4922111005573875,
            "unit": "ns/iter",
            "extra": "iterations: 18702825\ncpu: 7.47873115425076 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/1",
            "value": 6299863.070445504,
            "unit": "ns/iter",
            "extra": "iterations: 8061\ncpu: 29577.11202084109 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/2",
            "value": 6976010.799407959,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 199431.0000000005 ns\nthreads: 1"
          },
          {
            "name": "BM_BaselineBuffer/4",
            "value": 7517263.889312744,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 367567.99999999977 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/1",
            "value": 3923492.670059204,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 35213.1 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/2",
            "value": 4126917.3934346153,
            "unit": "ns/iter",
            "extra": "iterations: 672\ncpu: 201706.994047619 ns\nthreads: 1"
          },
          {
            "name": "BM_LockFreeBuffer/4",
            "value": 5211760.997772217,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 362396.9999999998 ns\nthreads: 1"
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
      },
      {
        "commit": {
          "author": {
            "email": "hamed0381@gmail.com",
            "name": "Hamed Mansouri",
            "username": "hamedprog"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a320739a46000dfedba7d7a8ba60119d2a6dc5f7",
          "message": "Update GettingStarted.rst (#1189)",
          "timestamp": "2022-02-02T13:58:45-08:00",
          "tree_id": "9a560ec05ec795317cff1ee6e6f328f6923fdf3d",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/a320739a46000dfedba7d7a8ba60119d2a6dc5f7"
        },
        "date": 1643839938193,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 37.750435537231866,
            "unit": "ns/iter",
            "extra": "iterations: 3747000\ncpu: 37.71809447558047 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 111.19701182623999,
            "unit": "ns/iter",
            "extra": "iterations: 1147000\ncpu: 110.32458587619881 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 953.3411227398037,
            "unit": "ns/iter",
            "extra": "iterations: 161000\ncpu: 869.8857142857142 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 2673943.2594992896,
            "unit": "ns/iter",
            "extra": "iterations: 275\ncpu: 480774.909090909 ns\nthreads: 1"
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
          "id": "e1b4a49f9b673cbdce4b992ae8fc712e49c39c50",
          "message": "disable failing test (#1193)",
          "timestamp": "2022-02-04T11:52:45-08:00",
          "tree_id": "ed584794fbdf5a6eb267184582f6fa13e64001f2",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e1b4a49f9b673cbdce4b992ae8fc712e49c39c50"
        },
        "date": 1644005085372,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 26.965957211735883,
            "unit": "ns/iter",
            "extra": "iterations: 5289000\ncpu: 26.896672338816412 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 76.0925626291812,
            "unit": "ns/iter",
            "extra": "iterations: 1957000\ncpu: 75.930454777721 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 658.3209307688587,
            "unit": "ns/iter",
            "extra": "iterations: 212000\ncpu: 655.9669811320757 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 2424333.3604600695,
            "unit": "ns/iter",
            "extra": "iterations: 450\ncpu: 416226.4444444445 ns\nthreads: 1"
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
          "id": "b6a28df5c46810728c1eb17ca797391695628e64",
          "message": "Metrics SDK: Filtering metrics attributes (#1191)",
          "timestamp": "2022-02-04T18:46:06-08:00",
          "tree_id": "ddcd8770b365e437fd35a082e33825bf256b751f",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/b6a28df5c46810728c1eb17ca797391695628e64"
        },
        "date": 1644030045505,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 37.24541920353201,
            "unit": "ns/iter",
            "extra": "iterations: 3815000\ncpu: 37.09473132372215 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 100.9206654755513,
            "unit": "ns/iter",
            "extra": "iterations: 1466000\ncpu: 100.83942701227829 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 801.3954288081119,
            "unit": "ns/iter",
            "extra": "iterations: 152000\ncpu: 799.2953947368422 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 2738745.0723087084,
            "unit": "ns/iter",
            "extra": "iterations: 255\ncpu: 498504.70588235307 ns\nthreads: 1"
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
          "id": "44795b6d21738a8478d620c6f4cde6f12ea5ccac",
          "message": "Add Aggregation as part of metrics SDK. (#1178)",
          "timestamp": "2022-02-04T21:53:10-08:00",
          "tree_id": "4e8e0f917db86d65af01d12113128c0637c8e104",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/44795b6d21738a8478d620c6f4cde6f12ea5ccac"
        },
        "date": 1644041050377,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 29.133603029344073,
            "unit": "ns/iter",
            "extra": "iterations: 4821000\ncpu: 29.054884878655887 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 79.75183214460101,
            "unit": "ns/iter",
            "extra": "iterations: 1750000\ncpu: 79.63222857142861 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 665.5607178312907,
            "unit": "ns/iter",
            "extra": "iterations: 211000\ncpu: 664.091469194313 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 1727450.5271647323,
            "unit": "ns/iter",
            "extra": "iterations: 433\ncpu: 298624.48036951496 ns\nthreads: 1"
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
          "id": "e1c85570329bb775d5294a1eb2d3f1d2b9f7e4b0",
          "message": "fix errors in SDK documentation (#1201)",
          "timestamp": "2022-02-09T12:35:54-08:00",
          "tree_id": "6f0fa081aec644ec46538fc807881e46f6abc82d",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e1c85570329bb775d5294a1eb2d3f1d2b9f7e4b0"
        },
        "date": 1644439715282,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 35.07031110662464,
            "unit": "ns/iter",
            "extra": "iterations: 4019000\ncpu: 35.07061458074148 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 97.4155452153454,
            "unit": "ns/iter",
            "extra": "iterations: 1460000\ncpu: 97.31308219178082 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 811.2825924837136,
            "unit": "ns/iter",
            "extra": "iterations: 158000\ncpu: 811.2854430379747 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 1855195.5457593573,
            "unit": "ns/iter",
            "extra": "iterations: 366\ncpu: 395525.40983606555 ns\nthreads: 1"
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
          "id": "04e3a68263a90ac0fef822fc1f87196b9c59c545",
          "message": "Sync and Async Instruments SDK (#1184)",
          "timestamp": "2022-02-09T22:27:55-08:00",
          "tree_id": "790993870b48f156a444c412c9953d597451c9e8",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/04e3a68263a90ac0fef822fc1f87196b9c59c545"
        },
        "date": 1644475125462,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 37.50732272745641,
            "unit": "ns/iter",
            "extra": "iterations: 4809000\ncpu: 29.06319401122895 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 97.47391551767,
            "unit": "ns/iter",
            "extra": "iterations: 1754000\ncpu: 79.66778791334093 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 663.6142730712891,
            "unit": "ns/iter",
            "extra": "iterations: 210000\ncpu: 662.5380952380953 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 2106060.7385741104,
            "unit": "ns/iter",
            "extra": "iterations: 451\ncpu: 359904.4345898004 ns\nthreads: 1"
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
          "id": "e89e5b3dd65b0e73435c12b9fdeb54f9420f4bb4",
          "message": "Benchmark documentation (#1205)",
          "timestamp": "2022-02-11T21:23:30-08:00",
          "tree_id": "ec08c9f6ed7b2efa77f3ed004f3b8b7e0c50c6f7",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/e89e5b3dd65b0e73435c12b9fdeb54f9420f4bb4"
        },
        "date": 1644644003519,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 26.56598083624117,
            "unit": "ns/iter",
            "extra": "iterations: 5721000\ncpu: 25.14927460234225 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 95.8762517789515,
            "unit": "ns/iter",
            "extra": "iterations: 1968000\ncpu: 79.45 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 666.0680635280519,
            "unit": "ns/iter",
            "extra": "iterations: 211000\ncpu: 663.2933649289099 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 1806545.6900737183,
            "unit": "ns/iter",
            "extra": "iterations: 441\ncpu: 311711.11111111124 ns\nthreads: 1"
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
          "id": "9ed312b29ca0af1d92bbf0cf63ab8a6d88272bfb",
          "message": "fix ostream_log_test Mac (#1208)",
          "timestamp": "2022-02-14T13:05:25-08:00",
          "tree_id": "1c45c26c34a4c593b49c6a31c05a8d7e9bb60312",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/9ed312b29ca0af1d92bbf0cf63ab8a6d88272bfb"
        },
        "date": 1644873378944,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 30.941866289862492,
            "unit": "ns/iter",
            "extra": "iterations: 4773000\ncpu: 29.080808715692438 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 80.4351872222813,
            "unit": "ns/iter",
            "extra": "iterations: 1745000\ncpu: 80.23971346704873 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 672.7320807320731,
            "unit": "ns/iter",
            "extra": "iterations: 210000\ncpu: 669.8276190476188 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 1789136.2655994503,
            "unit": "ns/iter",
            "extra": "iterations: 430\ncpu: 324086.0465116278 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "admin@owent.net",
            "name": "WenTao Ou",
            "username": "owent"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "15c6f33631d8cdf09b85911f35263449294d31a4",
          "message": "Update grpc to v1.43.2 to support VS2022/MSVC 19.30 and bazel 5.0 (#1207)\n\nSigned-off-by: owent <admin@owent.net>",
          "timestamp": "2022-02-14T20:24:58-08:00",
          "tree_id": "dff039d78807ee53fdc13748fd26ef7b48546fa8",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/15c6f33631d8cdf09b85911f35263449294d31a4"
        },
        "date": 1644899780582,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 31.36269283486564,
            "unit": "ns/iter",
            "extra": "iterations: 4970000\ncpu: 28.094225352112677 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 80.83449163948671,
            "unit": "ns/iter",
            "extra": "iterations: 1733000\ncpu: 80.76930178880556 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 628.0211465699332,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 627.6267857142857 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 1971188.8452091129,
            "unit": "ns/iter",
            "extra": "iterations: 426\ncpu: 356822.5352112676 ns\nthreads: 1"
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
          "id": "ef3a623a3bedd3fded167bade625d7fa392d2b88",
          "message": "Add Aggregation Storage (#1213)",
          "timestamp": "2022-02-16T14:58:32-08:00",
          "tree_id": "6f402b660d2b4da4a41caf6a91b28f6e4601ec2e",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/ef3a623a3bedd3fded167bade625d7fa392d2b88"
        },
        "date": 1645053073313,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 32.19280830200981,
            "unit": "ns/iter",
            "extra": "iterations: 4352000\ncpu: 32.135110294117645 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 91.91986782098907,
            "unit": "ns/iter",
            "extra": "iterations: 1530000\ncpu: 91.65137254901958 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 733.245392234958,
            "unit": "ns/iter",
            "extra": "iterations: 196000\ncpu: 714.4331632653061 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 2160533.5724126417,
            "unit": "ns/iter",
            "extra": "iterations: 367\ncpu: 358490.7356948229 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "Tom.Tan@microsoft.com",
            "name": "Tom Tan",
            "username": "ThomsonTan"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3a9d0870bc846f79f97c81796ce70440d944aa88",
          "message": "Detect vs2022 as build environment (#1212)",
          "timestamp": "2022-02-17T08:10:19Z",
          "tree_id": "361c175d162ec52518c7ad2e0eba088e6987fb3a",
          "url": "https://github.com/open-telemetry/opentelemetry-cpp/commit/3a9d0870bc846f79f97c81796ce70440d944aa88"
        },
        "date": 1645086273716,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_OtlpExporterEmptySpans",
            "value": 38.52880777527089,
            "unit": "ns/iter",
            "extra": "iterations: 3654000\ncpu: 38.285495347564314 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterSparseSpans",
            "value": 109.80950627581079,
            "unit": "ns/iter",
            "extra": "iterations: 1276000\ncpu: 109.7105799373041 ns\nthreads: 1"
          },
          {
            "name": "BM_OtlpExporterDenseSpans",
            "value": 865.5047855494213,
            "unit": "ns/iter",
            "extra": "iterations: 163000\ncpu: 862.1042944785277 ns\nthreads: 1"
          },
          {
            "name": "BM_otlp_grpc_with_collector",
            "value": 2759715.9907095158,
            "unit": "ns/iter",
            "extra": "iterations: 353\ncpu: 461126.912181303 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}