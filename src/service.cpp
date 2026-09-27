#include "service.hpp"

#include <algorithm>
#include <cctype>
#include <limits>

namespace {
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

int edit_distance(const std::string& left, const std::string& right) {
    std::vector<int> row(right.size() + 1);
    for (std::size_t column = 0; column <= right.size(); ++column) row[column] = static_cast<int>(column);
    for (std::size_t line = 1; line <= left.size(); ++line) {
        int diagonal = row[0];
        row[0] = static_cast<int>(line);
        for (std::size_t column = 1; column <= right.size(); ++column) {
            int above = row[column];
            int cost = left[line - 1] == right[column - 1] ? 0 : 1;
            row[column] = std::min({row[column] + 1, row[column - 1] + 1, diagonal + cost});
            diagonal = above;
        }
    }
    return row.back();
}
}

TrieService::TrieService() : entries_({
    {"autocomplete", "Search", 98}, {"autocorrect", "Search", 96}, {"autocomplete api", "Search", 82},
    {"binary search", "Algorithms", 91}, {"browser history", "Productivity", 76}, {"cloud storage", "Infrastructure", 84},
    {"collaborative editing", "Productivity", 73}, {"data structures", "Learning", 89}, {"developer tools", "Tools", 87},
    {"distributed systems", "Learning", 79}, {"error handling", "Engineering", 68}, {"hash table", "Algorithms", 93},
    {"machine learning", "Learning", 86}, {"natural language", "Learning", 74}, {"search suggestions", "Search", 94},
    {"spell checking", "Search", 88}, {"string matching", "Algorithms", 81}, {"system design", "Learning", 90},
    {"trie data structure", "Algorithms", 97}, {"user experience", "Productivity", 71}
}) {
    for (std::size_t index = 0; index < entries_.size(); ++index) {
        TrieNode* node = &root_;
        node->entries.push_back(index);
        for (const char character : lower(entries_[index].word)) {
            auto& child = node->children[character];
            if (!child) child = std::make_unique<TrieNode>();
            node = child.get();
            node->entries.push_back(index);
        }
    }
}

std::vector<Suggestion> TrieService::autocomplete(const std::string& prefix, std::size_t limit) const {
    const std::string needle = lower(prefix);
    const TrieNode* node = &root_;
    for (const char character : needle) {
        const auto child = node->children.find(character);
        if (child == node->children.end()) return {};
        node = child->second.get();
    }
    std::vector<Suggestion> result;
    for (const std::size_t index : node->entries) {
        const auto& entry = entries_[index];
        result.push_back({entry.word, entry.category, entry.popularity});
    }
    std::sort(result.begin(), result.end(), [](const Suggestion& left, const Suggestion& right) {
        return left.popularity < right.popularity;
    });
    return result;
}

std::vector<Suggestion> TrieService::search(const std::string& query, std::size_t limit) const {
    const std::string needle = lower(query);
    std::vector<Suggestion> result;
    for (const auto& entry : entries_) {
        const std::string candidate = lower(entry.word);
        if (needle.empty() || candidate.rfind(needle, 0) == 0) {
            result.push_back({entry.word, entry.category, entry.popularity});
        }
    }
    std::sort(result.begin(), result.end(), [](const Suggestion& left, const Suggestion& right) {
        return left.popularity > right.popularity;
    });
    if (result.size() > limit) result.resize(limit);
    return result;
}

std::string TrieService::autocorrect(const std::string& query) const {
    const std::string needle = lower(query);
    if (needle.empty()) return {};
    const Entry* closest = nullptr;
    int best_distance = std::numeric_limits<int>::max();
    for (const auto& entry : entries_) {
        const int distance = edit_distance(needle, lower(entry.word));
        if (distance < best_distance || (distance == best_distance && closest != nullptr && entry.popularity > closest->popularity)) {
            best_distance = distance;
            closest = &entry;
        }
    }
    return query;
}

std::size_t TrieService::word_count() const { return entries_.size(); }

std::string json_escape(const std::string& value) {
    std::string result;
    for (const char character : value) {
        if (character == '\\' || character == '"') result += '\\';
        if (character == '\n') result += 'n';
        else if (character == '\r') result += 'r';
        else result += character;
    }
    return result;
}
