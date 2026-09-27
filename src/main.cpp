#include "service.hpp"

#include <cstdlib>
#include <csignal>
#include <fstream>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

namespace {
int server_socket = -1;

std::string url_decode(const std::string& value) {
    std::string decoded;
    for (std::size_t index = 0; index < value.size(); ++index) {
        if (value[index] == '+') decoded += ' ';
        else if (value[index] == '%' && index + 2 < value.size()) {
            decoded += static_cast<char>(std::stoi(value.substr(index + 1, 2), nullptr, 16));
            index += 2;
        } else decoded += value[index];
    }
    return decoded;
}

std::string query_value(const std::string& path, const std::string& key) {
    const auto start = path.find('?');
    if (start == std::string::npos) return {};
    std::stringstream query(path.substr(start + 1));
    std::string part;
    while (std::getline(query, part, '&')) {
        const auto divider = part.find('=');
        if (divider != std::string::npos && part.substr(0, divider) == key) return url_decode(part.substr(divider + 1));
    }
    return {};
}

std::string suggestions_json(const std::vector<Suggestion>& suggestions) {
    std::string body = "[";
    for (std::size_t index = 0; index < suggestions.size(); ++index) {
        if (index) body += ',';
        body += "{\"word\":\"" + json_escape(suggestions[index].word) + "\",\"category\":\"" + json_escape(suggestions[index].category) + "\",\"popularity\":" + std::to_string(suggestions[index].popularity) + "}";
    }
    return body + ']';
}

std::string response(const std::string& content, const std::string& type = "text/html") {
    return "HTTP/1.1 200 OK\r\nContent-Type: " + type + "; charset=utf-8\r\nContent-Length: " + std::to_string(content.size()) + "\r\nConnection: close\r\n\r\n" + content;
}

void stop_server(int) {
    if (server_socket >= 0) close(server_socket);
    std::_Exit(0);
}
}

int main() {
    std::signal(SIGTERM, stop_server);
    std::signal(SIGINT, stop_server);
    TrieService service;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int reusable = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reusable, sizeof(reusable));
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if (bind(server_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0 || listen(server_socket, 12) < 0) return 1;

    while (true) {
        const int client = accept(server_socket, nullptr, nullptr);
        if (client < 0) continue;
        char buffer[8192]{};
        const ssize_t received = read(client, buffer, sizeof(buffer) - 1);
        if (received <= 0) { close(client); continue; }
        std::string request(buffer, static_cast<std::size_t>(received));
        std::stringstream first_line(request.substr(0, request.find("\r\n")));
        std::string method, path, version;
        first_line >> method >> path >> version;
        std::string body;
        std::string type = "application/json";
        if (path.rfind("/api/suggestions", 0) == 0) body = "{\"suggestions\":" + suggestions_json(service.autocomplete(query_value(path, "q"))) + "}";
        else if (path.rfind("/api/search", 0) == 0) body = "{\"results\":" + suggestions_json(service.search(query_value(path, "q"))) + "}";
        else if (path.rfind("/api/autocorrect", 0) == 0) body = "{\"original\":\"" + json_escape(query_value(path, "q")) + "\",\"correction\":\"" + json_escape(service.autocorrect(query_value(path, "q"))) + "\"}";
        else if (path.rfind("/api/stats", 0) == 0) body = "{\"words\":" + std::to_string(service.word_count()) + ",\"features\":3}";
        else {
            const std::string route = path.substr(0, path.find('?'));
            std::string asset = "public/index.html";
            if (route == "/style.css") { asset = "public/style.css"; type = "text/css"; }
            else if (route == "/app.js") { asset = "public/app.js"; type = "application/javascript"; }
            std::ifstream file(asset);
            body.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            if (route == "/" || route.empty()) type = "text/html";
        }
        const std::string result = response(body, type);
        write(client, result.c_str(), result.size());
        close(client);
    }
}
