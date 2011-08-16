<?php

function archimedes_git_show_commit($node, $hash) {
  drupal_add_js(drupal_get_path('module', 'archimedes_git') . '/syntaxhighlighter/scripts/shCore.js');
  drupal_add_js(drupal_get_path('module', 'archimedes_git') . '/syntaxhighlighter/scripts/shBrushDiff.js');
  drupal_add_css(drupal_get_path('module', 'archimedes_git') . '/syntaxhighlighter/styles/shCore.css');
  drupal_add_css(drupal_get_path('module', 'archimedes_git') . '/syntaxhighlighter/styles/shThemeDefault.css');
  drupal_add_js('SyntaxHighlighter.all();', 'inline', 'footer');
  
  $repo = variable_get('archimedes_git_repository_root', '/git') . '/' . $node->archimedes_git;
  exec(sprintf('cd %s && git show %s', $repo, $hash), $output);
  return '<pre class="brush: diff">' . implode(PHP_EOL, $output) . '</pre>';
}
