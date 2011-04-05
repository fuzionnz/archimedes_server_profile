<?php

!function_exists('profiler_v2') ? require_once('libraries/profiler/profiler.inc') : FALSE;
    profiler_v2('archimedes_server_profile');

# define('ARCHIMEDES_SERVER_THEME', 'archimedes');

#/**
# * Implementation of hook_profile_modules().
# */
#function archimedes_profile_modules() {
#  return array(
#    // Core
#    'color', 'comment', 'help', 'menu', 'taxonomy', 'dblog', 'search', 'path', 'update',
#
#    // Chaos Tools
#    'ctools', 'page_manager', 'panels', 'context', 'context_ui',
#
#    // Views
#    'views', 'views_ui', 'views_content', 'views_groupby',
#
#    // CCK
#    'content', 'fieldgroup', 'text', 'filefield', 'imagefield',
#    'optionwidgets', 'link', 'nodereference', 'nodereferrer',
#    'revisionreference', 'number', 'userreference',
#
#    // Date
#    'date_api', 'date_timezone', 'date',
#
#    // ImageAPI + ImageCache
#    'imageapi', 'imagecache', 'imageapi_gd',  'imagecache_profiles', 'imagecache_ui',
#    'imagecache_canvasactions', 'imagecache_coloractions',
#
#    // Strongarm
#    'strongarm',
#
#    // Style
#    'token', 'pathauto', 'charts_graphs', 'charts_graphs_google_charts', 'admin', 'admin_tools',
#
#    // Features
#    'features', 'archimedes_server', 'archimedes_drupal_site', 'archimedes_activity_stream',
#    'archimedes_server_ui',
#
#    // Archimedes Client
#    'archimedes',
#  );
#}


#function archimedes_profile_tasks(&$task, $url) {
#  archimedes_config_menu();
#  archimedes_config_panels();
#  archimedes_config_theme();
#   // Update the menu router information.
#  menu_rebuild();
#}
#
#/**
# * Add custom Menu links.
# */
#function archimedes_config_menu() {
#  $item = array('menu_name' => 'primary-links', 'weight' => -1, 'link_path' => '<front>', 'link_title' => t('Home'), 'module' => 'menu', 'expanded' => 0);
#  menu_link_save($item);
#}
#
#/**
# * Configure theme
# */
#function archimedes_config_theme() {
#  // Disable garland
#  db_query("UPDATE {system} SET status = 0 WHERE type = 'theme' and name = '%s'", 'garland');
#
#  // Enable Archimedes theme
#  db_query("UPDATE {system} SET status = 1 WHERE type = 'theme' and name = '%s'", ARCHIMEDES_SERVER_THEME);
#
#  // Set Archimedes theme as the default
#  variable_set('theme_default', ARCHIMEDES_SERVER_THEME);
#  $settings = variable_get('theme_' . ARCHIMEDES_SERVER_THEME . '_settings', array());
#  variable_set('theme_' . ARCHIMEDES_SERVER_THEME . '_settings', $settings);
#
#  // Insert blocks into regions
#  $block = array(
#    'bid' => 7,
#    'module' => 'menu',
#    'delta' => 'primary-links',
#    'theme' => ARCHIMEDES_SERVER_THEME,
#    'status' => 1,
#    'weight' => 0,
#    'region' => 'header_menu',
#    'custom' => 0,
#    'throttle' => 0,
#    'visibility' => 0,
#    'pages' => '',
#    'title' => '',
#    'cache' => -1,
#  );
#  drupal_write_record('blocks', $block);
#
#  // Refresh registry
#  list_themes(TRUE);
#
#  drupal_rebuild_theme_registry();
#}
#
#function archimedes_config_panels() {
#  ctools_include('node_view', 'page_manager', 'plugins/tasks');
#  $page = page_manager_cache_load('node_view');
#  page_manager_node_view_enable($page, FALSE);
#}
#
#/**
# * Implementation of hook_form_alter().
# *
# * Allows the profile to alter the site-configuration form. This is
# * called through custom invocation, so $form_state is not populated.
# */
#function archimedes_form_alter(&$form, $form_state, $form_id) {
#  if ($form_id == 'install_configure') {
#    // Set default for site name field.
#    $form['site_information']['site_name']['#default_value'] = t('Archimedes Monitoring');
#  }
#}
