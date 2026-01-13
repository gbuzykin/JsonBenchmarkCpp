/*
 * JsonBenchmarkCpp
 * A small program to compare performance of different json libs available
 *
 * Currently supporting following libs,
 *
 * 1. uxs::db::json (https://github.com/gbuzykin/uxs.git 2e0aaa9004 Fri Dec 26 17:24:45 2025 +0300)
 * 2. rapidjson (https://github.com/Tencent/rapidjson.git 24b5e7a8b2 Sun Dec 22 15:53:24 2024 +0200)
 * 3. jsoncpp (https://github.com/open-source-parsers/jsoncpp.git b511d9e649 Wed Nov 12 09:14:04 2025 +0100)
 * 4. nlohmann (https://github.com/nlohmann/json.git 55f93686c0 Fri Apr 11 10:42:28 2025 +0200)
 * 5. libjson (https://github.com/mousebird/libjson.git bdb42ade3b Tue Mar 6 13:35:54 2018 -0800)
 * 6. cajun (https://github.com/cajun-jsonapi/cajun-jsonapi.git 8a26ad3a17 Wed Jun 16 22:40:16 2021 +0200)
 * 7. json_spirit (https://github.com/png85/json_spirit.git 5e16cca59b Sun Sep 6 00:14:53 2015 +0200)
 * 8. json-parser (https://github.com/udp/json-parser.git 8ac4477ad3 Fri May 9 20:01:35 2025 -0500)
 * 9. avery
 *
 * Copyright Lijo Antony 2011
 * Distributed under Apache License, Version 2.0
 * see accompanying file LICENSE.txt
 */

#include <nlohmann/json.hpp>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// Cajun headers
#include <cajun/json/elements.h>
#include <cajun/json/reader.h>
#include <cajun/json/writer.h>

// json_spirit headers
#include <json_spirit.h>

// libjson headers
#include <libjson.h>

// json-parser headers
#define json_string __json_string
#include <json-parser/json.h>
#undef json_string

#include <utilities_js.hpp>

// rapidjson headers
#if defined(__SSE4_2__)
#    define RAPIDJSON_SSE42
#elif defined(__SSE2__)
#    define RAPIDJSON_SSE2
#endif

#include <uxs/db/json.h>
#include <uxs/db/value.h>
#include <uxs/db/xml.h>
#include <uxs/io/iflatbuf.h>
#include <uxs/io/oflatbuf.h>

#include <json/reader.h>
#include <json/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

/*
 * @brief A function to print time duration
 *
 * @param start starttime
 * @param end   endtime
 * @return none
 */
void printBenchmark(const char* name, std::pair<double, double> time, std::pair<double, double> ref) {
    std::cout << std::setw(25) << name;
    if (time.first != 0) {
        std::cout << std::setw(25) << std::fixed << std::setprecision(2) << 0.000001 * time.first << std::setw(0)
                  << " MiB/s (" << std::setw(4) << static_cast<int>(floor(.5 + (100. * time.first) / ref.first))
                  << std::setw(0) << "%)";
    } else {
        std::cout << std::setw(25) << "n/a" << std::setw(0) << "              ";
    }
    if (time.second != 0) {
        std::cout << std::setw(25) << std::fixed << std::setprecision(2) << 0.000001 * time.second << std::setw(0)
                  << " MiB/s (" << std::setw(4) << static_cast<int>(floor(.5 + (100. * time.second) / ref.second))
                  << std::setw(0) << "%)";
    } else {
        std::cout << std::setw(25) << "n/a" << std::setw(0) << "              ";
    }
    std::cout << std::endl;
}

/*
 * @brief function for cajun benchmark
 *
 * @param jsonString test data as a string
 * @return none
 */
std::pair<double, double> cajunBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;
    iter_count /= 10;

    try {
        // Parsing the string
        json::Object obj;
        std::istringstream buff(jsonString);
        time1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iter_count; i++) {
            obj.Clear();
            json::Reader::Read(obj, buff);
            buff.clear();
            buff.seekg(0);
        }
        time2 = std::chrono::high_resolution_clock::now();

        double parse_speed = jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count();

        // Serialize to string
        std::ostringstream out;
        time1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iter_count; i++) {
            out.str("");
            json::Writer::Write(obj, out);
        }
        time2 = std::chrono::high_resolution_clock::now();

        std::string s_out(out.str());

        std::ofstream ofs(name + "-cajun.json");
        ofs << s_out;
        return {parse_speed, s_out.size() * iter_count / std::chrono::duration<double>(time2 - time1).count()};

    } catch (const std::exception&) { return {0, 0}; }
}

/*
 * @brief function for json_spirit benchmark
 *
 * @param jsonString test data as a string
 * @return none
 */
std::pair<double, double> jsonspiritBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;
    iter_count /= 10;

    // Parsing the string
    json_spirit::Value value;
    std::istringstream buff(jsonString);
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        json_spirit::read(buff, value);
        buff.clear();
        buff.seekg(0);
    }
    time2 = std::chrono::high_resolution_clock::now();

    double parse_speed = jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count();

    // Serialize to string
    std::ostringstream out;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        out.str("");
        json_spirit::write(value, out);
    }
    time2 = std::chrono::high_resolution_clock::now();

    std::string s_out(out.str());

    std::ofstream ofs(name + "-jsonspirit.json");
    ofs << s_out;
    return {parse_speed, s_out.size() * iter_count / std::chrono::duration<double>(time2 - time1).count()};
}
/*
 * @brief function for libjson benchmark
 *
 * @param jsonString test data as a string
 * @return none
 */
std::pair<double, double> libjsonBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;

    // Parsing the string
    JSONNode n;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) { n = libjson::parse(jsonString); }
    time2 = std::chrono::high_resolution_clock::now();

    double parse_speed = jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count();

    // Serialize to string
    std::string s_out;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) { s_out = n.write(); }
    time2 = std::chrono::high_resolution_clock::now();

    std::ofstream ofs(name + "-libjson.json");
    ofs << s_out;
    return {parse_speed, s_out.size() * iter_count / std::chrono::duration<double>(time2 - time1).count()};
}

/*
 * @brief function for json-parser benchmark
 *
 * @param jsonString test data as a string
 * @return none
 */
std::pair<double, double> jsonparserBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;

    // Parsing the string
    json_value* value;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        value = json_parse(jsonString.data(), jsonString.size());
        json_value_free(value);
    }
    time2 = std::chrono::high_resolution_clock::now();

    return {jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count(), 0};
}

/*
 * @brief function for Avery benchmark
 *
 * @param jsonString test data as a string
 * @return none
 */
std::pair<double, double> averyBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;

    // Parsing the string
    Utilities::JS::Node root;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        Utilities::JS::Node::parse(jsonString.data(), jsonString.data() + jsonString.size(), root);
    }
    time2 = std::chrono::high_resolution_clock::now();

    double parse_speed = jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count();

    // Serialize to string
    std::ostringstream out;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        out.str("");
        out << root;
    }
    time2 = std::chrono::high_resolution_clock::now();

    std::string s_out(out.str());

    std::ofstream ofs(name + "-avery.json");
    ofs << s_out;
    return {parse_speed, s_out.size() * iter_count / std::chrono::duration<double>(time2 - time1).count()};
}

/*
 * @brief function for RapidJSON benchmark
 *
 * @param jsonString test data as a string
 * @return none
 */
std::pair<double, double> rapidjsonBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    using namespace rapidjson;
    std::chrono::high_resolution_clock::time_point time1, time2;

    // Parsing the string
    Document d;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) { d.Parse(jsonString.data()); }
    time2 = std::chrono::high_resolution_clock::now();

    double parse_speed = jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count();

    // Serialize to string
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        sb.Clear();
        d.Accept(writer);
    }
    time2 = std::chrono::high_resolution_clock::now();

    std::string s_out = sb.GetString();

    std::ofstream ofs(name + "-rapidjson.json");
    ofs << s_out;
    return {parse_speed, s_out.size() * iter_count / std::chrono::duration<double>(time2 - time1).count()};
}

std::pair<double, double> jsonCppBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;
    iter_count /= 2;

    // Parsing the string
    Json::Value root;
    std::istringstream buff(jsonString);
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        root.clear();
        buff >> root;
        buff.clear();
        buff.seekg(0);
    }
    time2 = std::chrono::high_resolution_clock::now();

    double parse_speed = jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count();

    // Serialize to string
    std::ostringstream out;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        out.str("");
        out << root;
    }
    time2 = std::chrono::high_resolution_clock::now();

    std::string s_out(out.str());

    std::ofstream ofs(name + "-jsoncpp.json");
    ofs << s_out;
    return {parse_speed, s_out.size() * iter_count / std::chrono::duration<double>(time2 - time1).count()};
}

std::pair<double, double> uxsDbJsonBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;

    try {
        uxs::iflatbuf buff(jsonString);
        uxs::oflatbuf out;

        // Parsing the string
        uxs::db::value v;
        time1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iter_count; i++) {
            v = uxs::db::json::read(buff);
            buff.clear();
            buff.seek(0);
        }
        time2 = std::chrono::high_resolution_clock::now();

        double parse_speed = jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count();

        // Serialize to string
        time1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iter_count; i++) {
            out.seek(0);
            out.truncate();
            uxs::db::json::write(out, v);
        }
        time2 = std::chrono::high_resolution_clock::now();

        std::string s_out(out.data(), out.size());

        std::ofstream ofs(name + "-uxs.json");
        ofs << s_out;

        return {parse_speed, s_out.size() * iter_count / std::chrono::duration<double>(time2 - time1).count()};

    } catch (const std::exception&) { return {0, 0}; }
}

std::pair<double, double> uxsDbJsonBenchmark_SAX(const std::string& name, const std::string& jsonString,
                                                 int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;

    try {
        uxs::iflatbuf buff(jsonString);

        // Parsing the string
        time1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iter_count; i++) {
            uxs::db::json::read(
                buff, [](uxs::db::json::token_t, std::string_view) { return uxs::db::json::parse_step::into; }, []() {},
                [](std::string_view) {}, []() {});
            buff.clear();
            buff.seek(0);
        }
        time2 = std::chrono::high_resolution_clock::now();

        return {jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count(), 0};

    } catch (const std::exception&) { return {0, 0}; }
}

std::pair<double, double> nlohmannBenchmark(const std::string& name, const std::string& jsonString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;

    // Parsing the string
    nlohmann::json value;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) { value = nlohmann::json::parse(jsonString); }
    time2 = std::chrono::high_resolution_clock::now();

    double parse_speed = jsonString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count();

    // Serialize to string
    std::ostringstream out;
    time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter_count; i++) {
        out.str("");
        out << value;
    }
    time2 = std::chrono::high_resolution_clock::now();

    std::string s_out(out.str());

    std::ofstream ofs(name + "-nlohmann.json");
    ofs << s_out;
    return {parse_speed, s_out.size() * iter_count / std::chrono::duration<double>(time2 - time1).count()};
}

void testJson(const std::string& name) {
    std::cout << "running test for: " << name << std::endl;

    std::string buff;
    std::ifstream ifs(name, std::ifstream::in);
    if (ifs.is_open()) {
        ifs.seekg(0, std::ios_base::end);
        buff.reserve(ifs.tellg());
        ifs.seekg(0);
        for (char ch = ifs.get(); ifs; ch = ifs.get()) { buff.push_back(ch); }
    }

    if (buff.empty()) {
        std::cout << "No data available for test, exiting!" << std::endl;
        exit(1);
    }

    int iter_count = static_cast<int>((512ull * 1024 * 1024 + buff.size() - 1) / buff.size());

    std::cout << std::setw(25) << "#library" << std::setw(25) << "parsing" << std::setw(39) << "writing" << std::endl;

    auto ref = uxsDbJsonBenchmark(name, buff, iter_count);
    printBenchmark("uxs::db::json-DOM", ref, ref);
    printBenchmark("uxs::db::json-SAX", uxsDbJsonBenchmark_SAX(name, buff, iter_count), ref);
    printBenchmark("rapidjson", rapidjsonBenchmark(name, buff, iter_count), ref);
    printBenchmark("nlohmann", nlohmannBenchmark(name, buff, iter_count), ref);
    printBenchmark("jsoncpp", jsonCppBenchmark(name, buff, iter_count), ref);
    printBenchmark("libjson", libjsonBenchmark(name, buff, iter_count), ref);
    printBenchmark("cajun", cajunBenchmark(name, buff, iter_count), ref);
    printBenchmark("json_spirit", jsonspiritBenchmark(name, buff, iter_count), ref);
    printBenchmark("json-parser", jsonparserBenchmark(name, buff, iter_count), ref);
    printBenchmark("avery", averyBenchmark(name, buff, iter_count), ref);
}

std::pair<double, double> uxsDbXmlBenchmark_SAX(const std::string& name, const std::string& xmlString, int iter_count) {
    std::chrono::high_resolution_clock::time_point time1, time2;

    try {
        uxs::iflatbuf buff(xmlString);

        // Parsing the string
        time1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iter_count; i++) {
            {
                uxs::db::xml::parser rd(buff);
                uxs::db::xml::parser::iterator it{rd}, it_end{};
                while (it != it_end) { ++it; }
            }
            buff.clear();
            buff.seek(0);
        }
        time2 = std::chrono::high_resolution_clock::now();

        return {xmlString.size() * iter_count / std::chrono::duration<double>(time2 - time1).count(), 0};

    } catch (const std::exception&) { return {0, 0}; }
}

void testXml(const std::string& name) {
    std::cout << "running test for: " << name << std::endl;

    std::string buff;
    std::ifstream ifs(name, std::ifstream::in);
    if (ifs.is_open()) {
        ifs.seekg(0, std::ios_base::end);
        buff.reserve(ifs.tellg());
        ifs.seekg(0);
        for (char ch = ifs.get(); ifs; ch = ifs.get()) { buff.push_back(ch); }
    }

    if (buff.empty()) {
        std::cout << "No data available for test, exiting!" << std::endl;
        exit(1);
    }

    int iter_count = static_cast<int>((512ull * 1024 * 1024 + buff.size() - 1) / buff.size());

    std::cout << std::setw(25) << "#library" << std::setw(25) << "parsing" << std::setw(39) << "writing" << std::endl;

    auto ref = uxsDbXmlBenchmark_SAX(name, buff, iter_count);
    printBenchmark("uxs::db::xml-SAX", ref, ref);
}

int main() {
    testJson("canada.json");
    testJson("citm_catalog.json");
    testJson("gsoc-2018.json");
    testJson("twitter.json");
    testJson("gltf.json");
    testXml("wikidatawiki-20220720-pages-articles-multistream6.xml-p5969005p6052571");
    testXml("iceland-latest.osm");
    return 0;
}
