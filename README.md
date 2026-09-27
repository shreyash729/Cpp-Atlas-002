# Atlas Search

Atlas Search is a lightweight product search workspace for discovering relevant content quickly. It demonstrates autocomplete, ranked search suggestions, spell checking, and autocorrect through a simple live interface backed by a fast in-memory search index.

## The Four Bugs

The challenge contains four user-visible behavior bugs. Reproduce each one through the live application and compare the result with the expected behavior below.

### 1. Prefix autocomplete is incomplete or incorrectly ranked

In the **Try a query** field, enter `auto`.

The application should show the strongest prefix matches first, including `autocomplete` and `autocorrect`. A bug may cause valid terms to be missing, return unrelated terms, or appear in the wrong popularity order.

### 2. Search suggestions do not match the full query

Enter `search` in the query field, or choose a suggested query chip when available.

The results should include terms containing the query, not only terms that begin with it. A bug may return too few results, miss `search suggestions`, or produce inconsistent ordering.

### 3. Spell autocorrect fails to recover a close typo

Enter the misspelled query `autocorect`.

The interface should recognize the nearby intended term and display a correction suggestion for `autocorrect`. A bug may leave the typo unchanged, suggest an unrelated term, or fail to display the correction notice.

### 4. Empty queries do not respect the result limit

Clear the query with the `x` control, then observe the suggestions state or enter a broad empty-prefix search if exposed by the running challenge.

The search experience should handle an empty query safely and return a bounded, popularity-ranked set of suggestions. A bug may return no suggestions, too many results, or an unstable result order.

## Expected Behaviour After Fixing All Bugs

- Typing a prefix immediately returns matching autocomplete suggestions.
- Suggestions are ranked by relevance and popularity.
- Search finds matching terms anywhere in the indexed phrase.
- Close spelling mistakes produce a useful autocorrect suggestion.
- Empty input is handled safely and respects the configured result limit.
- The interface updates without a manual server restart after candidate code changes.
- The live application remains available on port `8080` and works through the preview path.