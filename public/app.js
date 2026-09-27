const query = document.querySelector('#query');
const results = document.querySelector('#results');
const resultTitle = document.querySelector('#result-title');
const resultCount = document.querySelector('#result-count');
const correction = document.querySelector('#correction');
let searchMode = false;

async function load(path) {
  const response = await fetch(path);
  return response.json();
}
function render(items) {
  resultCount.textContent = `${items.length} match${items.length === 1 ? '' : 'es'}`;
  results.innerHTML = items.length ? items.map((item, index) => `<article class="result" style="animation-delay:${index * 35}ms"><div><div class="result-name">${item.word}</div><div class="result-meta">${item.category}</div></div><span class="score">${item.popularity}% relevance</span></article>`).join('') : '<div class="empty"><span>∅</span><p>No matching terms yet</p><small>Try a broader prefix or search term</small></div>';
}
async function update() {
  const value = query.value.trim();
  correction.hidden = true;
  if (!value) {
    resultTitle.textContent = 'Suggestions';
    render((await load('api/suggestions?q=')).suggestions);
    return;
  }
  const data = await load(`api/suggestions?q=${encodeURIComponent(value)}`);
  if (searchMode) {
    resultTitle.textContent = 'Search matches';
    render((await load(`api/search?q=${encodeURIComponent(value)}`)).results);
  } else {
    resultTitle.textContent = 'Suggestions';
    render(data.suggestions);
  }
  const fix = await load(`api/autocorrect?q=${encodeURIComponent(value)}`);
  if (fix.correction.toLowerCase() !== value.toLowerCase()) { correction.hidden = false; correction.innerHTML = `Did you mean <strong>${fix.correction}</strong>?`; }
}
query.addEventListener('input', update);
query.addEventListener('keydown', event => { if (event.key === 'Enter') { searchMode = true; update(); } });
query.addEventListener('input', () => { searchMode = false; });
document.querySelector('#clear').addEventListener('click', () => { searchMode = false; query.value = ''; update(); query.focus(); });
document.querySelectorAll('[data-query]').forEach(button => button.addEventListener('click', () => { searchMode = false; query.value = button.dataset.query; update(); query.focus(); }));
load('api/stats').then(data => document.querySelector('#word-count').textContent = data.words);
