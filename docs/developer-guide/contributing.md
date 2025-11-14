# Contributing to PlusLib

We follow the standard [GitHub Flow](https://guides.github.com/introduction/flow/) process. Contributions to the Plus project are welcome through GitHub pull requests.

## Issue Tracking

- Browse existing issues at [GitHub Issues](https://github.com/PlusToolkit/PlusLib/issues)
- Before creating a new issue, search to avoid duplicates
- Provide detailed information including:
  - Error messages
  - Configuration files
  - Steps to reproduce
  - Expected vs actual behavior
  - Platform and version information

When committing changes related to a bug (partial fix, etc.), add a reference to the ticket ID in the commit log:
```
re #123: Fixed memory leak in vtkPlusDevice
```

This will automatically link the changeset to the ticket.

## Development Workflow

### 1. Fork and Clone

```bash
git clone https://github.com/YOUR-USERNAME/PlusLib.git
cd PlusLib
```

### 2. Create Feature Branch

```bash
git checkout -b feature/my-new-feature
```

Use descriptive branch names:
- `feature/add-new-device-support`
- `bugfix/fix-memory-leak`
- `docs/update-calibration-guide`

### 3. Make Changes

- Follow [coding conventions](coding-conventions.md)
- Add tests for new functionality
- Update documentation
- Commit one fix/enhancement at a time (when possible)

### 4. Test Your Changes

Before committing, ensure all tests pass:

**Windows:**
```bash
cd PlusLib-bin
BuildAndTest.bat
```

**Linux/macOS:**
```bash
cd PlusLib-bin
./BuildAndTest.sh
```

Also verify:
- Build PlusApp.sln ALL_BUILD project with no errors (Windows)
- All existing tests still pass
- New tests are added for new functionality

### 5. Commit Changes

Write clear, descriptive commit messages:

```bash
git commit -m "Add support for new ultrasound device

- Implemented vtkPlusNewDevice class
- Added configuration parameters for device settings
- Included unit tests
- Updated documentation

re #456"
```

**Commit message guidelines:**
- First line: brief summary (50 chars or less)
- Blank line
- Detailed description of what and why
- Reference issue numbers with "re #123"

### 6. Push and Create Pull Request

```bash
git push origin feature/my-new-feature
```

Then create a pull request on GitHub:
1. Go to your fork on GitHub
2. Click "Pull Request" button
3. Provide a clear title and description
4. Reference related issues

## Code Review Process

- All contributions require code review
- Address reviewer feedback promptly
- Maintain clean commit history (squash commits if requested)
- Be responsive to questions and suggestions

## Monitoring Build Status

After your pull request is merged, monitor the dashboard:

- [PlusLib Dashboard](http://perkdata.cs.queensu.ca/CDash/index.php?project=PlusLib)
- [PlusApp Dashboard](http://perkdata.cs.queensu.ca/CDash/index.php?project=PlusApp)

Check about 30 minutes after your commit to ensure all automatic builds still pass.

You can register on CDash to receive automatic notifications about build failures.

## What to Contribute

### Bug Fixes
- Fix reported issues
- Improve error handling
- Fix memory leaks
- Correct documentation errors

### New Features
- New device support
- New calibration algorithms
- Performance improvements
- Additional test coverage

### Documentation
- Improve existing documentation
- Add usage examples
- Document undocumented features
- Fix typos and clarify unclear sections

### Testing
- Add unit tests
- Add integration tests
- Improve test coverage
- Add test data

## Communication

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: Questions and general discussion
- **Pull Requests**: Code contributions and reviews

## Getting Help

If you need help with your contribution:

1. Check the [Developer Guide](api-reference.md)
2. Search existing issues and discussions
3. Ask questions in GitHub Discussions
4. Reference similar existing code

## License

By contributing to PlusLib, you agree that your contributions will be licensed under the same BSD-style license as the project.

See [License](../about/license.md) for details.
