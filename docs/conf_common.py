from esp_docs.conf_docs import *  # noqa: F403,F401

languages = ['en']
idf_targets = ['esp32']

extensions += ['sphinx_copybutton',
               'sphinxcontrib.wavedrom',
               ]

# link roles config
github_repo = 'ramprasadajay/esp-ota-update'

# context used by sphinx_idf_theme
html_context['github_user'] = 'ramprasadajay'
html_context['github_repo'] = 'esp-ota-update'

html_static_path = ['../_static']

# Extra options required by sphinx_idf_theme
project_slug = 'esp-ota-update'

# Contains info used for constructing target and version selector
# Can also be hosted externally, see esp-idf for example
versions_url = './_static/docs_version.js'

# Final PDF filename will contains target and version
pdf_file_prefix = u'esp-ota-update'
