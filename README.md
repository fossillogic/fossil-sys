# **Fossil Sys by Fossil Logic**

Fossil Sys is a versatile, cross-platform utilities library designed to provide essential functionalities for high-performance system-level task development. It includes a suite of common utilities such as host system interaction, command execution, a cnullptr type definition, and secure memory management. Fossil Sys is implemented in both C and C++ and is optimized for consistency and efficiency across various operating systems.

## Key Features

1. **Cross-Platform Compatibility**: Provides a uniform interface and consistent behavior across major operating systems, including Windows, macOS, and Linux.
2. **Modular Design**: Contains modular components that can be easily integrated and customized for specific project needs.
3. **Efficient Build System**: Built using the Meson build system, which ensures fast build times and a straightforward development process.
4. **High Performance**: Developed in C and C++ to maximize performance and resource efficiency, suitable for both desktop and embedded systems.
5. **Extensive Documentation**: Offers comprehensive documentation and examples to help developers quickly understand and utilize the library.

## Getting Started

## ***Prerequisites***

To get started, ensure you have the following installed:

- **Meson Build System**: If you don’t have Meson `1.8.0` or newer installed, follow the installation instructions on the official [Meson website](https://mesonbuild.com/Getting-meson.html).

### Adding Dependency

#### Adding via Meson Git Wrap

To add a git-wrap, place a `.wrap` file in `subprojects` with the Git repo URL and revision, then use `dependency('fossil-sys')` in `meson.build` so Meson can fetch and build it automatically.

#### Integrate the Dependency:

Add the `fossil-sys.wrap` file in your `subprojects` directory and include the following content:

```ini
[wrap-git]
url = https://github.com/fossillogic/fossil-sys.git
revision = v0.2.5

[provide]
dependency_names = fossil-sys
```

**Note**: For the best experience, always use the latest releases. Visit the [releases](https://github.com/fossillogic/fossil-sys/releases) page for the latest versions.

## Configure Options

- **Running Tests**: Enable testing by configuring with `-Dwith_test=enabled`.

Example:

```sh
meson setup builddir -Dwith_test=enabled
```

### Tests Double as Samples

The project is designed so that **test cases serve two purposes**:

- ✅ **Unit Tests** – validate the framework’s correctness.  
- 📖 **Usage Samples** – demonstrate how to use these libraries through test cases.  

This approach keeps the codebase compact and avoids redundant “hello world” style examples.  
Instead, the same code that proves correctness also teaches usage.  

This mirrors the **Meson build system** itself, which tests its own functionality by using Meson to test Meson.  
In the same way, Fossil Logic validates itself by demonstrating real-world usage in its own tests via Fossil Test.  

```bash
meson test -C builddir -v
```

Running the test suite gives you both verification and practical examples you can learn from.

## Contributing and Support

For contributions, issues, or support, please open an issue on the project repository or visit the [Fossil Logic Docs](https://fossillogic.com/docs) for more information. Contributions and feedback are always appreciated.
