We follow the standard [GitHub Flow](https://guides.github.com/introduction/flow/) process. Contributions to the Plus project are welcome through GitHub pull requests.

See below information about development process and coding rules.

Issue tracking
--------------

- Commit one fix/enhancement at a time (and not multiple independent developments in one single commit) - whenever it's possible without significant extra effort (if you have a choice, commit your fix before start fixing a new problem).
- When changes committed related to a bug (partial fix, etc.) then add a reference to the ticket id to the commit log in the format: re #123 (this will automatically link the changeset to the ticket)

Committing code changes
-----------------------

- Before committing any code changes complete the following steps:
  - Run the automatic test: ...\PlusLib-bin\BuildAndTest.bat and make sure that all the tests pass
  - Run the automatic test: ...\PlusApp-bin\BuildAndTest.bat and make sure that all the tests pass
- Build PlusApp.sln ALL_BUILD project and make sure that there are no build errors
- Commit comments: describe what did you change and why (e.g., Added MyClass:MyObject method to allow doing something), if the modification is related to a ticket (which should be usually the case) then include the ticket id in the comments (for example: re #123: Changed something somewhere because of something...; see the Issue tracking section for details)
- After committing code changes have a look at the dashboard about half an hour later to make sure that all the automatic builds still pass at http://perkdata.cs.queensu.ca/CDash/index.php?project=PlusLib and http://perkdata.cs.queensu.ca/CDash/index.php?project=PlusApp. If you don't want to wait and manually check the results then you can get an automatic notification from CDash about any errors that you introduced if you register yourself to the Plus and PlusApp projects on the dashboard.

Coding conventions
------------------

- In case of child class of a VTK class, use the VTK coding convention, similarly in case of a Qt derived class, the Qt convention has to be used.
- A Qt-like commenting style shall be used in all classes, regardless if it was derived from Qt, VTK, or other class. Use the PlusLib\src\CalibrationAlgo\vtkSpacingCalibAlgo.h as an example.
  - See details on the doxygen commenting features at: http://www.stack.nl/~dimitri/doxygen/docblocks.html
- Use Windows-style end-of-line characters (CR/LF)
- Use 2 spaces to indent lines (do not use tabs)
- Always put curly brackets in new line, and add a new line after it as well. Use curly brackets even if they enclose only one statement (it can cause errors if the developer does not add the brackets when adding another statement in the block).
- Use PlusLib\src\CalibrationAlgo\vtkSpacingCalibAlgo.cxx as a sample for conventions of source files
- Error handling: PlusLib shall communicate errors to the caller by returning an error code. PlusLib shall not throw any exceptions. Using of exceptions within PlusLib is acceptable but not recommended (only when the additional risk of crashes due to unhandled exceptions and the additional complexity of writing exception-safe code is counterweighed by advantages of using exceptions). If exceptions are used then it has to be very well documented and limited to the smallest possible scope to make sure no exception-unsafe code can be exposed to exceptions. (see more information in the discussion about the topic)
- Always include PlusConfigure.h as a first header in the cxx files (it defines basic types such as PlusStatus and basic functions such as logging, which may be required by any header file).
- Don't use VTK IO classes for reading/writing image data (or flip right after reading or right before writing). VTK image and ITK image use the same memory layout (use PlusVideoFrame::ReadImageFromFile and PlusVideoFrame::SaveImageToFile methods instead). 
- If any method returns with PLUS_FAIL then the method shall log the cause of the failure using the LOG_ERROR() macro.
- Avoid using CRT string functions (sprintf, strcpy, sscanf, ...). Use STL strings and stream functions instead.Using of CRT string functions tend to lead to more complex implementations, especially when you want to make sure that there are no buffer overrun, unterminated string, memory leak, etc. errors.
- Member variable names: use VTK conventions for VTK classes, ITK conventions for ITK classes; for other classes use m_ as a prefix to member variable names (e.g., m_SampleVariable)
- Logging:
  - Never use printf or std::out << for logging debug or error messages. Always use the predefined Plus macros for this.
  - LOG_ERROR: use it when an error occurred, almost always displayed; typically a test is considered to be failed if an error message is logged during its execution
  - LOG_WARNING: use it to note when very probably an error is occurred or likely to occur soon, almost always displayed; typically a test is considered to be failed if a warning message is logged during its execution
  - LOG_INFO: message for the user, usually at important points in the application flow, results or metrics
  - LOG_DEBUG: generic debugging message, to be read by developers only, usually it's not displayed, but when you have a problem you enable this log level to show more information
  - LOG_TRACE: similar to LOG_DEBUG, but it shows even more detailed information
- In general all B-mode images are stored in MF coordinate system (see definition in Ultrasound image orientation) and RF images are stored in FM coordinate system. If this is not the case then the variable name that stores the image should reflect that the image is in a different coordinate system (e.g, image_UF = flipH(image)).


Changing configuration file structure
-------------------------------------

- When the configuration file structure changes in any way
  - Add an entry to the change history at Configuration file structure page
  - Refresh the configuration file structure section if needed
- When making major change to the configuration file structure
  - Perform the steps of minor changes 
  - Increment the configuration file version number in WriteConfiguration
  - Increment the configuration file version numbers in the existing configuration files to maintain
