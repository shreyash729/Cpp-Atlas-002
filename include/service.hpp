#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

struct Suggestion {
    std::string word;
    std::string category;
    int popularity;
};

class TrieService {
public:
    TrieService();
    std::vector<Suggestion> autocomplete(const std::string& prefix, std::size_t limit = 6) const;
    std::vector<Suggestion> search(const std::string& query, std::size_t limit = 6) const;
    std::string autocorrect(const std::string& query) const;
    std::size_t word_count() const;

private:
    struct Entry {
        std::string word;
        std::string category;
        int popularity;
    };

    struct TrieNode {
        std::map<char, std::unique_ptr<TrieNode>> children;
        std::vector<std::size_t> entries;
    };

    std::vector<Entry> entries_;
    TrieNode root_;
};

std::string json_escape(const std::string& value);
