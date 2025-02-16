# Distributed-Storage-File-System-DSFS-
Here's a **GitHub README.md** template for your **Distributed Storage File System (DSFS) project** in a structured format like the example you provided.  

---

## Distributed Storage File System (DSFS)  
![[Project Image](https://www.researchgate.net/publication/355175965/figure/fig1/AS:1078308498354177@1634100188750/Fig-Distributed-File-System-general-architecture-8.ppm)](#)  

## Overview  
The **Distributed Storage File System (DSFS)** is a **C++-based distributed file storage system** that efficiently stores, retrieves, and manages files across multiple nodes using an **HTTP-based communication model**. The system splits files into **smaller chunks**, distributes them across multiple storage nodes, and retrieves them seamlessly when needed.  

Key Features:  
✅ **File Chunking & Distribution** – Automatically splits files into chunks for distributed storage.  
✅ **Fault-Tolerant Replication** – Each chunk is stored on multiple nodes for redundancy.  
✅ **HTTP-Based API** – Uses `httplib.h` for communication between Master and Storage Nodes.  
✅ **Scalability** – New Storage Nodes can be added dynamically.  
✅ **Parallel Retrieval** – File chunks are fetched from multiple nodes simultaneously.  

---

## Table of Contents  
- [Introduction](#introduction)  
- [System Architecture](#system-architecture)  
- [Functional Requirements](#functional-requirements)  
- [Non-Functional Requirements](#non-functional-requirements)  
- [System Design & Implementation](#system-design--implementation)  
  - [Master Node](#master-node)  
  - [Storage Nodes](#storage-nodes)  
- [Technologies Used](#technologies-used)  
- [How It Works](#how-it-works)  
  - [Uploading a File](#uploading-a-file)  
  - [Retrieving a File](#retrieving-a-file)  
- [API Endpoints](#api-endpoints)  
- [Setup & Installation](#setup--installation)  
- [Testing](#testing)  
- [Future Enhancements](#future-enhancements)  
- [Conclusion](#conclusion)  

---

## Introduction  
Traditional **centralized file storage systems** suffer from **single points of failure, slow access times, and limited scalability**. This **Distributed Storage File System (DSFS)** overcomes these limitations by **splitting files into chunks, distributing them across multiple nodes, and ensuring redundancy** for fault tolerance.  

---

## System Architecture  
The **DSFS system** consists of:  
1️⃣ **Master Node** – Manages metadata, file chunking, and chunk distribution.  
2️⃣ **Storage Nodes** – Store file chunks and respond to retrieval requests.  
3️⃣ **Client Interface** – Allows users to upload and retrieve files.  

Architecture Diagram:  
*(Insert a diagram illustrating how the Master Node and Storage Nodes communicate.)*  

---

## Functional Requirements  
✅ **User Interface** – Allows users to upload and retrieve files.  
✅ **File Chunking** – Splits files into smaller chunks before distribution.  
✅ **Metadata Management** – Tracks chunk locations for file retrieval.  
✅ **Replication** – Ensures redundancy by storing each chunk on multiple nodes.  
✅ **File Retrieval** – Supports seamless reconstruction of files from chunks.  

## Non-Functional Requirements  
✅ **Performance** – Upload and retrieval should be fast.  
✅ **Fault Tolerance** – System should work even if some nodes fail.  
✅ **Scalability** – More storage nodes can be added as needed.  

---

## System Design & Implementation  
###🔹 Master Node  
- Accepts **file uploads** and splits them into **fixed-size chunks**.  
- Assigns chunks to multiple **Storage Nodes** based on a **replication factor**.  
- Maintains **metadata** about chunk locations.  

Master Node API (`master_node.cpp`) 
```cpp
server.Post("/upload", [](const Request &req, Response &res) {
    auto file_data = req.get_file_value("file");
    string file_name = file_data.filename;
    string file_content = file_data.content;

    int chunk_size = 1024 * 1024; // 1MB chunks
    int replication_factor = 2;  // Each chunk is stored on 2 nodes

    vector<pair<string, vector<string>>> file_chunks;
    for (int i = 0; i < total_chunks; ++i) {
        string chunk_id = file_name + "_chunk_" + to_string(i);
        string chunk_data = file_content.substr(i * chunk_size, chunk_size);
        vector<string> assigned_nodes = assign_nodes(replication_factor);

        for (const string &node : assigned_nodes) {
            Client cli(node.c_str());
            string request_url = "/store_chunk?chunk_id=" + chunk_id + "&chunk_data=" + chunk_data;
            auto res = cli.Post(request_url.c_str());
        }
        file_chunks.push_back({chunk_id, assigned_nodes});
    }
    metadata[file_name] = file_chunks;
});
```

---

###🔹 Storage Nodes  
- Receive **chunk storage requests** from the Master Node.  
- Store file chunks locally and return them upon request.  

Storage Node API (`storage_node.cpp`) 
```cpp
server.Post("/store_chunk", [](const Request &req, Response &res) {
    string chunk_id = req.get_param_value("chunk_id");
    string chunk_data = req.get_param_value("chunk_data");
    chunk_storage[chunk_id] = chunk_data;
    res.set_content("Chunk stored successfully.", "text/plain");
});

server.Get("/retrieve_chunk", [](const Request &req, Response &res) {
    string chunk_id = req.get_param_value("chunk_id");
    if (chunk_storage.find(chunk_id) != chunk_storage.end()) {
        res.set_content(chunk_storage[chunk_id], "text/plain");
    } else {
        res.status = 404;
        res.set_content("Chunk not found.", "text/plain");
    }
});
```

---

## Technologies Used
✅ **Programming:** C++  
✅ **Networking:** HTTP (`httplib.h`)  
✅ **Storage:** File-based chunk storage  
✅ **API Communication:** RESTful API  
✅ **Tools:** VS Code, PowerShell, `curl`, MinGW  

---

## How It Works
### 1️⃣ Uploading a File
```
curl.exe -X POST -F "file=@sample.txt" http://localhost:8080/upload
```
- The **file is chunked** and stored across multiple Storage Nodes.

### 2️⃣ Retrieving a File
```
curl.exe "http://localhost:8081/retrieve_chunk?chunk_id=sample.txt_chunk_0"
```
- The chunk is retrieved from **Storage Node 8081**.

### 3️⃣ Check Metadata
```
curl.exe http://localhost:8080/metadata
```
- Shows where **each chunk is stored**.

---

## API Endpoints
| **Endpoint** | **Method** | **Description** |
|-------------|-----------|----------------|
| `/upload` | `POST` | Upload a file to the Master Node. |
| `/metadata` | `GET` | Get metadata of stored files. |
| `/store_chunk` | `POST` | Store a chunk on a Storage Node. |
| `/retrieve_chunk` | `GET` | Retrieve a chunk from a Storage Node. |

---

## Future Enhancements
 **Full File Retrieval API** – Automate chunk retrieval and reconstruction.  
 **Dynamic Replication** – Adjust replication factor based on available nodes.  
 **Web Interface** – Add a frontend for uploading/downloading files.  
 **Security Enhancements** – Implement encryption and authentication.  

---

## Conclusion
The **Distributed Storage File System (DSFS)** enables **scalable, fault-tolerant file storage** using a **decentralized approach**. By implementing **chunking, replication, and HTTP-based communication**, it provides a **robust alternative to centralized storage systems**.

---

### Reports and Downloads
🔗 *(Add link to a project report or additional documentation if available.)*  

Would you like a **sample project report PDF** or any modifications to this README? 🚀🔥
