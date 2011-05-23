; Set core and API.
core = "6.x"
api = 2

; Get Profiler module
libraries[profiler][download][type] = "get"
libraries[profiler][download][url] = "http://ftp.drupal.org/files/projects/profiler-6.x-2.0-beta2.tar.gz"

; Contrib Modules
projects[admin][subdir] = "contrib"
projects[admin_tools][subdir] = "contrib"
projects[advanced_help][subdir] = "contrib"
projects[cck][subdir] = "contrib"
projects[charts_graphs][subdir] = "contrib"
projects[context][subdir] = "contrib"
projects[ctools][subdir] = "contrib"
projects[date][subdir] = "contrib"
projects[environment][subdir] = "contrib"
projects[environment][version] = "1.0-alpha1"
projects[environment][patch][] = http://drupal.org/files/issues/1130374-proper-fomatted-patch.patch
projects[features][subdir] = "contrib"
projects[filefield][subdir] = "contrib"
projects[imageapi][subdir] = "contrib"
projects[imagecache][subdir] = "contrib"
projects[imagecache_actions][subdir] = "contrib"
projects[imagecache_profiles][subdir] = "contrib"
projects[imagefield][subdir] = "contrib"
projects[job_scheduler][subdir] = "contrib"
projects[lightbox2][subdir] = "contrib"
projects[link][subdir] = "contrib"
projects[nodeaccess][subdir] = "contrib"
projects[nodereferrer][subdir] = "contrib"
projects[panels][subdir] = "contrib"
projects[panels_tabs][subdir] = "contrib"
projects[pathauto][subdir] = "contrib"
projects[revisionreference][subdir] = "contrib"
projects[strongarm][subdir] = "contrib"
projects[tabs][subdir] = "contrib"
projects[tagging][subdir] = "contrib"
projects[token][subdir] = "contrib"
projects[views][subdir] = "contrib"
projects[views_groupby][subdir] = "contrib"

; Archimedes Client Module
projects[archimedes][type] = module
projects[archimedes][download][type] = "git"
projects[archimedes][download][url] = http://git.drupal.org/sandbox/fiasco/1116808.git 
projects[archimedes][download][revision] = "drupal-6"
projects[archimedes][subdir] = "custom"

; Archimedes Server Module
projects[archimedes_server][type] = module
projects[archimedes_server][download][type] = "git"
projects[archimedes_server][download][url] = http://git.drupal.org/sandbox/fiasco/1116748.git
projects[archimedes_server][download][revision] = "master"
projects[archimedes_server][subdir] = "custom"

; Archimedes Client Library
libraries[archimedes][download][type] = "file"
libraries[archimedes][download][url] = https://github.com/fiasco/Archimedes-Library/raw/master/php/archimedes.class.php

;Includes
includes[openatrium] = ../openatrium/openatrium.make
