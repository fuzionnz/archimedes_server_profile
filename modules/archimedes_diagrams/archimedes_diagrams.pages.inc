<?php

/**
 * Generate a page for system architecture diagram.
 */
function archimedes_diagrams_system_architecture($node) {
  $title = t('!environment System Architecture', array(
    '!environment' => $node->field_site_env[0]['value'],
  ));

  $struct = '<ul id="struct-' . $node->nid . '">';

  $struct .= '<li class="webservers" title="Webservers">';
  foreach ($node->field_webserver as $field) {
    $server = node_load($field['nid']);
    $struct .= theme('server_icon', $server, 'web');
  }
  $struct .= '<ul><li class="databases" title="Databases">';
  foreach ($node->field_dbhost as $field) {
    $server = node_load($field['nid']);
    $struct .= theme('server_icon', $server, 'database');
  }
  $struct .= '</li></ul>';
  $struct .= '</li>';

  $struct .= '</ul>';

  drupal_add_js(array(
    'archimedes_diagrams' => array(
      'servers' => $js_settings,
      'path' => drupal_get_path('module', 'archimedes_diagrams'),
      'canvas' => 'syscanvas-' . $node->nid,
      'struct' => 'struct-' . $node->nid,
    )
  ), 'setting');

  
  drupal_add_js(drupal_get_path('module', 'archimedes_diagrams') . '/js/jquery.orgchart.js');
  drupal_add_js(drupal_get_path('module', 'archimedes_diagrams') . '/js/system-architecture.js');
  drupal_add_css(drupal_get_path('module', 'archimedes_diagrams') . '/css/jquery.orgchart.css');

  return '<h2>' . $title . '</h2><div id="syscanvas-' . $node->nid . '"></div>' . $struct;
}
