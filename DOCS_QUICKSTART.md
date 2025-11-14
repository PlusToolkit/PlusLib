# Quick Start Guide for ReadTheDocs Documentation

## For Users

### View Documentation Online

Once deployed to ReadTheDocs:
- Visit: https://pluslib.readthedocs.io
- Browse by section using the left sidebar
- Use search box (top left) to find specific topics
- Toggle light/dark theme (top right)

### Local Preview

If you want to preview documentation locally:

```bash
# Install MkDocs and dependencies
pip install -r docs/requirements.txt

# Serve documentation locally
mkdocs serve
```

Open browser to: http://localhost:8000

## For Contributors

### Edit Documentation

1. Navigate to `docs/` directory
2. Edit the relevant `.md` file
3. Use Markdown syntax

Example structure:
```
docs/
├── index.md                    # Home page
├── getting-started/
│   ├── overview.md
│   ├── build-instructions.md
│   └── quick-start.md
├── user-guide/
│   ├── configuration.md
│   ├── devices.md
│   ├── calibration.md
│   └── data-collection.md
└── developer-guide/
    ├── contributing.md
    ├── coding-conventions.md
    ├── creating-devices.md
    └── api-reference.md
```

### Preview Your Changes

```bash
# Start local server
mkdocs serve

# Server will auto-reload when you save changes
# View at http://localhost:8000
```

### Add New Pages

1. Create new `.md` file in appropriate directory
2. Add to navigation in `mkdocs.yml`:

```yaml
nav:
  - User Guide:
    - Configuration: user-guide/configuration.md
    - Devices: user-guide/devices.md
    - Your New Page: user-guide/new-page.md  # Add here
```

### Markdown Features

#### Code Blocks

````markdown
```cpp
vtkSmartPointer<vtkPlusDevice> device = 
  vtkSmartPointer<vtkPlusDevice>::New();
```
````

#### Admonitions

```markdown
!!! note
    This is a note

!!! warning
    This is a warning

!!! tip
    This is a helpful tip
```

#### Math Equations

```markdown
Inline: $E = mc^2$

Block:
$$
\frac{-b \pm \sqrt{b^2 - 4ac}}{2a}
$$
```

#### Tables

```markdown
| Header 1 | Header 2 |
|----------|----------|
| Cell 1   | Cell 2   |
| Cell 3   | Cell 4   |
```

#### Links

```markdown
[Link text](relative/path/to/page.md)
[External link](https://example.com)
```

## For Maintainers

### Deploy to ReadTheDocs

1. **Connect Repository**
   - Go to https://readthedocs.org
   - Import PlusToolkit/PlusLib repository
   - ReadTheDocs will auto-detect `.readthedocs.yaml`

2. **Configure Project**
   - Set default branch (usually `master`)
   - Enable automatic builds on commit
   - Configure version settings

3. **Verify Build**
   - Check build logs
   - Test all pages render correctly
   - Verify search works

### Update Dependencies

If you need to add Python packages:

1. Edit `docs/requirements.txt`
2. Test locally:
   ```bash
   pip install -r docs/requirements.txt
   mkdocs build
   ```
3. Commit changes

### Build Locally

```bash
# Build static HTML site
mkdocs build

# Output in site/ directory
# Can host on any web server
```

### Common Tasks

**Add a new device documentation:**
1. Edit `docs/user-guide/devices.md`
2. Add device configuration example
3. Document parameters

**Update API reference:**
1. Edit `docs/developer-guide/api-reference.md`
2. Add new class/method documentation
3. Include code examples

**Add troubleshooting:**
1. Find relevant guide page
2. Add troubleshooting section at bottom
3. Use structured format:
   - **Problem**: Description
   - **Cause**: Why it happens
   - **Solution**: How to fix

## Troubleshooting

### Build Fails Locally

```bash
# Clear cache
rm -rf site/

# Reinstall dependencies
pip install --upgrade -r docs/requirements.txt

# Try building again
mkdocs build
```

### Links Not Working

- Use relative paths: `../other-section/page.md`
- Don't include `.md` extension in rendered links
- Check path is correct relative to current file

### Search Not Working

- Search only works in served mode (`mkdocs serve`) or built site
- Rebuild search index: `mkdocs build --clean`

### Theme Not Loading

```bash
# Reinstall mkdocs-material
pip install --upgrade mkdocs-material
```

## Quick Reference

### File Locations

- **Configuration**: `.readthedocs.yaml`, `mkdocs.yml`
- **Documentation**: `docs/**/*.md`
- **Requirements**: `docs/requirements.txt`
- **Build output**: `site/` (generated, not committed)

### Commands

```bash
# Install
pip install -r docs/requirements.txt

# Serve locally
mkdocs serve

# Build
mkdocs build

# Deploy (if manually deploying)
mkdocs gh-deploy
```

### Navigation

Edit navigation in `mkdocs.yml`:

```yaml
nav:
  - Home: index.md
  - Section Name:
    - Page Title: path/to/page.md
```

### Markdown Syntax

- Headings: `#`, `##`, `###`
- Bold: `**text**`
- Italic: `*text*`
- Code: `` `code` ``
- Links: `[text](url)`
- Images: `![alt](path/to/image.png)`

## Getting Help

- **MkDocs Documentation**: https://www.mkdocs.org
- **Material Theme**: https://squidfunk.github.io/mkdocs-material/
- **ReadTheDocs**: https://docs.readthedocs.io
- **Markdown Guide**: https://www.markdownguide.org

## Need More Information?

See `docs/README.md` for detailed documentation about the documentation system.
