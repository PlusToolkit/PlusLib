@rem This batch file starts a command prompt that makes Plus executables easily accessible.
@rem The Plus binary directory is added to the system PATH variable,
@rem so any Plus commands can be started even if the current directory is changed.
@rem This file must be in the Plus binary directory (where PlusVersion.exe is located).

@echo Plus command prompt
@set PATH="%~dp0";%PATH%
@cd /d %~dp0
@PlusVersion.exe --short
@echo.
@%comspec% /k
