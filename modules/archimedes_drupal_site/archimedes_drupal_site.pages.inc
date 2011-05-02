<?php

/**
 * Build a list of modules for a givien report.
 */
function archimedes_drupal_site_modules($node, $vid = FALSE) {
  if ($vid) {
    $node = node_load($node, $vid);
  }
  $rows = array();
  $packages = array();
  foreach ($node->field_drupal_mod as $field) {
    $module = node_load(array('vid' => $field['vid']));
    $project = !empty($module->field_dru_pkg[0]['value']) ? $module->field_dru_pkg[0]['value'] : 'Other';
    $packages[$project][] = $module;
  }

  module_load_include('inc', 'archimedes_drupal_site');

  ksort($packages);
  foreach ($packages as $pkg => $modules) {
    $rows[] = array(array('data' => $pkg, 'header' => TRUE, 'colspan' => 3));
    foreach ($modules as $module) {
      $rows[] = array(l($module->title, $module->path), $module->field_mod_version[0]['value'], $module->available['status']);
    }
  }

  return theme('table', array('Module', 'Version', 'Status'), $rows);
}
