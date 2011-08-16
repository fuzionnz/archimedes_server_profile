<?php

/**
 * Implementation of hook_taxonomy_default_vocabularies().
 */
function archimedes_server_taxonomy_default_vocabularies() {
  return array(
    'app_categories' => array(
      'name' => 'Application Categorization',
      'description' => '',
      'help' => '',
      'relations' => '1',
      'hierarchy' => '1',
      'multiple' => '1',
      'required' => '0',
      'tags' => '0',
      'module' => 'features_app_categories',
      'weight' => '0',
      'nodes' => array(
        'application' => 'application',
      ),
    ),
  );
}
