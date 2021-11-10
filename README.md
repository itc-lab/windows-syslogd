# Syslog Server for Windows

## General info

Syslog Server for Windows is a free open source program to receive syslog messages, reads syslog.conf in a Unix compatible format and runs as a Windows service.  
Compatible with router and Linux syslog facilities and levels.

## Technologies

Project is created with:

- Visual Studio Community 2019
- C++ MFC for latest v142-Buildtools (x86 & x64)
- Windows 10 SDK (10.0.18362.0)
- Microsoft Visual Studio Installer Projects Extension

## Build

Choose options.
|![Choose options](https://user-images.githubusercontent.com/76575923/140642284-76d407dd-9c92-4410-9007-a33912699279.png)|
|:-:|

Clone this repository.
![Clone this repository](https://user-images.githubusercontent.com/76575923/140642295-79dfb3bc-ee50-4e08-92a0-e0e3152f51c1.png)|
|:-:|

Add an extension.
![Add an extension](https://user-images.githubusercontent.com/76575923/140642305-2fb79eaa-e74c-4f65-bbdd-c8cc92f5ce39.png)|
|:-:|

Open `ntservice.sln`.
![Open ntservice.sln](https://user-images.githubusercontent.com/76575923/140642421-df3aa80f-2c9d-4d8b-b437-1eb26fb61647.png)|
|:-:|

Run "Batch Build" as the image below.
![Batch Build](https://user-images.githubusercontent.com/76575923/140642328-a0948567-d7df-435d-b333-fe867c18b6d4.png)|
|:-:|

## Setup

Double-click `setup\Release\syslogd.msi` to install.
![Double-click](https://user-images.githubusercontent.com/76575923/140642368-8853dbd2-783b-4ded-ba15-5d537c3a9826.png)|
|:-:|

Make sure that the "Syslog Server" service is started.
![Syslog Server](https://user-images.githubusercontent.com/76575923/140642381-333271cf-31dc-449c-acdd-8d4560110603.png)|
|:-:|

Edit `C:\Program Files (x86)\Common Files\syslogd\syslog.conf`.  
_**Warning:** Since it cannot be overwritten as it is, copy it to another folder, edit it, and overwrite it._
![Edit syslog conf](https://user-images.githubusercontent.com/76575923/140642519-738bdf73-83ea-4189-acd1-dce0cd8d7f30.png)|
|:-:|

## syslog.conf

Below is a table of how much the specification is supported compared to FreeBSD's syslog.conf.

| Features                                 | Support         |
| ---------------------------------------- | --------------- |
| facility detection                       | ✔               |
| The special facility "mark"              | ❌              |
| priority level detection                 | ✔               |
| include                                  | ❌              |
| Delimiter space / tab                    | ✔               |
| Case insensitive                         | ✔               |
| Comparison flag, equal =                 | ✔               |
| Comparison flag, > < => >= !             | ❌              |
| program specification                    | ❌              |
| !prog1,prog2                             | ❌              |
| !-prog1,prog2                            | ❌              |
| hostname specification                   | ❌              |
| #+@ or +@                                | ❌              |
| +hostname1,hostname2                     | ❌              |
| -hostname1,hostname2                     | ❌              |
| PROPERTY-BASED FILTERS                   | ❌              |
| Reset filtering                          | ❌              |
| Priority from behind ;semicolon          | ✔               |
| Comma separated facility                 | ✔               |
| asterisk                                 | ✔               |
| none                                     | ✔               |
| Semicolon delimiter;Priority from behind | ✔               |
| Action - minus sign                      | ❌              |
| Action - @ sign forward host:port, IPv6  | ✔(port, IPv6❌) |
| Action - list of users                   | ❌              |
| Action - vertical bar "\|"               | ❌              |
| Escape #                                 | ❌              |

## See also

[Windows 版 syslogd サービスを作成した(C++)](https://itc-engineering-blog.netlify.app/blogs/windows-syslogd 'Windows版syslogdサービスを作成した(C++)') (Japanese text only)
