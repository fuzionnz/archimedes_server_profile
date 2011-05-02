<?php

function _archimedes_drupal_site_project_status(&$node) {
  list($core, $version) = explode('-', $node->field_mod_version[0]['value']);

  $project = array(
    'name' => $node->field_name[0]['value'],
    'info' => array(
      'version' => $node->field_mod_version[0]['value'],
      'project' => $node->field_dru_proj[0]['value'],
      'package' => $node->field_dru_pkg[0]['value'],
      'core' => $core,
      'name' => $node->field_name[0]['value']
    ),
    'type' => substr($node->type, 7),
  );

  $cid = $project['info']['project'] . $project['info']['version'];

  if ($cache = cache_get($cid, 'cache_archimedes_drupal')) {
    return $cache->data[$project['name']]['releases'][$project['info']['version']];
  }

  $url = _archimedes_drupal_site_build_fetch_url($project);
  $xml = drupal_http_request($url);
  if (!isset($xml->data)) {
    return FALSE;
  }

  $data[] = $xml->data;

  module_load_include('inc', 'update', 'update.fetch');

  $parser = new update_xml_parser;
  $available = $parser->parse($data);
  cache_set($cid, $available, 'cache_archimedes_drupal', strtotime('+1 day'));
  return $available[$project['name']]['releases'][$project['info']['version']];

}

/**
 * Generates the URL to fetch information about project updates.
 *
 * This figures out the right URL to use, based on the project's .info file
 * and the global defaults. Appends optional query arguments when the site is
 * configured to report usage stats.
 *
 * @param $project
 *   The array of project information from update_get_projects().
 * @param $site_key
 *   The anonymous site key hash (optional).
 *
 * @see update_refresh()
 * @see update_get_projects()
 */
function _archimedes_drupal_site_build_fetch_url($project) {
  global $base_url;
  $site_key = md5($base_url . drupal_get_private_key());
  $name = $project['info']['project'];
  $url = isset($project['info']['project status url']) ? $project['info']['project status url'] : variable_get('update_fetch_url', UPDATE_DEFAULT_URL);
  $url .= '/'. $name .'/'. $project['info']['core'];
  return $url;
}
