; make file for archimedes_server_profile
core = 7.x
api = 2

; Include the definition for how to build Drupal core directly, including patches:
includes[] = drupal-org-core.make

; Profile.
projects[archimedes_server_profile][type] = profile
projects[archimedes_server_profile][download][type] = git
projects[archimedes_server_profile][download][url] = https://github.com/xurizaemon/archimedes_server_profile.git
