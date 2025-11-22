# 🚨 SPL251 Assignment 3 – Emergency Service Platform

## 📘 Overview

This project implements an **Emergency Service Subscription System** using the **STOMP (Simple Text Oriented Messaging Protocol)**.  
It allows clients to subscribe to specific emergency channels (e.g., fire, police, medical), report emergencies, and receive updates from others in real-time.

The system consists of:
- **Java Server** – implements a STOMP broker supporting both **Thread-Per-Client (TPC)** and **Reactor** architectures.
- **C++ Client** – enables users to log in, join or exit emergency channels, report events, and summarize reports.

---

## 🧩 Architecture

### **1. Server (Java)**
Implements a centralized STOMP server using one of two modes:
- **TPC (Thread-Per-Client)** – each client handled by a dedicated thread.
- **Reactor** – event-driven server using non-blocking I/O for scalability.

The server is generic and protocol-agnostic, supporting any `StompMessagingProtocol` implementation.

#### Main Components:
| File | Responsibility |
|------|----------------|
| `BaseServer.java` | Abstract class for shared server logic |
| `BlockingConnectionHandler.java` / `NonBlockingConnectionHandler.java` | Handle client connections |
| `Connections.java` / `ConnectionsImpl.java` | Manages active connections and broadcasts |
| `ConnectionHandler.java` | Interface for sending messages to clients |
| `StompMessagingProtocol.java` / `StompMessagingProtocolImpl.java` | Defines and implements STOMP frame logic |
| `MessageEncoderDecoder.java` / `MessageEncoderDecoderImpl.java` | Serializes and parses STOMP frames |
| `Server.java` / `Reactor.java` | Entry points for TPC and Reactor modes |
| `StompServer.java` | Main class — parses args and launches the chosen mode |

---

### **2. Client (C++)**
The client connects to the STOMP server and executes user commands through the console.

It uses **two threads**:
- One for reading from the keyboard (user input)
- One for reading from the socket (server responses)

Synchronization between threads is handled using `std::mutex`.

#### Main Components:
| File | Responsibility |
|------|----------------|
| `ConnectionHandler.cpp/h` | TCP socket communication |
| `StompMessageParser.cpp/h` | Handles parsing and building STOMP frames |
| `StompMessagingProtocolImpl.cpp/h` | Client-side logic for STOMP commands |
| `User.cpp/h` | Represents a logged-in user and their subscriptions |
| `ActorThreadPool.cpp/h` | Thread pool for managing parallel tasks |
| `ObjectEncoderDecoder.cpp/h` | Handles encoding/decoding message objects |

---

## 📡 STOMP Protocol

The STOMP protocol defines all communication between the client and server.  
Each frame follows this format:

```
COMMAND
header1:value1
header2:value2

body^@
```

### Supported Frames

**Client → Server**
- `CONNECT`
- `SEND`
- `SUBSCRIBE`
- `UNSUBSCRIBE`
- `DISCONNECT`

**Server → Client**
- `CONNECTED`
- `MESSAGE`
- `RECEIPT`
- `ERROR`

---

## 🧠 Client Commands

| Command | Description |
|----------|-------------|
| `login {host:port} {username} {password}` | Connects to the server. Sends a `CONNECT` frame. |
| `join {channel_name}` | Subscribes to an emergency channel (`SUBSCRIBE`). |
| `exit {channel_name}` | Unsubscribes from a channel (`UNSUBSCRIBE`). |
| `report {file}` | Parses a JSON file with emergency events and sends each as a `SEND` frame. |
| `summary {channel_name} {user} {file}` | Generates a summary file of reports from the specified user. |
| `logout` | Sends a `DISCONNECT` frame and gracefully closes the connection. |

---

## 🧾 Example Session

### 1️⃣ Login
```
login localhost:7777 meni films
```

**Frame sent:**
```
CONNECT
accept-version:1.2
host:stomp.cs.bgu.ac.il
login:meni
passcode:films

^@
```

**Frame received:**
```
CONNECTED
version:1.2
^@
```

Output:  
`Login successful`

---

### 2️⃣ Join a channel
```
join fire_dept
```
Output:  
`Joined channel fire_dept`

---

### 3️⃣ Report events
```
report events1_partial.json
```
Sends multiple `SEND` frames parsed from JSON.

---

### 4️⃣ Generate a summary
```
summary police meni report_summary.txt
```

**Example Output:**
```
Channel police
Stats:
Total: 2
active: 1
forces_arrival_at_scene: 1

Event Reports:
Report_1:
city: Liberty City
date time: 29/12/24 22:15
event name: Grand Theft Auto
summary: Pink Lampadati Felon with li...

Report_2:
city: Raccoon City
date time: 30/12/24 10:00
event name: Burglary
summary: Suspect broke into a residen...
```

---

## ⚙️ Running Instructions

### **Server**
Build and run with Maven:
```bash
mvn compile
mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="<port> <tpc|reactor>"
```

**Example:**
```bash
mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 tpc"
```

### **Client**
Build and run with Make:
```bash
make
./bin/StompEMIClient
```

---

## 🧪 Testing

- The provided `StompServer.jar` and `StompESClient.exe` can be used for reference and testing.
- You can test cross-communication between your implementation and the provided binaries to ensure protocol correctness.

---

## 💡 Design Highlights

- **Thread-Safe Architecture:** C++ client uses mutex locks to synchronize socket and keyboard threads.
- **Generic Interfaces:** Java server uses generics (`T`) to support reusable messaging protocols.
- **Extensible Protocol Layer:** STOMP logic is isolated, allowing future integration of additional message types.
- **Graceful Disconnection:** Implemented via `DISCONNECT` and `RECEIPT` handshake.

---

## 🧰 Tools & Technologies

| Component | Technology |
|------------|-------------|
| Server | Java 17 + Maven |
| Client | C++11 + Makefile |
| Communication | STOMP 1.2 over TCP |
| Concurrency | Java Threads / NIO Reactor, std::thread + mutex |

---


