# Documentation Migration: MkDocs → Sphinx

This document describes the migration from MkDocs to Sphinx for PlusLib documentation.

## What Changed

### Configuration Files

1. **Removed**: `mkdocs.yml` - MkDocs configuration
2. **Added**: `docs/conf.py` - Sphinx configuration with equivalent settings
3. **Updated**: `.readthedocs.yaml` - Changed from MkDocs to Sphinx builder
4. **Updated**: `docs/requirements.txt` - Replaced MkDocs dependencies with Sphinx packages

### Documentation Structure

1. **Added**: `docs/index.rst` - Main index page with toctree directives for navigation
2. **Renamed**: `docs/index.md` → `docs/index.md.old` - Preserved original for reference
3. **Added**: `docs/_static/css/custom.css` - Custom styling for Read the Docs theme
4. **Kept**: All existing Markdown files remain unchanged and work with MyST parser

### Build Scripts

1. **Added**: `Makefile` - For Unix/Linux/Mac builds
2. **Added**: `make.bat` - For Windows builds

## New Dependencies

The documentation now uses:
- **sphinx** (≥7.0.0) - Documentation generator
- **sphinx-rtd-theme** (≥2.0.0) - Read the Docs theme
- **myst-parser** (≥2.0.0) - Markdown support in Sphinx
- **sphinx-copybutton** (≥0.5.0) - Copy button on code blocks
- **sphinx-design** (≥0.5.0) - UI components (cards, tabs, etc.)
- **sphinx-autobuild** (≥2021.3.14) - Auto-rebuild on file changes
- **linkify-it-py** (≥2.0.0) - Automatic link detection

## Building the Documentation

### Prerequisites

Install Python dependencies:
```bash
pip install -r docs/requirements.txt
```

### Build Commands

**Windows:**
```cmd
make.bat html
```

**Linux/Mac:**
```bash
make html
```

The built documentation will be in `docs/_build/html/index.html`.

### Live Development Server

For auto-rebuilding during development:
```bash
sphinx-autobuild docs docs/_build/html
```

Then open http://127.0.0.1:8000 in your browser.

## Features Preserved

All MkDocs features have Sphinx equivalents:

| MkDocs | Sphinx | Notes |
|--------|--------|-------|
| Material theme | Read the Docs theme | Similar navigation structure |
| pymdownx extensions | MyST parser | Supports most Markdown features |
| mkdocstrings | autodoc/napoleon | Python API documentation |
| search | Built-in | Full-text search included |
| Code highlighting | Pygments | Syntax highlighting for all languages |
| Dark mode | Theme option | Available in RTD theme |
| Copy buttons | sphinx-copybutton | Copy code blocks |

## MyST Markdown Extensions

The following Markdown extensions are enabled:

- **colon_fence**: `:::` fences for directives
- **deflist**: Definition lists
- **fieldlist**: Field lists
- **html_admonition**: HTML-style admonitions
- **linkify**: Auto-detect and linkify URLs
- **smartquotes**: Smart quotes conversion
- **strikethrough**: `~~text~~` strikethrough
- **tasklist**: `- [ ]` task lists
- **attrs_inline**: Inline attributes `{#id .class}`

## Navigation Structure

Navigation is now defined in `docs/index.rst` using toctree directives:

```rst
.. toctree::
   :maxdepth: 2
   :caption: Section Name

   path/to/page1
   path/to/page2
```

All existing Markdown files continue to work without modification.

## Custom Styling

Custom CSS matching PlusLib branding (#900000 dark red) is in:
- `docs/_static/css/custom.css`

## ReadTheDocs Integration

The `.readthedocs.yaml` configuration:
- Uses Ubuntu 22.04
- Python 3.11
- Builds HTML, PDF, and ePub formats
- Automatically installs dependencies from `docs/requirements.txt`
- Points to `docs/conf.py` for Sphinx configuration

## Continuous Integration

When pushed to GitHub, ReadTheDocs will:
1. Detect the `.readthedocs.yaml` configuration
2. Install dependencies from `docs/requirements.txt`
3. Run `sphinx-build` with the configuration in `docs/conf.py`
4. Publish to https://pluslib.readthedocs.io

## Troubleshooting

### "Linkify enabled but not installed"
Install linkify-it-py:
```bash
pip install linkify-it-py
```

### "Document isn't included in any toctree"
Add the document to the appropriate toctree in `docs/index.rst`.

### Build warnings about missing references
These are expected during migration. Update cross-references to use Sphinx syntax:
- MkDocs: `[text](page.md)`
- Sphinx: `:doc:\`text <page>\``

### Theme option warnings
Some theme options differ between themes. These warnings can be ignored or the conf.py updated to use RTD theme-specific options.

## Migration Notes

### What Works Automatically
- All existing Markdown files
- Admonitions (note, warning, tip, etc.)
- Code blocks with syntax highlighting
- Tables
- Images
- Internal links (mostly)
- External links

### What May Need Updates
- Some cross-references between documents
- Custom HTML/CSS in Markdown files
- Material theme-specific features

### Known Warnings (22 total)
Most warnings are for missing cross-references to documents that may not exist or need path updates. These don't affect the build and can be fixed incrementally.

## Benefits of Sphinx

1. **More powerful**: Extensive extension ecosystem
2. **Better API docs**: Superior Python code documentation with autodoc
3. **Multiple formats**: HTML, PDF, ePub, man pages
4. **Industry standard**: Used by Python, Django, Flask, etc.
5. **Better versioning**: Built-in version management
6. **Intersphinx**: Link to other Sphinx documentation
7. **Better search**: More advanced full-text search

## Markdown Compatibility

MyST parser provides excellent Markdown compatibility:
- GitHub Flavored Markdown (GFM) support
- CommonMark compliance
- Extended syntax for Sphinx features
- Most MkDocs Markdown works without changes

## Additional Resources

- [Sphinx Documentation](https://www.sphinx-doc.org/)
- [MyST Parser Documentation](https://myst-parser.readthedocs.io/)
- [Read the Docs Theme](https://sphinx-rtd-theme.readthedocs.io/)
- [ReadTheDocs Build Configuration](https://docs.readthedocs.io/en/stable/config-file/v2.html)
