# Project
The main idea behind this project is to:

A) Learn about Driver Development on Windows (WDM Framework)

B) Look more deeply into the internals of Windows, Operating Systems and Memory in general

Additionally, this driver aims to implement kernel security techniques, such as:
Detecting remote thread injection/dll injection
Dumping DLL/Drivers
Checking for registry access
Driver read/write memory
...
etc.


 # Research/Credits
Paper on VAD tree's: [link](https://www.sciencedirect.com/science/article/pii/S1742287607000503)

nasa-injector: [link](https://git.back.engineering/IDontCode/nasa-injector)
- The injector shows the intial design of the driver to be flawed as, it was initially going to check the VAD for suspicious entries (e.g. entries with no mapping associated with it)
- But, as shown, it takes advantage of a vulnerable driver and does not create any VAD entries meaning a different solution is required; some of which are already provided for.
