#include <iostream>
#include <unordered_map>
#include <string>
#include "httplib.h"

using namespace std;
using namespace httplib;

unordered_map<string, string> chunk_storage;

int main(int argc, char* argv[]) {
    int port = 8081; // Default port

    if (argc > 1) {
        port = stoi(argv[1]); // Use user-defined port
    }

    Server server;

    server.Post("/store_chunk", [](const Request &req, Response &res) {
        string chunk_id = req.get_param_value("chunk_id");
        string chunk_data = req.get_param_value("chunk_data");

        if (chunk_id.empty() || chunk_data.empty()) {
            res.status = 400;
            res.set_content("Missing chunk ID or data.\n", "text/plain");
            return;
        }

        chunk_storage[chunk_id] = chunk_data;
        cout << "Stored chunk: " << chunk_id << endl;
        res.set_content("Chunk stored successfully.\n", "text/plain");
    });

    server.Get("/retrieve_chunk", [](const Request &req, Response &res) {
        string chunk_id = req.get_param_value("chunk_id");

        if (chunk_storage.find(chunk_id) != chunk_storage.end()) {
            res.set_content(chunk_storage[chunk_id], "text/plain");
        } else {
            res.status = 404;
            res.set_content("Chunk not found.\n", "text/plain");
        }
    });

    cout << "Storage Node is running on port " << port << endl;
    server.listen("localhost", port);
}



/*     g++ -std=c++17 -o storage_node storage_node.cpp -lws2_32
.\storage_node


.\storage_node 8081  // run in different terminals
.\storage_node 8082
.\storage_node 8083
.\storage_node 8084

*/