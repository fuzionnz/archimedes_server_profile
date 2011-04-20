<?php

/**
 * Implementation of hook_fieldgroup_default_groups().
 */
function archimedes_server_fieldgroup_default_groups() {
  $groups = array();

  // Exported group: group_database
  $groups['application-group_database'] = array(
    'group_type' => 'standard',
    'type_name' => 'application',
    'group_name' => 'group_database',
    'label' => 'Database',
    'settings' => array(
      'form' => array(
        'style' => 'fieldset_collapsed',
        'description' => '',
      ),
      'display' => array(
        'weight' => '0',
        'label' => 'above',
        'teaser' => array(
          'format' => 'hidden',
          'exclude' => 0,
        ),
        'full' => array(
          'format' => 'fieldset_collapsible',
          'exclude' => 0,
        ),
        'description' => '',
        '4' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '2' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '3' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        'token' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
      ),
    ),
    'weight' => '0',
    'fields' => array(
      '0' => 'field_dbname',
      '1' => 'field_dbhost',
      '2' => 'field_db_size',
    ),
  );

  // Exported group: group_metrics
  $groups['application-group_metrics'] = array(
    'group_type' => 'standard',
    'type_name' => 'application',
    'group_name' => 'group_metrics',
    'label' => 'Web Server',
    'settings' => array(
      'form' => array(
        'style' => 'fieldset_collapsed',
        'description' => '',
      ),
      'display' => array(
        'weight' => '4',
        'label' => 'above',
        'teaser' => array(
          'format' => 'hidden',
          'exclude' => 0,
        ),
        'full' => array(
          'format' => 'fieldset_collapsible',
          'exclude' => 0,
        ),
        'description' => '',
        '4' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '2' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '3' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        'token' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
      ),
    ),
    'weight' => '4',
    'fields' => array(
      '0' => 'field_webserver',
      '1' => 'field_webroot',
      '2' => 'field_sitedata',
      '3' => 'field_sitesize',
    ),
  );

  // Exported group: group_reference
  $groups['application-group_reference'] = array(
    'group_type' => 'standard',
    'type_name' => 'application',
    'group_name' => 'group_reference',
    'label' => 'Reference',
    'settings' => array(
      'form' => array(
        'style' => 'fieldset_collapsed',
        'description' => '',
      ),
      'display' => array(
        'weight' => '5',
        'label' => 'above',
        'teaser' => array(
          'format' => 'hidden',
          'exclude' => 0,
        ),
        'full' => array(
          'format' => 'fieldset_collapsible',
          'exclude' => 0,
        ),
        'description' => '',
        '4' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '2' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '3' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        'token' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
      ),
    ),
    'weight' => '5',
    'fields' => array(
      '0' => 'field_unique_hash',
      '1' => 'field_common_hash',
    ),
  );

  // Exported group: group_metrics
  $groups['sla-group_metrics'] = array(
    'group_type' => 'standard',
    'type_name' => 'sla',
    'group_name' => 'group_metrics',
    'label' => 'Metrics',
    'settings' => array(
      'form' => array(
        'style' => 'fieldset_vertical_tab',
        'description' => '',
      ),
      'display' => array(
        'description' => '',
        'teaser' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        'full' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '4' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '2' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '3' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        'token' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        'label' => 'above',
      ),
    ),
    'weight' => '-2',
    'fields' => array(
      '0' => 'field_disk_space',
      '1' => 'field_sprt_hrs',
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Database');
  t('Metrics');
  t('Reference');
  t('Web Server');

  return $groups;
}
