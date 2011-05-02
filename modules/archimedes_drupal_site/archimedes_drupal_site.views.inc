<?php

/**
 * Implementation of hook_views_data().
 */
function archimedes_drupal_site_views_data() {
  $data = array();
  $data['archimedes_drupal_modules']['table'] = array(
    'group' => 'Archimedes',
    'title' => 'Archimedes Drupal Modules',
    'help' => 'Drupal modules related to the site report',
    'join' => array(
      'node_revisions' => array(
        'left_field' => 'vid',
        'field' => 'cvid',
      ),
      'app_node' => array(
        'left_field' => 'vid',
        'field' => 'vid',
        'table' => 'node',
      ),
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
    ),
  );
  $data['archimedes_drupal_modules']['nid'] = array(
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    // Information for accepting a nid as an argument
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    // Information for accepting a nid as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    // Information for sorting on a nid.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  return $data;
}
