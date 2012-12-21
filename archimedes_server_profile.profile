<?php
/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Allows the profile to alter the site configuration form.
 */
function system_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the server name.
  $form['site_information']['site_name']['#default_value'] = 'Archimedes Server';

  $form['site_information']['archimedes_server_url_domain'] = array(
    '#title' => st('Server Domain'),
    '#type' => 'textfield',
    '#description' => st('This should be the domain Archimedes server will be accessed under which is usually the domain you\'re viewing this page on.'),
    '#default_value' => isset($form_state['values']['archimedes_server_url_domain']) ? $form_state['values']['archimedes_server_url_domain'] : $_SERVER['HTTP_HOST'],
    '#field_prefix' => (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS']) ? 'https://' : 'http://',
    '#field_suffix' => base_path() . 'archimedes/update',
  );
  $form['#submit'][] = 'system_form_install_configure_archimedes_submit';
}

/**
 * Submission handler for system install form.
 */
function system_form_install_configure_archimedes_submit(&$form, $form_state) {
  $scheme = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS']) ? 'https://' : 'http://';
  variable_set('archimedes_server_url', $scheme . $form_state['values']['archimedes_server_url_domain'] . base_path() . 'archimedes/update');
}



/**
 * Implements hook_form_alter().
 */
function system_form_install_select_profile_form_alter(&$form, $form_state) {
  // select archimedes_server_profile install profile by default
  foreach ($form['profile'] as $key => $element) {
    $form['profile'][$key]['#value'] = 'archimedes_server_profile';
  }
}
