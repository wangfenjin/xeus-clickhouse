#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import subprocess

on_rtd = os.environ.get('READTHEDOCS', None) == 'True'

if on_rtd:
    subprocess.call('cd ..; doxygen', shell=True)

import sphinx_rtd_theme

html_theme = "sphinx_rtd_theme"

html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]

def setup(app):
    app.add_stylesheet("main_stylesheet.css")

extensions = ['breathe']
breathe_projects = { 'xeus-sqlite': '../xml' }
templates_path = ['_templates']
source_suffix = '.rst'
master_doc = 'index'
project = 'xeus-sqlite'
copyright = '2020, Mariana Meireles'
author = 'Mariana Meireles'

html_logo = 'quantstack-white.svg'

exclude_patterns = []
pygments_style = 'sphinx'
todo_include_todos = False
htmlhelp_basename = 'xeus-sqlitedoc'
