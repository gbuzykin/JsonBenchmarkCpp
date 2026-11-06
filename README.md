# JsonBenchmarkCpp 

JsonBenchmarkCpp is a small program to benchmark various JSON frameworks in C++

Currently supports the following libs:

1. [uxs::db::json](https://github.com/gbuzykin/uxs) (8e3ed22916 Thu Nov 5 16:51:46 2026 +0300)
2. [rapidjson](https://github.com/Tencent/rapidjson) (297637bc0a Thu Oct 29 09:30:27 2026 +0000)
3. [jsoncpp](https://github.com/open-source-parsers/jsoncpp) (93acc7b155 Mon Oct 26 21:23:47 2026 +0530)
4. [nlohmann](https://github.com/nlohmann/json) (55f93686c0 Fri Apr 11 10:42:28 2025 +0200)
5. [json-parser](https://github.com/udp/json-parser) (9d50ae5ea2 Sat Sep 19 20:54:10 2026 +0200)
6. [libjson](https://github.com/mousebird/libjson) (bdb42ade3b Tue Mar 6 13:35:54 2018 -0800)
7. [cajun](https://github.com/cajun-jsonapi/cajun-jsonapi) 8a26ad3a17 Wed Jun 16 22:40:16 2021 +0200)
8. [json_spirit](https://github.com/png85/json_spirit) (5e16cca59b Sun Sep 6 00:14:53 2015 +0200)
9. [avery](https://github.com/matthewaveryusa/averyws) (???)

See my [blog post](http://www.lijoantony.com/?p=5) for details.

## Building

```bash
$ git clone git://github.com/gbuzykin/JsonBenchmarkCpp.git
$ cmake -S . -B build
$ cmake --build build
$ cmake --install build --prefix install
```

## Running

```bash
$ cd testdata
$ ../install/bin/JsonBenchmarkCpp
```

## Results

Current run result (core-i7 14700 linux clang 22.1.8 + libc++):

```
running test for: canada.json
                 #library                  parsing                                writing
        uxs::db::json-DOM                   260.29 MiB/s ( 100%)                   631.62 MiB/s ( 100%)
        uxs::db::json-SAX                   656.08 MiB/s ( 252%)                      n/a
                rapidjson                   982.06 MiB/s ( 377%)                   453.21 MiB/s (  72%)
                 nlohmann                   120.12 MiB/s (  46%)                   324.34 MiB/s (  51%)
                  jsoncpp                    46.03 MiB/s (  18%)                   115.06 MiB/s (  18%)
              json-parser                   179.28 MiB/s (  69%)                      n/a
                  libjson                    57.78 MiB/s (  22%)                   524.74 MiB/s (  83%)
                    cajun                     8.47 MiB/s (   3%)                    89.46 MiB/s (  14%)
              json_spirit                    28.37 MiB/s (  11%)                    86.52 MiB/s (  14%)
                    avery                   475.65 MiB/s ( 183%)                   624.68 MiB/s (  99%)
running test for: citm_catalog.json
                 #library                  parsing                                writing
        uxs::db::json-DOM                   787.09 MiB/s ( 100%)                  1410.75 MiB/s ( 100%)
        uxs::db::json-SAX                  1828.92 MiB/s ( 232%)                      n/a
                rapidjson                  1659.61 MiB/s ( 211%)                  1522.70 MiB/s ( 108%)
                 nlohmann                   332.44 MiB/s (  42%)                   330.69 MiB/s (  23%)
                  jsoncpp                   273.45 MiB/s (  35%)                   235.73 MiB/s (  17%)
              json-parser                   291.02 MiB/s (  37%)                      n/a
                  libjson                   339.48 MiB/s (  43%)                   372.01 MiB/s (  26%)
                    cajun                    38.64 MiB/s (   5%)                    80.58 MiB/s (   6%)
              json_spirit                    21.92 MiB/s (   3%)                   165.14 MiB/s (  12%)
                    avery                   732.30 MiB/s (  93%)                   261.61 MiB/s (  19%)
running test for: gsoc-2018.json
                 #library                  parsing                                writing
        uxs::db::json-DOM                  1203.92 MiB/s ( 100%)                  2908.45 MiB/s ( 100%)
        uxs::db::json-SAX                  2179.42 MiB/s ( 181%)                      n/a
                rapidjson                  1532.73 MiB/s ( 127%)                  3457.04 MiB/s ( 119%)
                 nlohmann                   467.11 MiB/s (  39%)                   339.19 MiB/s (  12%)
                  jsoncpp                   290.22 MiB/s (  24%)                   331.14 MiB/s (  11%)
              json-parser                   434.69 MiB/s (  36%)                      n/a
                  libjson                   326.97 MiB/s (  27%)                   297.68 MiB/s (  10%)
                    cajun                    52.54 MiB/s (   4%)                    96.89 MiB/s (   3%)
              json_spirit                    28.21 MiB/s (   2%)                   210.84 MiB/s (   7%)
                    avery                  2051.92 MiB/s ( 170%)                  1873.66 MiB/s (  64%)
running test for: twitter.json
                 #library                  parsing                                writing
        uxs::db::json-DOM                   554.64 MiB/s ( 100%)                  2072.31 MiB/s ( 100%)
        uxs::db::json-SAX                  1442.94 MiB/s ( 260%)                      n/a
                rapidjson                   689.66 MiB/s ( 124%)                  1434.06 MiB/s (  69%)
                 nlohmann                   243.60 MiB/s (  44%)                   356.62 MiB/s (  17%)
                  jsoncpp                   183.11 MiB/s (  33%)                   290.89 MiB/s (  14%)
              json-parser                   299.25 MiB/s (  54%)                      n/a
                  libjson                   210.40 MiB/s (  38%)                   387.37 MiB/s (  19%)
                    cajun                    40.30 MiB/s (   7%)                   110.34 MiB/s (   5%)
              json_spirit                    22.80 MiB/s (   4%)                   338.12 MiB/s (  16%)
                    avery                   939.31 MiB/s ( 169%)                   510.35 MiB/s (  25%)
running test for: gltf.json
                 #library                  parsing                                writing
        uxs::db::json-DOM                   359.89 MiB/s ( 100%)                  1115.87 MiB/s ( 100%)
        uxs::db::json-SAX                  1037.97 MiB/s ( 288%)                      n/a
                rapidjson                   881.15 MiB/s ( 245%)                   886.76 MiB/s (  79%)
                 nlohmann                   199.99 MiB/s (  56%)                   285.71 MiB/s (  26%)
                  jsoncpp                   111.27 MiB/s (  31%)                   161.51 MiB/s (  14%)
              json-parser                   238.92 MiB/s (  66%)                      n/a
                  libjson                   132.30 MiB/s (  37%)                   326.59 MiB/s (  29%)
                    cajun                    25.16 MiB/s (   7%)                    78.94 MiB/s (   7%)
              json_spirit                    20.62 MiB/s (   6%)                   127.35 MiB/s (  11%)
                    avery                   507.18 MiB/s ( 141%)                   275.40 MiB/s (  25%)
running test for: wikidatawiki-20220720-pages-articles-multistream6.xml-p5969005p6052571
                 #library                  parsing                                writing
         uxs::db::xml-SAX                   805.23 MiB/s ( 100%)                      n/a
running test for: iceland-latest.osm
                 #library                  parsing                                writing
         uxs::db::xml-SAX                   564.78 MiB/s ( 100%)                      n/a
```
        
## Thanks

Thanks to the library authors for their effort and time. You saved me from reinventing the same wheel again!

Special thanks to Jonathan Wallace, for actively developing libjson and helping me with my queries.
