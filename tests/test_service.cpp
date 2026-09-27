#include "service.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

struct TestResult { std::string name; bool passed; std::string error; long long milliseconds; };

int main() {
    TrieService service;
    std::vector<TestResult> tests;
    auto run = [&](const std::string& name, const auto& check, const std::string& error) {
        const auto start = std::chrono::steady_clock::now();
        bool passed = check();
        const auto elapsed = std::max<long long>(1, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count());
        tests.push_back({name, passed, passed ? "" : error, elapsed});
    };
    run("test_prefix_autocomplete", [&] { auto items = service.autocomplete("auto"); return items.size() >= 2 && items[0].word == "autocomplete"; }, "Expected ranked autocomplete matches for prefix auto");
    run("test_search_suggestions", [&] { auto items = service.search("search"); return items.size() == 2 && items[0].word == "search suggestions"; }, "Expected substring search to return two search entries");
    run("test_spell_autocorrect", [&] { return service.autocorrect("autocorect") == "autocorrect"; }, "Expected autocorect to be corrected to autocorrect");
    run("test_empty_query_and_limit", [&] { return service.autocomplete("", 3).size() == 3 && service.word_count() == 20; }, "Expected empty prefix to honor limit and expose 20 indexed words");

    int passed = 0;
    long long total_time = 0;
    std::cout << "{";
    for (const auto& test : tests) {
        if (test.passed) ++passed;
        total_time += test.milliseconds;
        std::cout << "\"" << test.name << "\":{\"Status\":\"" << (test.passed ? "passed" : "failed") << "\",\"Execution time\":\"" << test.milliseconds << "ms\"";
        if (!test.passed) std::cout << ",\"Error\":\"" << json_escape(test.error) << "\"";
        std::cout << "},";
    }
    std::cout << "\"Passed\":" << passed << ",\"Failed\":" << (tests.size() - passed) << ",\"Total bugs\":" << (tests.size() - passed) << ",\"Total Execution time\":\"" << total_time << "ms\"}\n";
    return passed == static_cast<int>(tests.size()) ? 0 : 1;
}
