<?php

/**
 * Format bytes to a readable size.
 */
function archimedes_server_format_bytes($size) {
    $units = array(' B', ' KB', ' MB', ' GB', ' TB');
    for ($i = 0; $size >= 1024 && $i < 4; $i++) $size /= 1024;
    return round($size, 2).$units[$i];
}

/**
 * Decrypt data provided the data and ekey.
 */
function archimedes_server_decrypt($data, $ekey) {
  if (!$prikey = openssl_pkey_get_private(variable_get('archimedes_key_private', FALSE))) {
    return FALSE;
  }
  openssl_open($data,$output,$ekey, $prikey);
  openssl_free_key($prikey);
  return $output;
}

/**
 * Process an XML document into a site report.
 */
function archimedes_server_process_document($xml) {
  module_load_include('inc', 'archimedes_server');
  // create blank node object
  $node = new stdClass();

  $dom = new DOMDocument();
  $dom->loadXML($xml);

  // fail if $dom->documentElement not set
  if (!isset($dom->documentElement)) {
    return;
  }

  // Get unique identifier and check if in system
  $id = $dom->documentElement->getAttribute('id');
  $type = $dom->documentElement->getAttribute('type');
  $author = $dom->documentElement->getAttribute('author');
  $datetime = $dom->documentElement->getAttribute('datetime');

  // ignore if content type does not exist
  if (!$node_type = content_types($type)) {
    watchdog('Archimedes', "Archimedes doesn't not currently support $type.", array(), WATCHDOG_ERROR);
    return;
  }

  // The user may not yet exist so create the user
  // but do not activate the user.
  $uid = archimedes_server_user_save($author);

  // check if there is a node with unique id already
  $field = content_fields('field_unique_hash', $type);
  $db_info = content_database_info($field);
  if ($nid = db_result(db_query(sprintf("SELECT type.nid FROM {content_type_%s} type LEFT JOIN {".$db_info['table']."} hash ON type.vid=hash.vid WHERE hash.field_unique_hash_value = '%s' LIMIT 1", $type, $id)))) {
    $node = node_load(array('nid' => $nid));
    $node->revision = TRUE;
  }
  else {
    $node->type = $node_type['type'];
    $node->created = time();
    $node->status = 1; //published
    $node->promote = 0;
    $node->sticky = 0;
    $node->uid = $uid;
    $node->comment = variable_get('comment_' . $type, 2);
    $node->field_unique_hash = array(0 => array('value' => $id));
  }

  // Attempt to parse the XML into a node. Any failure to parse
  // will throw an exception in which case we will not try to
  // save the node.
  try {
    foreach($dom->getElementsByTagName('field') as $item) {
      $fid = $item->getAttribute('id');
      // non cck fields
      if ($fid == 'title' || $fid == 'body') {
        $node->{$fid} = trim($item->nodeValue);
        return;
      }

      // If content type has no record of field id
      // then do not attempt to save it.
      if (!$field = $node_type['fields'][$fid]) {
        return;
      }

      $values = array();
      foreach ($item->getElementsByTagName('value') as $value) {
        // Check for plugins being used.
        $plugins = array();
        foreach ($value->attributes as $v) {
          $plugins[$v->namespaceURI] = strtr($v->namespaceURI, array('monitor-plugin:' => ''));
        }
        $values[] = archimedes_server_run_plugins($plugins, $field, $value);
      }

      //attach values to node field
      $node->{$fid} = $values;
    }

    // collect screenshot of website if not already saved.
    if (archimedes_server_check_webkit() && !isset($node->field_thumbnail[0]) && isset($node->field_servername[0]['url'])) {
      try {
        $filename = preg_replace("/[^a-zA-Z0-9]/", "", $node->title) . '.png';
        $filepath = file_directory_temp() . '/' . $filename;
        $files_directory = archimedes_find_root() . '/' . file_directory_path();
        $cmd = "python " . dirname(__FILE__) . "/python-webkit2png/webkit2png.py --log=" . $files_directory . "/webkit2png.log -x 1024 768 -g 1024 768 -o $filepath -w 1 %s";
        archimedes_server_shell_exec($cmd, $node->field_servername[0]['url']);
        if (!file_exists($filepath)) {
          watchdog("Archimedes", "Server failed to take screenshot of site %url", array("%url" => $node->field_servername[0]['url']), WATCHDOG_ERROR);
          // Exit early.
          throw new Exception();
        }

        $max_width = 800;
        $max_height = 600;

        if (module_exists('imageapi') && imageapi_default_toolkit()) {
          $res = imageapi_image_open($filepath);
          imageapi_image_scale($res, $max_width, $max_height);
          imageapi_image_close($res);
        }
        // Try to resize the image to fit the dimensions.
        elseif (image_get_toolkit()) {
          image_scale($filepath, $filepath, $max_width, $max_height);
        }

        $field = field_file_save_file($filepath, array(),file_directory_path().'/thumbs');

        // Generate imagecache thumbnail
        $presetname = 'SiteThumb';
        if ($preset = imagecache_preset_by_name($presetname)) {
          $dst = imagecache_create_path($presetname, $field['filepath']);
          if (!file_exists($dst)) {
            imagecache_build_derivative($preset['actions'], $field['filepath'], $dst);
          }
        }

        $node->field_thumbnail = array(0 => $field);
      }
      catch (Exception $e) {}
    }

    // Record the time of report import.
    $update = explode('+', date('c',strtotime($datetime))); // convert back to UTC timezone
    $node->field_last_update = array(0 => array('value' => $update[0]));

    // save now so hooks have access to the nid if needed
    node_save($node);
    $node->revision = FALSE;

    // allow other modules to modify node before final save
    drupal_alter('archimedes_site', $node, 'pre');

    // save node after hooks
    node_save($node);

    // allow other modules to modify node after final save
    drupal_alter('archimedes_site', $node, 'post');

  }
  catch (Exception $e) {
    watchdog('Archimedes', "Cannot import report into node: %reason", array("%reason" => $e->getMessage()), WATCHDOG_ERROR);
    return FALSE;
  }
  return TRUE;
}

/**
 * Parse xml values through any defined plugins
 */
function archimedes_server_run_plugins($plugins, $field, $value) {
  $module_plugins = archimedes_server_get_plugins();
  $nValue = strtr($value->nodeValue, array(
    'file://' => '',
    'mailto:' => '',
  ));
  $return = array();
  switch($field['module']) {
    case 'link':
      $return['url'] = $nValue;
      break;
    default:
      $return['value'] = $nValue;
  }
  foreach ($plugins as $ns => $plugin) {
    $result = archimedes_server_plugin_invoke($module_plugins[$plugin], $plugin, $value, $ns);
    $return = array_merge($return, $result);
  }
  return $return;
}

function archimedes_server_plugin_invoke($module, $plugin, $value, $ns) {
  module_load_include('inc', $module, $module . '.plugins');
  $class = 'ArchimedesServerPlugin_' . str_replace('-', '_', $plugin);
  if (class_exists($class)) {
    $parser = new $class($ns);
    return $parser->parse($value);
  }
  return array();
}

/**
 * Get (and generate if required) list of plugins from all modules
 */
function archimedes_server_get_plugins() {
  static $plugins;
  if (!empty($plugins)) {
    return $plugins;
  }
  foreach (module_implements('archimedes_server_plugins') as $module) {
    foreach (module_invoke($module, 'archimedes_server_plugins') as $plugin) {
      $plugins[$plugin] = $module;
    }
  }
  return $plugins;
}

/**
 * Utility function to return uid from mail address and create user if required.
 */
function archimedes_server_user_save($mail) {
  $uid = '';
  if (substr($mail,0,7) == 'mailto:') {
    $mail = substr($mail,7);
  }
  if($user = user_load(array('mail' => $mail))) {
    $uid = $user->uid;
  } else {

    $user_data = array(
      'mail' => $mail,
      'name' => $mail,
      'init' => $mail,
      'status' => 0,
    );

    // If LDAP Integration is enabled. Try find a user
    // in LDAP that may have not had there account provisioned
    // yet. If we find one, provision there account for them.
    if (module_exists('ldapauth')) {
      _ldapauth_init(db_result(db_query("SELECT sid FROM {ldapauth} WHERE status = 1 ORDER BY weight")));
      global $_ldapauth_ldap;
       // If there is no bindn and bindpw - the connect will be an anonymous connect.
      $_ldapauth_ldap->connect($_ldapauth_ldap->getOption('binddn'), $_ldapauth_ldap->getOption('bindpw'));
      foreach (explode("\r\n", $_ldapauth_ldap->getOption('basedn')) as $base_dn) {
        if (empty($base_dn))
          continue;

        $name_attr = $_ldapauth_ldap->getOption('mail_attr') ? $_ldapauth_ldap->getOption('mail_attr') : LDAPAUTH_DEFAULT_USER_ATTR;
        $filter = $name_attr .'='. $mail;
        $result = $_ldapauth_ldap->search($base_dn, $filter);
        if (!$result)
          continue;

        $num_matches = $result['count'];
        // Must find exactly one user for authentication to.
        if ($num_matches != 1) {
          watchdog('ldapauth', "Error: %num_matches users found with $%filter under %base_dn.", array('%num_matches' => $num_matches, '%filter' => $filter, '%base_dn' => $base_dn), WATCHDOG_ERROR);
          continue;
        }
        $match = $result[0];
        break;
      }
      if (isset($match) && $match) {
        $user_data = array(
          'name' => $match['cn'][0],
          'pass' => user_password(20),
          'mail' => $mail,
          'init' => $mail,
          'status' => 1,
          'authname_ldapauth' => $match['cn'][0],
          'ldap_authentified' => TRUE,
          'ldap_dn' => $match['dn'],
          'ldap_config' => $_ldapauth_ldap->getOption('sid'),
        );
        $user = (object) $user_data;
        user_save($user, $user_data);
        $user = user_load(array('mail' => $mail));
        $uid = $user->uid;
        return $uid;
      }
    }
  }
  return 0;
}

/**
 * Definition of an Archimedes Server Plugin.
 */
abstract class ArchimedesServerPlugin {

  /**
   * XML Namespace.
   */
  protected $ns;

  public function __construct ($ns) {
    $this->ns = $ns;
  }

  /**
   * Parse a DOMElement value and return the CCK key values to save the value.
   */
  abstract public function parse($value);

}

