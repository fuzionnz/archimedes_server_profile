<?php

/**
 * Implementation of hook_context_default_contexts().
 */
function archimedes_server_ui_context_default_contexts() {
  $export = array();
  $context = new stdClass;
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'servers_breadcrumb';
  $context->description = '';
  $context->tag = '';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'host' => 'host',
      ),
      'options' => array(
        'node_form' => '1',
      ),
    ),
  );
  $context->reactions = array(
    'breadcrumb' => 'servers',
  );
  $context->condition_mode = 0;

  $export['servers_breadcrumb'] = $context;
  $context = new stdClass;
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'site_breadcrumb';
  $context->description = '';
  $context->tag = '';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'drupal' => 'drupal',
      ),
      'options' => array(
        'node_form' => '1',
      ),
    ),
  );
  $context->reactions = array(
    'breadcrumb' => 'applications',
  );
  $context->condition_mode = 0;

  $export['site_breadcrumb'] = $context;
  return $export;
}
