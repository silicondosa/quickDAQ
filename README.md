# QuickDAQ
QuickDAQ is an easy-to-use C/C++ wrapper for the National Instruments (NI) DAQmx C API to perform data acquisition using NI hardware. It will also support the [USC Valero Lab](https://valerolab.org/) DAQ board and Arduino profiles for the control of tendon-driven robots.

## Prerequisites (bundled in `lib/`)
- **cLinkedList**: A simple linked list manager for C/C++.
- **NI DAQmx C API** _(if using NI hardware)_: C API and drivers to interface with NI PCI(e)/PXI(e)/USB data acquition hardware. More info about support and licensing in [this section](#National-Instruments-DAQmx-support-and-licenseing-for-use-with-QuickDAQ) of the README.

## License
The QuickDAQ wrapper is licensed under the GNU Leser GPL v3. The bundled NI-DAQmx C API (a registered trademark of National Instruments Inc.) is an exception to this and is licensed as follows.

## National Instruments DAQmx support and licenseing for use with QuickDAQ
While QuickDAQ is designed to be modular, it was first designed to use NI data acquisition hardware. The NI-DAQmx C API allows us to interface with NI hardware. QuickDAQ intelligently uses the NI-DAQmx C API to provision National Instrument hardware resources based on user function calls.

To use NI hardware with QuickDAQ, you might also need to install NI-DAQmx on your computer (download the latest version [here](https://www.ni.com/en-us/support/downloads/drivers/download.ni-daq-mx.html)). NI hardware and operating systems supported by your version of NI-DAQmx listed [here](https://www.ni.com/en-us/support/documentation/compatibility/21/ni-hardware-and-operating-system-compatibility.html). Additionally, there are [some feature differences](https://www.ni.com/en-us/support/documentation/supplemental/18/windows-vs-desktop-linux-daqmx-experience-differences.html) between the Windows and Linux versions of NI-DAQmx.

Note that if you install NI-DAQmx (on Windows), you can also simulate many (but not all) NI devices and test many (again, not all) QuickDAQ functions on them, a powerful tool for debugging without hardware access. So, I highly recommend that you install it. [Here](https://knowledge.ni.com/KnowledgeArticleDetails?id=kA03q000000x0PxCAI&l=en-US) is a tutorial explaining this in greater detail.

The NI-DAQmx (2023 Q2 release) C API static libraries (for Windows systems) and header files are bundled with this library for user convenience. However, by using the NI-DAQmx C API (that is, if you use QuickDAQ with NI hardware), you agree to the National Instruments Software License Agreement (see `NI_software_license_agreement_en_2023.pdf`) and all other agreements presented to you when installing NI-DAQmx on your computer(s).

## Feedback, issues and pull requests
Please don't hesitate to leave your feedback or request features by opening up an issue. Want to add support for you hardware on QuickDAQ? Then send me a board or create a pull request! ;-)

## Learning Outcomes
The library was created as a means to simplify the usage of NI-DAQmx in real-time control applications. So, in addition to a thorough understanding of the NI-DAQmx C API (which underlies a lot of LabView functionality), I also got a deep understanding of the fundamentals of data acquisition hardware and software best practices when collecting large amounts of data. This understanding also translated to better architectures, firmware and software tools for the modular DAQ hardware (PCB/board) that we are currently designing for tendon-driven robots.

