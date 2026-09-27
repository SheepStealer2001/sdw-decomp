@echo off
rem The level editor (tools\level_editor.py) in the browser: it opens the page, and stops once the page has been closed.
rem Python 3 must be installed (python.org); pyw runs it without a console window.
where pyw >nul 2>nul && (start "" pyw -3 "%~dp0tools\level_editor.py" --app) || (start "" pythonw "%~dp0tools\level_editor.py" --app)
