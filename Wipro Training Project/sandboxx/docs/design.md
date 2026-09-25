# SandBoxX Design Document

## Design Principles

- **Zero External Library Dependencies**: Built purely with C++17 standard library and Linux kernel APIs.
- **Fail-Safe Security**: Enforces no-new-privileges and capability dropping before executing user code.
- **Configurable Profiles**: Supports JSON configuration profiles (`default.json`, `restricted.json`, `development.json`) for flexible operational policies.
- **Observable Execution**: Tracks CPU percentage, RSS memory, and process thread count via `/proc` filesystem parsing.
