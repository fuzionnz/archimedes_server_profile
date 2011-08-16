<?php

/**
 * Build a list of modules for a givien report.
 */
function archimedes_drupal_site_modules($node, $vid = FALSE) {
  if ($vid) {
    $node = node_load($node, $vid);
  }

  $output = '';

  foreach (archimedes_drupal_site_load_references($node) as $type => $refs) {
    $sort = array();
    foreach ($refs as $ref) {
      if ($project = node_load(array('vid' => $ref['vid']))) {
        if (empty($project->field_dru_pkg[0]['value'])) {
          $project->field_dru_pkg[0]['value'] = 'Other';
        }
        $sort[$project->field_dru_pkg[0]['value']][$project->title] = $project;
      }
    }
  }
  $security_warning = FALSE;
  ksort($sort);
  foreach ($sort as $package => $projects) {
    ksort($projects);
    $rows = array();
    $collapse = TRUE;
    foreach ($projects as $project_name => $project) {
        $status = array('%status' => 'unknown');
        $row = array(
          'title ' => l($project->title, 'node/' . $project->nid . '/revisions/' . $project->vid), 
          'version' => $project->field_mod_version[0]['value'], 
          'status' => t('%status', $status),
          'tags' => 'Custom',
          'link' => '',
        );
        if (isset($project->upstream_status)) {
          switch ($project->upstream_status) {
            case UPDATE_NOT_SECURE:
              $collapse = FALSE;
              $status = array('%status' => 'Unsecure');
              $security_warning = TRUE;
              break;
            case UPDATE_REVOKED:
              $collapse = FALSE;
              $status = array('%status' => 'Revoked');
              $security_warning = TRUE;
              break;
            case UPDATE_NOT_SUPPORTED:
              $status = array('%status' => 'Unsupported');
              break;
            case UPDATE_NOT_CURRENT:
              $status = array('%status' => 'Updates Available');
              break;
            case UPDATE_CURRENT:
              $status = array('%status' => 'Current');
              break;
            case UPDATE_NOT_CHECKED:
            default:
              break;
          }
          $row['status'] = t('%status', $status);
        }
        if (isset($project->release['download_link'])) {
          $row['version'] = l($project->field_mod_version[0]['value'], $project->release['download_link']);
        }
        if (isset($project->release['terms']['Release type'])) {
          $row['tags'] = implode(', ', $project->release['terms']['Release type']);
        }
        if (!empty($project->link)) {
          $row['link'] = l('Project Page', $project->link);
        }
        $rows[] = array('data' => $row, 'class' => strtolower($status['%status']));
    }
    $element = array(
      '#title' => $package,
      '#value' => theme('table', array('Project', 'Version', 'Status', 'Tags', 'Link'), $rows),
      '#collapsible' => TRUE,
      '#collapsed' => $collapse,
    );
    $output .= theme('fieldset', $element);
  }
  if ($security_warning) {
    drupal_set_message(t('Some projects in this Drupal application are insecure. These project should be updated and deployed immediately.'), 'warning');
  }
  drupal_add_css(drupal_get_path('module', 'archimedes_drupal_site') . '/archimedes_drupal_site.css');

  return $output;
}
