# PlusLib Documentation

This directory contains the new ReadTheDocs-based documentation for PlusLib.

## Documentation Structure

The documentation is organized into the following sections:

### Getting Started
- **Overview**: - Introduction to PlusLib architecture and components
- **Build Instructions** - Detailed guide to building PlusLib
- **Quick Start** - Basic usage examples

### User Guide
- **Configuration**: - XML configuration file structure and options
- **Devices**: - Supported tracking and imaging devices
- **Calibration**: - Calibration procedures and workflows
- **Data Collection** - Data acquisition and streaming

### Developer Guide
- **Contributing**: - How to contribute to PlusLib
- **Coding Conventions** - Code style and best practices
- **Creating New Devices** - Guide to adding device support
- **API Reference** - Core classes and APIs

### Algorithms
- **Phantom Registration** - Registration algorithms
- **Temporal Calibration** - Video-tracking synchronization
- **Pattern Recognition** - Feature detection in ultrasound images

### Testing
- **Test Dashboards** - CDash dashboard usage
- **Running Tests** - Building and running tests

### About
- **License**: - BSD-style license information
- **Citation**: - How to cite PlusLib

## Building the Documentation

### Local Preview

Install dependencies:
```bash
pip install -r requirements.txt
```

Serve locally:
```bash
mkdocs serve
```

Open browser to <http://localhost:8000>

### Building Static Site

Build HTML documentation:
```bash
mkdocs build
```

Output will be in `site/` directory.

## ReadTheDocs Integration

The documentation is automatically built and hosted by ReadTheDocs when pushed to the repository.

Configuration files:
- `.readthedocs.yaml` - ReadTheDocs build configuration
- `mkdocs.yml` - MkDocs site configuration
- `docs/requirements.txt` - Python dependencies

## Legacy Documentation

The existing Doxygen documentation in `src/Documentation/` is still maintained for API reference. This new Markdown documentation provides:

- Better user experience
- Easier maintenance
- Modern search functionality
- Responsive design
- Better integration with GitHub

## Contributing to Documentation

To update documentation:

1. Edit Markdown files in `docs/` directory
2. Test locally with `mkdocs serve`
3. Commit changes
4. Documentation will auto-deploy on ReadTheDocs

### Markdown Features

We use MkDocs Material theme with support for:

- **Code blocks** with syntax highlighting
- **Admonitions** (notes, warnings, tips)
- **Tables**
- **Math equations** (LaTeX)
- **Tabs**: for multi-option content
- **Diagrams** (Mermaid)

Example:

````markdown
!!! note "Important Note"
    This is a note with important information.

```cpp
// Code block with syntax highlighting
vtkSmartPointer<vtkPlusDevice> device = 
  vtkSmartPointer<vtkPlusDevice>::New();
```

$$E = mc^2$$
````

## Links

- **Documentation**: <https://pluslib.readthedocs.io> (after deployment)
- **Repository**: <https://github.com/PlusToolkit/PlusLib>
- **Website**: <http://www.plustoolkit.org>
