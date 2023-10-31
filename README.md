
# PortScanner

PortScanner is a versatile and efficient multithreaded tool designed to scan ports on remote hosts, providing you with additional information about open ports - banner information, and TLS/SSL verification. This tool is designed to help you assess the security and services of a target system quickly and effectively.

## Features

- **Multithreaded Scanning:** PortScanner uses multithreading to scan ports concurrently, making it faster and more efficient in discovering open ports.

- **Banner Printing:** It can retrieve banner information from open ports, giving you insights into the services running on those ports.

- **TLS/SSL Verification:** PortScanner can verify TLS/SSL certificates for secure connections, ensuring that you get complete information about encrypted services.

## Usage

To use PortScanner, simply specify the desired flags and provide the IP address or hostname of the target system as an argument. Here are the available flags:

- **-v (Verbose mode):** Provides detailed output, including banner information and SSL verification details.

- **-q (Quick scan for TLS/SSL related ports):** Scans common TLS/SSL-related ports.

- **-s (Scan system ports):** Scans well-known system ports (0-1023).

- **-u (Scan user ports):** Scans registered user ports (1024-49151).

- **-p (Scan private ports):** Scans dynamic and/or private ports (49152-65535).

- **-a (Scan all ports):** Scans all available ports.

- **-h (Help):** Displays the help message, providing a quick reference to the available flags.

Example usage:

```
./portscan -q 192.168.1.1
```

## Building the Program

PortScanner can be easily built using the provided Makefile. Before building, make sure you have the following prerequisites installed:

- **g++:** The C++ compiler is required to build and run the program.

- **libcurl4-openssl-dev:** This library is essential for handling network operations, including TLS/SSL verification.

Follow these steps to build the program:

1. Clone or download the repository to your local machine.

2. Place the Makefile in the same directory as your source files.

3. Open a terminal and navigate to the directory containing the Makefile.

4. Run `make` to build your PortScanner program.

5. The compiled binary, named `portscan`, will be created in the same directory.

## Compatibility

PortScanner is designed to work on both macOS and Linux operating systems, providing a consistent and reliable experience across these platforms.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- PortScanner is built upon the libcurl library, which provides robust networking capabilities.

- Special thanks to the open-source community for providing valuable tools and libraries.
