# Atlas Search

## Description

Atlas Search is a C++17 product-discovery application that helps users find relevant resources through instant autocomplete, ranked search results, and typo recovery. It runs as a lightweight HTTP service with a browser interface and an in-memory search catalog. The implementation uses a prefix index internally, but the user experience is presented as a normal search product.

## Repository Structure

```text
applications/
├── CMakeLists.txt          Build configuration for the server and tests
├── challenge.json          Isolated runtime, port, build, start, and test configuration
├── start.sh                Build, launch, and live-rebuild watcher entrypoint
├── include/
│   └── service.hpp         Search service API and index data structures
├── src/
│   ├── main.cpp            HTTP server, routing, static assets, and JSON responses
│   └── service.cpp         Catalog data, autocomplete, search, ranking, and autocorrect logic
├── public/
│   ├── index.html          Atlas Search page structure and user-facing copy
│   ├── style.css           Responsive visual design
│   └── app.js               Live query handling, API calls, and result rendering
├── tests/
│   ├── run_tests.sh        Incremental test rebuild and JSON test runner
│   └── test_service.cpp    Four behavioral challenge tests
└── README.md               Candidate-facing application and bug reproduction guide
```

## Bugs and Bug Locations

These are the four behavioral bug surfaces covered by the challenge. The named locations identify the owning implementation areas for debugging and review.

### 1. Prefix autocomplete is incomplete or incorrectly ranked

- **Bug location:** `src/service.cpp`, `TrieService::autocomplete`
- **How to observe it:** Enter `auto` in the live search field.
- **Failure:** Matching terms may be missing, unrelated, or returned in the wrong relevance order.
- **Expected:** Prefix matches include `autocomplete`, `autocorrect`, and `autocomplete api`, ranked by popularity with the strongest result first.

### 2. Search suggestions do not match the full query

- **Bug location:** `src/service.cpp`, `TrieService::search`
- **How to observe it:** Enter `search` in the live search field.
- **Failure:** The application may only match the beginning of a phrase, omit valid terms, or return unstable ordering.
- **Expected:** Search matches the query anywhere in an indexed phrase and returns both relevant search entries in descending relevance order.

### 3. Spell autocorrect fails to recover a close typo

- **Bug location:** `src/service.cpp`, `TrieService::autocorrect`; correction display is surfaced by `public/app.js`
- **How to observe it:** Enter `autocorect` in the live search field.
- **Failure:** The typo may remain unchanged, produce an unrelated correction, or fail to show a correction notice.
- **Expected:** The interface suggests `autocorrect` as the intended term while preserving the user's original query until they choose to use the correction.

### 4. Empty queries do not respect the result limit

- **Bug location:** `src/service.cpp`, empty-prefix result handling; `public/app.js`, empty-query rendering
- **How to observe it:** Clear the search field with the clear control, then inspect the suggestions state.
- **Failure:** Empty input may produce no useful state, too many results, or an unstable result set.
- **Expected:** Empty input is handled safely, the UI remains usable, and any broad suggestion result is bounded by the configured limit and ranked consistently.

## Expected Behaviour After Fixing All Bugs

- Typing a prefix immediately shows only matching autocomplete suggestions.
- Results are ranked consistently by popularity and relevance.
- Search matches terms containing the query, not only terms beginning with it.
- A close spelling mistake produces a useful correction suggestion.
- Empty input does not crash, hang, or create an unbounded result list.