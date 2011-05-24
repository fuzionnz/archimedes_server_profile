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
 * Load an application node from its unique hash
 */
function archimedes_server_load_application($id) {
  $field = content_fields('field_unique_hash', 'application');
  $db_info = content_database_info($field);
  $nid = db_result(db_query("SELECT type.nid FROM {content_type_application} type LEFT JOIN {".$db_info['table']."} hash ON type.vid=hash.vid WHERE hash.field_unique_hash_value = '%s' LIMIT 1", $id));
  if ($nid) {
    return node_load($nid);
  }
  return FALSE;
}

/**
 * Process an XML document into a site report.
 */
function archimedes_server_process_document($xml) {
  module_load_include('inc', 'archimedes_server');
  // create blank node object
  $options = variable_get('node_options_application', array());
  $node = (object) array(
    'type' => 'application',
    'status' => in_array('status', $options),
    'promote' => in_array('promote', $options),
    'sticky' => in_array('sticky', $options),
    'created' => time(),
    'comment' => variable_get('comment_application', 2),
    'uid' => ARCHIMEDES_SYSTEM_UID,
  );

  $dom = new DOMDocument();
  $dom->loadXML($xml);

  // fail if $dom->documentElement not set
  if (!isset($dom->documentElement)) {
    return;
  }

  $vocab = archimedes_server_get_vocabulary(ARCHIMEDES_APP_CATEGORIES);

  // Get unique identifier and check if in system
  $node->field_unique_hash = array(array('value' => $dom->documentElement->getAttribute('id')));

  $datetime = $dom->documentElement->getAttribute('datetime');

  if ($existing_node = archimedes_server_load_application($node->field_unique_hash[0]['value'])) {
    $node = $existing_node;
    // Make sure we create a new revision rather than overwriting the existing one.
    // This is so we can track changes over a period of time.
    $node->revision = TRUE;
  }

  archimedes_server_set_term($node, $dom->documentElement->getAttribute('type'), $vocab);

  // Grab the field information about the application content type.
  // This allows us to dynamically expand the application content
  // type without having to write code to handle the expansion.
  $node_type = content_types('application');

  // Attempt to parse the XML into a node. Any failure to parse
  // will throw an exception in which case we will not try to
  // save the node.
  try {
    foreach($dom->getElementsByTagName('field') as $item) {
      $fid = $item->getAttribute('id');
      // non cck fields
      if ($fid == 'title' || $fid == 'body') {
        $node->{$fid} = trim($item->nodeValue);
        continue;
      }

      $values = array();

      // Field handlers process the data provided by the document
      // and return the nessacary meta data to the node to store.
      $handler = archimedes_server_field_handler($fid);
      foreach ($item->getElementsByTagName('value') as $value) {

        $plugins = array();
        // Plugins are attributes added to the element that may
        // provide more functional meta data about the element.
        // for example, the data maybe refering to another node
        // such as a Host. In which case, this element may use a
        // namespaced attribute set to describe the referenced
        // host node.
        foreach ($value->attributes as $v) {
          $plugin = strtr($v->namespaceURI, array('monitor-plugin:' => ''));
          $plugins[$plugin] = $v->namespaceURI;
        }
        $handler->setPlugins($plugins);
        $values[] = $handler->process($value);
      }

      // Attach values to node field.
      $node->{$fid} = $values;
    }

    // Check that there isn't already a screenshot and that
    // a new one can be generated.
    if (!isset($node->field_screenshot[0]) && isset($node->field_servername[0]['url'])) {
      if ($field = archimedes_server_snap_screenshot($node->field_servername[0]['url'])) {
        $node->field_screenshot = array(0 => $field);
      }
    }

    // Record the time of report import.
    $update = explode('+', date('c',strtotime($datetime))); // convert back to UTC timezone
    $node->field_last_update = array(0 => array('value' => $update[0]));

    // allow other modules to modify node before save
    drupal_alter('archimedes_site', $node, 'pre');

    // save now so hooks have access to the nid if needed
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
 * Load a vocabulary based on machine_name.
 */
function archimedes_server_get_vocabulary($machine_name) {
  static $vocabs;
  if (!isset($vocabs)) {
    module_load_include('inc', 'features', 'includes/features.taxonomy');
    $vocabs = _taxonomy_features_get_vocabularies();
  }
  return $vocabs[$machine_name];
}

/**
 * Take a screenshot of a website if its accessible from Archimedes Server.
 */
function archimedes_server_snap_screenshot($url) {
  if (!archimedes_server_check_webkit()) {
    return;
  }

  $urlinfo = parse_url($url);
  // Create a filepath name that is url compliant as it will be requested
  // over HTTP later.
  $filename = urlencode(strtr($urlinfo['host'] . urldecode($urlinfo['path']), array('/' => '-', '.' => '', ' ' => '')));
  $filepath = file_directory_temp() . '/' . $filename;
  $files_directory = archimedes_find_root() . '/' . file_directory_path();
  $cmd = "python " . dirname(__FILE__) . "/python-webkit2png/webkit2png.py --log=" . $files_directory . "/webkit2png.log -x 1024 768 -g 1024 768 -o $filepath -w 1 %s";
  archimedes_server_shell_exec($cmd, $url);
  if (!file_exists($filepath)) {
    watchdog("Archimedes", "Server failed to take screenshot of site %url", array("%url" => $node->field_servername[0]['url']), WATCHDOG_ERROR);
    return;
  } 
    
  $max_width = 800;
  $max_height = 600;

  // If the max width and height is set on the field, use that as the resolution limitaion.
  $field = content_fields('field_screenshot', 'application');
  if (!in_array($field['widget']['max_resolution'], array('0', '')) && (strpos($field['widget']['max_resolution'], 'x') !== FALSE)) {
    list($max_width, $max_height) = explode('x', $field['widget']['max_resolution']);
  }
  
  if (module_exists('imageapi') && imageapi_default_toolkit()) {
    $res = imageapi_image_open($filepath);
    imageapi_image_scale($res, $max_width, $max_height);
    imageapi_image_close($res);
  } 
  // Try to resize the image to fit the dimensions.
  elseif (image_get_toolkit()) {
    image_scale($filepath, $filepath, $max_width, $max_height);
  }

  // Save the image and generate data for population to the field.
  return field_file_save_file($filepath, array(), file_directory_path());
}

/**
 * Retrive a handler for handling an unknown field.
 *
 * Archimedes Server itself doesn't use this because it relies on
 * on CCK and only provides global application meta data. Plugins
 * on the otherhand have application specific data handling capabilities
 * and will use hook_archimedes_field_handlers and the handler class
 * to handle data appropriately.
 */
function archimedes_server_field_handler($fid) {
  static $handlers;
  if (empty($handlers)) {
    foreach (module_implements('archimedes_field_handlers') as $module) {
      foreach (module_invoke($module, 'archimedes_field_handlers') as $fieldname => $handler) {
        $handler['module'] = $module;
        $handlers[$fieldname] = $handler;
      }
    }
  }
  if (isset($handlers[$fid])) {
    if (isset($handlers[$fid]['file'])) {
      include_once drupal_get_path('module', $handlers[$fid]['module']) . '/' . $handlers[$fid]['file'];
    }
    $handler = new $handlers[$fid]['handler'];
    return $handler;
  }
  return new ArchimedesServer_Handler();
}

/**
 * Load a plugin from a module and parse the XML node.
 */
function archimedes_server_plugin_invoke($plugin, $value, $ns) {
  $plugins = archimedes_server_get_plugins();
  if (!$module = $plugins[$plugin]) {
    return FALSE;
  }
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
 * Set a term to a node
 */
function archimedes_server_set_term(&$node, $name, $vocab) {
  $rs = db_query("SELECT t.tid, t.* FROM {term_data} t WHERE LOWER(t.name) = LOWER('%s') AND t.vid = %d", trim($name), $vocab->vid);
  $term_type = db_fetch_object($rs) ? db_fetch_object($rs) : NULL;
  if (isset($term_type) && !isset($node->taxonomy[$term_type->tid])) {
    $node->taxonomy[$term_type->tid] = $term_type;
  }
  elseif (!isset($term_type)) {
    $form_values = array(
      'name' => $name,
      'vid' => $vocab->vid,
    );
    taxonomy_save_term($form_values);
    $node->taxonomy[$form_values['tid']] = taxonomy_get_term($form_values['tid']);
  }
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

/**
 * Definiation of an Archimedes Server Handler.
 */
class ArchimedesServer_Handler {
  
  protected $plugins;

  public function setPlugins($plugins) {
    $this->plugins = $plugins;
  }

  public function process($value) {
    $return = $this->processPlugins($value);
    if ($url = parse_url($value->nodeValue)) {
      switch ($url['scheme']){
        case 'http':
        case 'https':
        case 'ftp':
        case 'ftps':
          $return['url'] = $value->nodeValue;
        default:
          $return['value'] = $url['path'];
        break;
      }
    }
    return $return;
  } 

  protected function processPlugins($value) {
    $plugins = array();
    // Generate a list of plugins used on this value.
    foreach ($value->attributes as $v) {
      $plugin = strtr($v->namespaceURI, array('monitor-plugin:' => ''));
      $plugins[$plugin] = $v->namespaceURI;
    }
    $plugins = array_filter($plugins);

    // Invoke each plugin.
    $return = array();
    foreach ($plugins as $plugin => $ns) {
      $result = archimedes_server_plugin_invoke($plugin, $value, $ns);
      $return = array_merge($return, $result);
    }
    return $return;
  }
}
