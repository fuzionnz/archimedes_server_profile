<?php

function archimedes_drupal_site_get_project(&$node) {
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
    return $cache->data;
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
  $project = archimedes_drupal_site_calculate_project_data($available[$project['info']['project']], $project);
  cache_set($cid, $project, 'cache_archimedes_drupal', strtotime('+1 day'));
  return $project;

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

/**
 * Given the installed projects and the available release data retrieved from
 * remote servers, calculate the current status.
 *
 * This function is the heart of the update status feature. It iterates over
 * every currently installed project. For each one, it first checks if the
 * project has been flagged with a special status like "unsupported" or
 * "insecure", or if the project node itself has been unpublished. In any of
 * those cases, the project is marked with an error and the next project is
 * considered.
 *
 * If the project itself is valid, the function decides what major release
 * series to consider. The project defines what the currently supported major
 * versions are for each version of core, so the first step is to make sure
 * the current version is still supported. If so, that's the target version.
 * If the current version is unsupported, the project maintainer's recommended
 * major version is used. There's also a check to make sure that this function
 * never recommends an earlier release than the currently installed major
 * version.
 *
 * Given a target major version, it scans the available releases looking for
 * the specific release to recommend (avoiding beta releases and development
 * snapshots if possible). This is complicated to describe, but an example
 * will help clarify. For the target major version, find the highest patch
 * level. If there is a release at that patch level with no extra ("beta",
 * etc), then we recommend the release at that patch level with the most
 * recent release date. If every release at that patch level has extra (only
 * betas), then recommend the latest release from the previous patch
 * level. For example:
 *
 * 1.6-bugfix <-- recommended version because 1.6 already exists.
 * 1.6
 *
 * or
 *
 * 1.6-beta
 * 1.5 <-- recommended version because no 1.6 exists.
 * 1.4
 *
 * It also looks for the latest release from the same major version, even a
 * beta release, to display to the user as the "Latest version" option.
 * Additionally, it finds the latest official release from any higher major
 * versions that have been released to provide a set of "Also available"
 * options.
 *
 * Finally, and most importantly, it keeps scanning the release history until
 * it gets to the currently installed release, searching for anything marked
 * as a security update. If any security updates have been found between the
 * recommended release and the installed version, all of the releases that
 * included a security fix are recorded so that the site administrator can be
 * warned their site is insecure, and links pointing to the release notes for
 * each security update can be included (which, in turn, will link to the
 * official security announcements for each vulnerability).
 *
 * This function relies on the fact that the .xml release history data comes
 * sorted based on major version and patch level, then finally by release date
 * if there are multiple releases such as betas from the same major.patch
 * version (e.g. 5.x-1.5-beta1, 5.x-1.5-beta2, and 5.x-1.5). Development
 * snapshots for a given major version are always listed last.
 *
 * The results of this function are expensive to compute, especially on sites
 * with lots of modules or themes, since it involves a lot of comparisons and
 * other operations. Therefore, we cache the results into the {cache_update}
 * table using the 'update_project_data' cache ID. However, since this is not
 * the data about available updates fetched from the network, it is ok to
 * invalidate it somewhat quickly. If we keep this data for very long, site
 * administrators are more likely to see incorrect results if they upgrade to
 * a newer version of a module or theme but do not visit certain pages that
 * automatically clear this cache.
 *
 * @param $available
 *  Array of data about available project releases.
 *
 * @see update_get_available()
 * @see update_get_projects()
 * @see update_process_project_info()
 * @see update_project_cache()
 */
function archimedes_drupal_site_calculate_project_data($project, $installed) {
  // If the project status is marked as something bad, there's nothing
  // else to consider.
  if (isset($project['project_status'])) {
    switch ($project['project_status']) {
      case 'insecure':
        $project['status'] = UPDATE_NOT_SECURE;
        if (empty($project['extra'])) {
          $project['extra'] = array();
        }
        $project['extra'][] = array(
          'class' => 'project-not-secure',
          'label' => t('Project not secure'),
          'data' => t('This project has been labeled insecure by the Drupal security team, and is no longer available for download. Immediately disabling everything included by this project is strongly recommended!'),
        );
        break;
      case 'unpublished':
      case 'revoked':
        $project['status'] = UPDATE_REVOKED;
        if (empty($project['extra'])) {
          $project['extra'] = array();
        }
        $project['extra'][] = array(
          'class' => 'project-revoked',
          'label' => t('Project revoked'),
          'data' => t('This project has been revoked, and is no longer available for download. Disabling everything included by this project is strongly recommended!'),
        );
        break;
      case 'unsupported':
        $project['status'] = UPDATE_NOT_SUPPORTED;
        if (empty($project['extra'])) {
          $project['extra'] = array();
        }
        $project['extra'][] = array(
          'class' => 'project-not-supported',
          'label' => t('Project not supported'),
          'data' => t('This project is no longer supported, and is no longer available for download. Disabling everything included by this project is strongly recommended!'),
        );
        break;
      case 'not-fetched':
        $project['status'] = UPDATE_NOT_FETCHED;
        $project['reason'] = t('Failed to fetch available update data');
        break;

      default:
        // Assume anything else (e.g. 'published') is valid and we should
        // perform the rest of the logic in this function.
        break;
    }
  }

  // Figure out the target major version.
  if (preg_match('/^(\d+\.x-)?(\d+)\..*$/', $installed['info']['version'], $matches)) {
    $installed['info']['existing_major'] = $matches[2];
  }

  if (isset($installed['info']['version'])) {
    $installed['info']['install_type'] = 'official';
    // Check for development snapshots
    if (preg_match('@(dev|HEAD)@', $installed['info']['version'])) {
      $installed['info']['install_type'] = 'dev';
    }
    
    // Figure out what the currently installed major version is. We need
    // to handle both contribution (e.g. "5.x-1.3", major = 1) and core
    // (e.g. "5.1", major = 5) version strings.
    $matches = array();
    if (preg_match('/^(\d+\.x-)?(\d+)\..*$/', $installed['info']['version'], $matches)) {
      $installed['info']['major'] = $matches[2];
    }
    elseif (!isset($installed['info']['major'])) {
      // This would only happen for version strings that don't follow the
      // drupal.org convention. We let contribs define "major" in their
      // .info in this case, and only if that's missing would we hit this.
      $installed['info']['major'] = -1;
    }
  }
  else {
    // No version info available at all.
    $installed['info']['install_type'] = 'unknown';
    $installed['info']['version'] = t('Unknown');
    $installed['info']['major'] = -1;
  }

  $supported_majors = array();
  if (isset($project['supported_majors'])) {
    $supported_majors = explode(',', $project['supported_majors']);
  }
  elseif (isset($project['default_major'])) {
    // Older release history XML file without supported or recommended.
    $supported_majors[] = $project['default_major'];
  }

  if (in_array($installed['info']['existing_major'], $supported_majors)) {
    // Still supported, stay at the current major version.
    $target_major = $installed['info']['existing_major'];
  }
  elseif (isset($project['recommended_major'])) {
    // Since 'recommended_major' is defined, we know this is the new XML
    // format. Therefore, we know the current release is unsupported since
    // its major version was not in the 'supported_majors' list. We should
    // find the best release from the recommended major version.
    $target_major = $project['recommended_major'];
    $project['status'] = UPDATE_NOT_SUPPORTED;
  }
  elseif (isset($project['default_major'])) {
    // Older release history XML file without recommended, so recommend
    // the currently defined "default_major" version.
    $target_major = $project['default_major'];
  }
  else {
    // Malformed XML file? Stick with the current version.
    $target_major = $installed['info']['existing_major'];
  }

  // Make sure we never tell the admin to downgrade. If we recommended an
  // earlier version than the one they're running, they'd face an
  // impossible data migration problem, since Drupal never supports a DB
  // downgrade path. In the unfortunate case that what they're running is
  // unsupported, and there's nothing newer for them to upgrade to, we
  // can't print out a "Recommended version", but just have to tell them
  // what they have is unsupported and let them figure it out.
  $target_major = max($installed['info']['existing_major'], $target_major);

  $version_patch_changed = '';
  $patch = '';

  // Defend ourselves from XML history files that contain no releases.
  if (empty($project['releases'])) {
    $project['status'] = UPDATE_UNKNOWN;
    $project['reason'] = t('No available releases found');
    return $project;
  }

  foreach ($project['releases'] as $version => $release) {
    // First, if this is the existing release, check a few conditions.
    if ($installed['info']['version'] === $version) {
      if (isset($release['terms']['Release type']) &&
          in_array('Insecure', $release['terms']['Release type'])) {
        $project['status'] = UPDATE_NOT_SECURE;
      }
      elseif ($release['status'] == 'unpublished') {
        $project['status'] = UPDATE_REVOKED;
        if (empty($project['extra'])) {
          $project['extra'] = array();
        }
        $project['extra'][] = array(
          'class' => 'release-revoked',
          'label' => t('Release revoked'),
          'data' => t('Your currently installed release has been revoked, and is no longer available for download. Disabling everything included in this release or upgrading is strongly recommended!'),
        );
      }
      elseif (isset($release['terms']['Release type']) &&
              in_array('Unsupported', $release['terms']['Release type'])) {
        $project['status'] = UPDATE_NOT_SUPPORTED;
        if (empty($project['extra'])) {
          $project['extra'] = array();
        }
        $project['extra'][] = array(
          'class' => 'release-not-supported',
          'label' => t('Release not supported'),
          'data' => t('Your currently installed release is now unsupported, and is no longer available for download. Disabling everything included in this release or upgrading is strongly recommended!'),
        );
      }
    }

    // Otherwise, ignore unpublished, insecure, or unsupported releases.
    if ($release['status'] == 'unpublished' ||
        (isset($release['terms']['Release type']) &&
         (in_array('Insecure', $release['terms']['Release type']) ||
          in_array('Unsupported', $release['terms']['Release type'])))) {
      return $project;
    }

    // See if this is a higher major version than our target and yet still
    // supported. If so, record it as an "Also available" release.
    if ($release['version_major'] > $target_major) {
      if (in_array($release['version_major'], $supported_majors)) {
        if (!isset($project['also'])) {
          $project['also'] = array();
        }
        if (!isset($project['also'][$release['version_major']])) {
          $project['also'][$release['version_major']] = $version;
        }
      }
      // Otherwise, this release can't matter to us, since it's neither
      // from the release series we're currently using nor the recommended
      // release. We don't even care about security updates for this
      // branch, since if a project maintainer puts out a security release
      // at a higher major version and not at the lower major version,
      // they must remove the lower version from the supported major
      // versions at the same time, in which case we won't hit this code.
      continue;
    }

    // Look for the 'latest version' if we haven't found it yet. Latest is
    // defined as the most recent version for the target major version.
    if (!isset($project['latest_version'])
        && $release['version_major'] == $target_major) {
      $project['latest_version'] = $version;
    }

    // Look for the development snapshot release for this branch.
    if (!isset($project['dev_version'])
        && $release['version_major'] == $target_major
        && isset($release['version_extra'])
        && $release['version_extra'] == 'dev') {
      $project['dev_version'] = $version;
    }

    // Look for the 'recommended' version if we haven't found it yet (see
    // phpdoc at the top of this function for the definition).
    if (!isset($project['recommended'])
        && $release['version_major'] == $target_major
        && isset($release['version_patch'])) {
      if ($patch != $release['version_patch']) {
        $patch = $release['version_patch'];
        $version_patch_changed = $release['version'];
      }
      if (empty($release['version_extra']) && $patch == $release['version_patch']) {
        $project['recommended'] = $version_patch_changed;
      }
    }

    // Stop searching once we hit the currently installed version.
    if ($installed['info']['version'] === $version) {
      break;
    }

    // If we're running a dev snapshot and have a timestamp, stop
    // searching for security updates once we hit an official release
    // older than what we've got. Allow 100 seconds of leeway to handle
    // differences between the datestamp in the .info file and the
    // timestamp of the tarball itself (which are usually off by 1 or 2
    // seconds) so that we don't flag that as a new release.
    if ($installed['info']['install_type'] == 'dev') {
      if (empty($project['datestamp'])) {
        // We don't have current timestamp info, so we can't know.
        continue;
      }
      elseif (isset($release['date']) && ($project['datestamp'] + 100 > $release['date'])) {
        // We're newer than this, so we can skip it.
        continue;
      }
    }

    // See if this release is a security update.
    if (isset($release['terms']['Release type'])
        && in_array('Security update', $release['terms']['Release type'])) {
      $project['security updates'][] = $release;
    }
  } // foreach ($project['releases'] as $release)

  // If we were unable to find a recommended version, then make the latest
  // version the recommended version if possible.
  if (!isset($project['recommended']) && isset($project['latest_version'])) {
    $project['recommended'] = $project['latest_version'];
  }

  // Stash the info about available releases into our $project array.
  $project += $project;

  //
  // Check to see if we need an update or not.
  //

  if (!empty($project['security updates'])) {
    // If we found security updates, that always trumps any other status.
    $project['status'] = UPDATE_NOT_SECURE;
  }

  if (isset($project['status'])) {
    // If we already know the status, we're done.
    return $project;
  }

  // If we don't know what to recommend, there's nothing we can report.
  // Bail out early.
  if (!isset($project['recommended'])) {
    $project['status'] = UPDATE_UNKNOWN;
    $project['reason'] = t('No available releases found');
    return $project;
  }

  // If we're running a dev snapshot, compare the date of the dev snapshot
  // with the latest official version, and record the absolute latest in
  // 'latest_dev' so we can correctly decide if there's a newer release
  // than our current snapshot.
  if ($installed['info']['install_type'] == 'dev') {
    if (isset($project['dev_version']) && $project['releases'][$project['dev_version']]['date'] > $project['releases'][$project['latest_version']]['date']) {
      $project['latest_dev'] = $project['dev_version'];
    }
    else {
      $project['latest_dev'] = $project['latest_version'];
    }
  }

  // Figure out the status, based on what we've seen and the install type.
  switch ($installed['info']['install_type']) {
    case 'official':
      if ($installed['info']['version'] === $project['recommended'] || $installed['info']['version'] === $project['latest_version']) {
        $project['status'] = UPDATE_CURRENT;
      }
      else {
        $project['status'] = UPDATE_NOT_CURRENT;
      }
      break;

    case 'dev':
      $latest = $project['releases'][$project['latest_dev']];
      if (empty($project['datestamp'])) {
        $project['status'] = UPDATE_NOT_CHECKED;
        $project['reason'] = t('Unknown release date');
      }
      elseif (($project['datestamp'] + 100 > $latest['date'])) {
        $project['status'] = UPDATE_CURRENT;
      }
      else {
        $project['status'] = UPDATE_NOT_CURRENT;
      }
      break;

    default:
      $project['status'] = UPDATE_UNKNOWN;
      $project['reason'] = t('Invalid info');
  }
  return $project;
}
