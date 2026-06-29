# Changelog

All notable changes to FastJSON will be documented in this file.

## [0.1.1] - 2026-06-29

### Fixed
- **CRITICAL**: Fixed a major bug where `FastJsonValue` would cause a double-free `EXCEPTION_ACCESS_VIOLATION` crash when sub-nodes were garbage collected. Sub-nodes extracted via `.path()` or `.get()` no longer incorrectly attempt to free the native JSON tree memory.

## [0.1.0] - Initial Release
- Initial implementation of the zero-copy FastJSON parser.
