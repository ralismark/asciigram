#pragma once

#include <string>

/**
 * Copy string to clipboard.
 *
 * This is a common interface for OS-specific clipboard API. Calling this
 * function sets the contents of the clipboard to be \p content.
 *
 * Returns if copied successfully.
 */
bool copy_to_sysclip(const std::string& content);
