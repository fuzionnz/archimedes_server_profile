Archimedes Server Profile
=========================

Archimedes is a passive application monitoring server built on Drupal but designed to facilitate any web based application. Archimedes achieves passive application monitoring by installing an Archimedes client on your web hosted application. In Drupal's case, it's installing the Archimedes Client module.

The Client collects up information about the application and sends it back to the server where the server reads it in and creates or updates the site node that represents that client on the server.

Archimedes currently collects these metrics

* Hostname
* Servername
* Site Data size
* Site Root size
* Database details
* Database size
* Site instance Metrics
* Environment (Production, Staging, Development)
* Associated users
* Git hash
* Git remotes
* Unique Identifier
* Software specific
 * Modules, themes, etc
 * Features

Some of the features of Archimedes
----------------------------------

 * HTTP or SMTP communication with the server. Permits Archimedes to monitor sites behind firewalls without requiring firewall exemptions.
 * RSA key encryption. All data collected by clients is encrypted in transit with the server's public RSA key.

Installation
------------

Clone this profile repo, then run `drush make` with the file `build-archimedes.make` from within it.

    drush make path/to/archimedes_server_profile/build-archimedes.make /path/to/archimedes.example.org

@TODO: _It should be possible to do this by pointing `drush make` at
the URL for a raw .make file?_

This will build your Archimedes Server codebase. Now you can run
Drupal installation by visiting the site or from the CLI using `drush
site-install`

When using the web installer, you will be prompted also for the
"Server Domain" - this is the URL which your Archimedes client sites
will report status to if submitting over HTTP.

To install via CLI, set the site URL using `site_information.archimedes_server_url_domain` -

    drush -y site-install archimedes_server_profile \
      --db-url=mysql://test_archimedes:password@localhost/test_archimedes \
      --account-mail=admin@example.org \
      --site-name='Archimedes' \
      -l https://test-archimedes.fudev.co.nz \
      site_information.archimedes_server_url_domain=https://test-archimedes.fudev.co.nz

Setting up client sites
-----------------------

You're done! OK, you're not, but your Archimedes server should have
reported in its own initial module status to itself, so you have a
taste ready to go.

To install additional client sites, add the Archimedes library and
module to the site, and configure it by visiting the Archimedes
settings screen (Reports > Archimedes > Settings).

Here you'll need to copy the settings from your Archimedes server to
your client. For HTTP submission, you need the _Submission URL_ and
the _Server Public Key_.

@TODO: _Document how to submit over email, currently unexplored._

Managing Updates
----------------

When a site is configured, it will push its reports to the Archimedes
Server daily. The Archimedes Server processes these reports and
imports them to new nodes/node revisions when cron runs. Because your
reports depend on cron happening on two sites, you may see some delays
if you don't configure cron to account for this.
