# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = 'OpenTelemetry C++'
copyright = '2021, OpenTelemetry authors'
author = 'OpenTelemetry authors'

# The full version, including alpha/beta/rc tags
release = '1.0.0-rc2'

# Run sphinx on subprojects and copy output
# -----------------------------------------
# This is necessary so the readthedocs build works. It doesn't invoke the
# Makefile, but just runs sphinx on this conf.py.
import os
import shutil
import subprocess

apidir = os.path.join('..', '..', 'api', 'docs')
subprocess.call(['make', 'html'], cwd=apidir)
targetdir = os.path.join(os.getcwd(), 'otel_api')
sourcedir = os.path.join(apidir, 'otel_api')
if os.path.exists(targetdir):
  shutil.rmtree(targetdir)
os.makedirs(targetdir)

# Now copying the previously created APi documentation. Table of contents
# are filtered out, because those don't go well together with the furo theme.
for fname in os.listdir(sourcedir):
  with open(os.path.join(sourcedir, fname), 'r') as fin:
    with open(os.path.join(targetdir, fname), 'w') as fout:
      skip = False
      for line in fin:
        if line.startswith('.. contents'):
          skip = True
        elif not line.startswith(' '):
          skip = False
        if not skip:
          fout.write(line)

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe",
]

breathe_projects = {
        "OpenTelemetry C++ API": "../../api/docs/doxyoutput/xml"
}

breathe_default_project = "OpenTelemetry C++ API"

primary_domain = "cpp"

higlight_language = "cpp"


# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "furo"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']
