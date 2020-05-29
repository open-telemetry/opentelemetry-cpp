# Open Telemetry Onboarding Resource Guide

Hi! If you’re looking at this document, this guide will provide you the resources as a newcomer to the Open Telemetry project. This guide will help you understand the Open Telemetry Project, its components, and specifically the C++ SDK.

### Reading Resources

* Medium [article](https://medium.com/opentelemetry/how-to-start-contributing-to-opentelemetry-b23991ad91f4) (October 2019) on how to start contributing to the Open Telemetry project.
* Medium [article](https://medium.com/opentelemetry/opentelemetry-beyond-getting-started-5ac43cd0fe26) (January 2020) describing the overarching goals and use cases for Open Telemetry.

### Relevant Documentation

* Open Telemetry [Specification](https://github.com/open-telemetry/opentelemetry-specification)
    * The Open Telemetry Specification describes the requirements and expectations of for all Open Telemetry implementations.

* Read through the [Open Telemetry Collector](https://github.com/open-telemetry/opentelemetry-collector) GitHub repository
    * This repository has a lot of good information surrounding the Open Telemetry ecosystem. At the top of the **[readme](https://github.com/open-telemetry/opentelemetry-collector/blob/master/README.md)**, there are multiple links that give newcomers a good idea of what the project is about and how to get involved in it.
* Read through the Open Telemetry Python documentation for its [API](https://opentelemetry-python.readthedocs.io/en/stable/api/api.html) and [SDK](https://opentelemetry-python.readthedocs.io/en/stable/sdk/sdk.html)
    * The [API](https://github.com/open-telemetry/opentelemetry-java/blob/master/QUICKSTART.md) and [SDK](https://github.com/open-telemetry/opentelemetry-java/blob/master/QUICKSTART.md) documentation provides a lot of information on what the classes and their functions are used for. Since there is currently minimal documentation for C++, use the Python repository’s extensive documentation to learn more about how the API and SDK work.

### Code Examples

* Follow the [simple trace example](https://github.com/open-telemetry/opentelemetry-cpp/pull/92) for an introduction to basic Open Telemetry functionality in C++.  Currently the example can be found in [PR #94](https://github.com/open-telemetry/opentelemetry-cpp/pull/94)

* Read through the [Java Quick-Start Guide](https://github.com/open-telemetry/opentelemetry-java/blob/master/QUICKSTART.md)
    * This shows you how the classes and functions will interact in simple and easy to digest examples.
* Take a look at this [Java SDK example.](https://github.com/open-telemetry/opentelemetry-java/tree/master/examples/sdk-usage) This shows a good use case of the SDK using stdout exporter.
* Take a look at the [Java Jaeger example.](https://github.com/open-telemetry/opentelemetry-java/tree/master/examples/jaeger) This provides a brief introduction to the Jaeger exporter, its interface, and how to interact with the service.

### Community Resources

* Visit the Open Telemetry Community repo for links to public meeting notes and recorded meetings from the C++ SIG. These resources highlight current priorities in the C++ project and ongoing discussions.
    * Link to the [Open Telemetry Community](https://github.com/open-telemetry/community) GitHub repository.
    * [C++ SIG meeting notes.](https://docs.google.com/document/d/1i1E4-_y4uJ083lCutKGDhkpi3n4_e774SBLi9hPLocw/edit)
    * [Open Telemetry YouTube Channel](https://www.youtube.com/channel/UCHZDBZTIfdy94xMjMKz-_MA)(Here is the [most recent C++ meeting](https://www.youtube.com/watch?v=zuoDyXSQ68I).)
* [Active contributors](https://github.com/open-telemetry/opentelemetry-cpp/graphs/contributors) of the project. If you have any questions about the repository, these people may be able to help.

### Interactive Resources

* Join the [Gitter Chat](https://gitter.im/open-telemetry/opentelemetry-cpp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) to join the conversation around the C++ project and ask contributors any questions you may have.


Please contribute! You’re welcome to add more information to this guide if you come across any helpful resources. See [CONTRIBUTING.md](https://github.com/open-telemetry/opentelemetry-cpp/blob/master/CONTRIBUTING.md) for more information. Thanks [@ankit-bhargava](https://github.com/ankit-bhargava) and [@Brandon-Kimberly](https://github.com/Brandon-Kimberly) for contributing to this resource guide.

Last updated May 29, 2020

