<?php

#function archimedes_drupal_site_modules($node) {
#  module_load_include('inc', 'archimedes_drupal_site', 'archimedes_drupal_site.update');
#  module_load_include('inc', 'archimedes_drupal_site', 'archimedes_drupal_site.theme');
#  module_load_include('inc', 'update', 'update.report');
#  if ($node->type == 'drupal') {
#    $header = array('Module', 'Version', 'Latest Version');
#    $mod_vids = $node->field_drupal_mod;
#    $theme_vids = $node->field_drupal_theme;
#    $modules = array();
#    $available_mod = array();
#    //first process modules
#    foreach ($mod_vids as $vid) {
#      $vid = $vid['vid'];
#      if($vid != null)  {
#        $module = node_load(array('type' => 'drupal_module'),$vid);
#        $modules[] = $module;
#        $cached_data = archimedes_drupal_site_archimedes_cache_get($module);
#        $available[$cached_data->data['short_name']] = $cached_data->data;
#      }
#    }
#    //then process themes
#    foreach ($theme_vids as $vid) {
#      $vid = $vid['vid'];
#      if($vid != null)  {
#        $theme = node_load(array('type' => 'drupal_theme'),$vid);
#        $modules[] = $theme;
#        $cached_data = archimedes_drupal_site_archimedes_cache_get($theme);
#        $available[$cached_data->data['short_name']] = $cached_data->data;
#      }
#    }
#    $projects = archimedes_drupal_site_calculate_project_data($available, $modules);
#    return $projects;
#  }
#}


/**
 * Find module project updates
 */
function archimedes_drupal_site_project_updates() {
  $key = db_result(db_query("SELECT cid FROM {cache_archimedes_drupal} ORDER BY created ASC"));
  $data = cache_get($key,'cache_archimedes_drupal');
  archimedes_drupal_site_archimedes_cache_set($data->data['release_url'],$key,$data->data['short_name']);
}


/**
 * Update cache with data from module release URL
 */
function archimedes_drupal_site_archimedes_cache_set($url,$key,$project) {
  module_load_include('inc', 'update', 'update.fetch');

  $data = array();

  $xml = drupal_http_request($url);
  if(isset($xml->data)) {
    $data[] = $xml->data;
    $parser = new update_xml_parser;
    $available = $parser->parse($data);
    if (!empty($available) && is_array($available[$project])) {
      $available[$project]['release_url'] = $url;
      cache_set($key,$available[$project],'cache_archimedes_drupal');
      return TRUE;
    }
  }
  return FALSE;
}

/**
  * Hook into the archimedes_site hook (for drupal_alter in update function)
  *
  * Check whether any projects associated with a site ($node) contain security vulnerabilities.
  * @author Adam Bramley
  */
function archimedes_drupal_site_check_modules($node){
  $roles = variable_get('archimedes_notifications', array());
  $security_roles = $roles['archimedes_security_roles'];
  $revisions = node_revision_list($node);
  //take the 2nd element in the array
  next($revisions);
  if ($key = key($revisions))  {
    $previous_node = node_load($node->nid, $key);
  }
  else  {
    //no previous revision to compare to so just return
    return;
  }
  $projects = archimedes_drupal_site_modules($node);
  $body = array();

  foreach ($projects as $project)  {
      $proj_name = $project['info']['name'];
      $proj_version = $project['info']['version'];
      $proj_recommend = $project['recommended'];
    if ($project['status'] == 3) {
      $body[] = "<li>The $proj_name project is at version $proj_version which is <span style=\"color:red; font-weight:bold\">no longer supported</span>, and needs to be updated. The recommended version is $proj_recommend</li>";

    }
    elseif ($project['status'] == 1)  {
      $body[] = "<li>The $proj_name project is at version $proj_version which contains a <span style=\"color:red; font-weight:bold\">security vulnerability</span>, and needs to be updated. The recommended version is $proj_recommend</li>";
    }
  }
  if (!empty($body))  {
    $header = "<h4>Security Updates for $node->title</h4><ul>";
    array_unshift($body, $header);
    $users = $node->field_users;
    $emails = archimedes_observer_get_role_emails($users, $security_roles);
    if (!empty($emails)) {
      $from = variable_get('archimedes_server_email','archimedes@catalyst.net.nz');
      $message = array(
        'to' => implode(', ', $emails),
        'subject' => "Security Updates for $node->title",
        'body' => implode("", $body) . "</ul>",
        'headers' => array(
          'MIME-Version' => '1.0',
          'Content-Type' => 'text/html; charset=UTF-8; format=flowed',
          'Content-Transfer-Encoding' => '8Bit',
          'X-Mailer' => 'Drupal',
          'From' => $from
        ),
      );
      if (module_exists('messaging')) {
        $send = messaging_message_send($emails, $message, variable_get('messaging_default_method', 'mail'));
      }
      else{
        $send = drupal_mail_send($message);
      }
    }
  }
}

/**
 * Find the current status of a given project.
 */
function archimedes_drupal_site_project_status($project, $version) {
  $cid = $project . $version;
  if (!$data = cache_get($cid, 'cache_archimedes_drupal')) {
    array(
      'name' => $project,
      'info' => array(
        'name' => $cached_data->data['title'],
        'package' => $module->field_dru_pkg[0]['value'],
        'version' => $version,
        'project' => $project,
        'datestamp' => $module->created,
      ),
      'project_type' => 'module',
    );

  }
}
