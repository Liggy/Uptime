# Uptime

Tool to display Windows system uptimes
* [Usage](#usage)
* [Development](#development)
  * [ToDo](#todo)
* [Translation](#translation)

## Usage
Uptime [server] [/s] [/a] [/h] [/?]
* server - optionally connect to remote machine
* /s - Show all available startup and shutdown times
* /a - Show absolute startup time (only without /s)
* /h - Include hibernate/sleep (only with /s)
* /? - Show this help

## Development

### ToDo
* Check if building language only once for all targets can be implemented

## Translation
The main binary will include US-English (en-US) resources like stringtables or dialogs for having valid output even without MUI file and language neutral resources like icons.
Language specific files are located under the language folder. Each language requires a new project named after the [Language tag](https://learn.microsoft.com/openspecs/windows_protocols/ms-lcid/63d3d639-7fd2-4afb-abbe-0d5b5551eef8)
