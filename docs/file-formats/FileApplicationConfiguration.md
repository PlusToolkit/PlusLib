# Application configuration file

Some settings can be set commonly for all applications and device set configurations,
such as directories, last used device set, and preferred text editor application.
These settings are described in the application configuration file.

The application configuration file name is PlusConfig.xml and it is loaded from the directory where the executable file is located. If the file does not exist, the application creates one with the default values.

The application configuration file is an XML file with the following root element:

- **PlusConfig**
    - **LogLevel**: Logging level. See more information on the [**log files**](./FileLog.md) page.
        - `1` (`ERROR`) Only errors are logged
        - `2` (`WARNING`) Only errors and warnings are logged
        - `3` (`DEBUG`) Errors, warnings, and debugging information are logged. Useful for developers and troubleshooting.
        - `4` (`TRACE`) Errors, warnings, and detailed debugging information are logged. Large amount of data may be generated, even if the application is idle. Useful for developers and troubleshooting.
    - **DeviceSetConfigurationDirectory**: Device set configuration files will be searched relative to this directory
    - **ImageDirectory**: Sequence metafiles
    - **ModelDirectory**: Model files
    - **ScriptsDirectory**: Directory of scripts that application may need.
