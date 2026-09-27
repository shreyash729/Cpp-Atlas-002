# Atlas Search

Atlas Search is a lightweight product search workspace for discovering relevant content quickly. It demonstrates autocomplete, ranked search suggestions, spell checking, and autocorrect through a simple live interface backed by a fast in-memory search index.

## The Four Bugs

The challenge contains four user-visible behavior bugs. Reproduce each one through the live application and compare the result with the expected behavior below.

### 1. Prefix autocomplete is ranked in the wrong order

**Reproduce it:**

1. Open the live preview.
2. Type `auto` in the **Try a query** field.
3. Do not press Enter; this is the autocomplete view.

**Visible buggy result:** The three matching suggestions appear in ascending relevance order. You may see `autocomplete api` at `82%`, `autocorrect` at `96%`, and `autocomplete` at `98%`.

**Expected result after the fix:** The same three prefix matches remain, but the highest relevance appears first: `autocomplete` (`98%`), `autocorrect` (`96%`), then `autocomplete api` (`82%`).

### 2. Full search only checks the beginning of a phrase

**Reproduce it:**

1. Enter `search` in the **Try a query** field.
2. Press Enter to switch from autocomplete to full search.

**Visible buggy result:** The preview shows only one result, `search suggestions` (`94%`). It misses `binary search`, even though the word `search` appears in that phrase.

**Expected result after the fix:** The preview finds the query anywhere in an indexed phrase and shows both `search suggestions` and `binary search`, ranked by relevance.

### 3. Spell autocorrect does not recover a close typo

**Reproduce it:**

1. Type the misspelled word `autocorect` exactly. It has only one `r` after `o`.
2. Watch the live preview.

Do not test this bug with `autocorrect`; that is already the correctly spelled indexed word.

**Visible buggy result:** The query remains `autocorect` and no useful `Did you mean` correction appears.

**Expected result after the fix:** The preview keeps the original query visible but shows a correction suggestion: `Did you mean autocorrect?`

### 4. Empty-query suggestions ignore the result limit

**Reproduce it:**

1. Click the `x` control at the end of the search field, or delete all query text.
2. Observe the suggestions view.

**Visible buggy result:** The preview displays all `20 matches`, creating an unnecessarily long unbounded list.

**Expected result after the fix:** Empty input remains safe and useful, but the preview shows only the configured number of suggestions, ranked consistently by relevance. It should not render the entire catalog at once.

## Expected Behaviour After Fixing All Bugs

- Typing a prefix immediately returns matching autocomplete suggestions.
- Suggestions are ranked by relevance and popularity.
- Search finds matching terms anywhere in the indexed phrase.
- Close spelling mistakes produce a useful autocorrect suggestion.
- Empty input is handled safely and respects the configured result limit.