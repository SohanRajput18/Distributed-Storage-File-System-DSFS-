#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include "httplib.h"
#include <random>

using namespace std;
using namespace httplib;

// Metadata structure to store file-chunk information
unordered_map<string, vector<pair<string, vector<string>>>> metadata; // file -> {chunk_id, [nodes]}

// List of available storage nodes
vector<string> storage_nodes = {"http://localhost:8081", "http://localhost:8082", "http://localhost:8083", "http://localhost:8084"};

// Function to assign nodes for storing chunks
vector<string> assign_nodes(int replication_factor) {
    vector<string> assigned_nodes;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, storage_nodes.size() - 1);

    while (assigned_nodes.size() < replication_factor) {
        string node = storage_nodes[dis(gen)];
        if (find(assigned_nodes.begin(), assigned_nodes.end(), node) == assigned_nodes.end()) {
            assigned_nodes.push_back(node);
        }
    }
    return assigned_nodes;
}

int main() {
    Server server;

    // Handle file upload and chunk assignment
    server.Post("/upload", [](const Request &req, Response &res) {
        if (!req.has_file("file")) {
            res.status = 400;
            res.set_content("Missing file in the request.\n", "text/plain");
            return;
        }

        auto file_data = req.get_file_value("file");
        string file_name = file_data.filename;
        string file_content = file_data.content;

        const int chunk_size = 1024 * 1024; // 1 MB chunks
        const int replication_factor = 3;  // 3 replicas per chunk

        int total_chunks = (file_content.size() + chunk_size - 1) / chunk_size;
        vector<pair<string, vector<string>>> file_chunks;

        for (int i = 0; i < total_chunks; ++i) {
            string chunk_id = file_name + "_chunk_" + to_string(i);
            string chunk_data = file_content.substr(i * chunk_size, chunk_size);

            // Assign nodes for the chunk
            vector<string> assigned_nodes = assign_nodes(replication_factor);

            // Send chunk to the assigned nodes using query parameters instead of MultipartFormDataItems
            for (const string &node : assigned_nodes) {
                Client cli(node.c_str());
                string request_url = "/store_chunk?chunk_id=" + chunk_id + "&chunk_data=" + chunk_data;
                auto res = cli.Post(request_url.c_str());

                if (res && res->status == 200) {
                    cout << "Chunk " << chunk_id << " stored on " << node << endl;
                } else {
                    cerr << "Failed to store chunk " << chunk_id << " on " << node << endl;
                }
            }

            file_chunks.push_back({chunk_id, assigned_nodes});
        }

        metadata[file_name] = file_chunks;
        res.set_content("File uploaded and chunks stored successfully.\n", "text/plain");
    });

    // Handle metadata request
    server.Get("/metadata", [](const Request &req, Response &res) {
        stringstream ss;
        for (const auto &entry : metadata) {
            ss << "File: " << entry.first << "\n";
            for (const auto &chunk : entry.second) {
                ss << "  Chunk: " << chunk.first << " -> Nodes: ";
                for (const auto &node : chunk.second) {
                    ss << node << " ";
                }
                ss << "\n";
            }
        }
        res.set_content(ss.str(), "text/plain");
    });

    cout << "Master Node is running on http://localhost:8080" << endl;
    server.listen("localhost", 8080);

    return 0;  // Ensure proper termination
}


/*    g++ -std=c++17 -o master_node master_node.cpp -lws2_32
.\master_node       */ 

