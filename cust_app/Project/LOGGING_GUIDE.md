# Professional Embedded Logging Framework - Usage Guide

## Overview

This is a header-only, compile-time filtering logging framework designed for the Fibocom OpenCPU RTOS environment. It provides professional-grade logging with zero runtime overhead when disabled.

**Key Characteristics:**
- ✅ Zero overhead when logs are disabled (compile-time elimination)
- ✅ Header-only implementation (no .c files needed)
- ✅ No dynamic memory allocation
- ✅ No printf redirection
- ✅ Thread-safe (single call to `fibo_textTrace`)
- ✅ Per-module TAG support
- ✅ Per-module verbose control
- ✅ Optional function name and line number in logs
- ✅ C99 compatible

## Installation

The logging framework is located in: `common/log.h`

Already integrated in:
- `main.c`
- `service/gps.c`  
- `service/sim.c`

## Quick Start

### 1. Include the Header and Define TAG

At the top of your .c file (after other includes):

```c
#include "common/log.h"

#define TAG "MYMODULE"
```

### 2. Use Logging Macros

```c
// Info level
LOGI(TAG, "Initialization started");

// Debug level  
LOGD(TAG, "Counter = %d", counter);

// Warning level
LOGW(TAG, "Low memory: %d bytes", free_mem);

// Error level
LOGE(TAG, "Failed to open file");

// Verbose level (rarely used, can be disabled)
LOGV(TAG, "Detailed state: %s", state_str);
```

## Log Levels

Five log levels from highest to lowest priority:

| Level | Macro | Use Case |
|-------|-------|----------|
| ERROR | `LOGE()` | Fatal errors, critical failures |
| WARN | `LOGW()` | Warnings, degraded operation |
| INFO | `LOGI()` | General information, state changes |
| DEBUG | `LOGD()` | Detailed debugging information |
| VERBOSE | `LOGV()` | Very detailed tracing (rarely used) |

## Global Log Level Control

In `common/log.h`, set the global control:

```c
#define GLOBAL_LOG_LEVEL LOG_LEVEL_DEBUG
```

Available levels:
- `LOG_LEVEL_ERROR` → Only errors shown
- `LOG_LEVEL_WARN` → Errors and warnings
- `LOG_LEVEL_INFO` → Errors, warnings, info (recommended for production)
- `LOG_LEVEL_DEBUG` → All except verbose (default, good for development)
- `LOG_LEVEL_VERBOSE` → All messages (for deep debugging)

**How it works:**
- Logs at or above the global level are compiled in
- Logs below the global level compile to nothing (zero overhead)
- Example: If `GLOBAL_LOG_LEVEL = LOG_LEVEL_INFO`, then `LOGD()` and `LOGV()` compile to nothing

## Output Format

### Without Location Info (Default)

```
[E][GPS] GNSS init failed after 3 retries
[I][SIM] Service initialized  
[D][MAIN] AT Response: OK
[W][GPRS] No APN found on SIM
```

Format: `[LEVEL][TAG] message`

### With Location Info

Enable in `common/log.h`:

```c
#define LOG_INCLUDE_LOCATION 1
```

Output becomes:

```
[E][GPS][gps_init:110] GNSS init failed after 3 retries
[I][SIM][sim_init:35] Service initialized
[D][MAIN][at_res_callback:40] AT Response: OK
[W][GPRS][get_apn_from_sim:46] No APN found on SIM
```

Format: `[LEVEL][TAG][function:line] message`

## Per-Module Verbose Control

Enable verbose logging for specific modules:

**Example 1: Enable verbose for GPS**

In `service/gps.c`:

```c
#include "common/log.h"

#define GPS_VERBOSE_ENABLE 1
#define TAG "GPS"
```

Now `LOGV(TAG, ...)` will log if `GLOBAL_LOG_LEVEL` is `LOG_LEVEL_VERBOSE`.

**Example 2: Disable verbose for SIM**

In `service/sim.c`:

```c
#include "common/log.h"

// Don't define SIM_VERBOSE_ENABLE - verbose is disabled
#define TAG "SIM"

// LOGV() will compile to nothing for this module
```

## Best Practices

### 1. Module-Level TAG Definition

Always define TAG at the top of each module (after includes):

```c
#include "common/log.h"

#define TAG "MYMODULE"  // Must match module name for clarity

void my_function(void) {
    LOGI(TAG, "Function started");
}
```

### 2. Use Appropriate Log Levels

```c
if (result != 0) {
    LOGE(TAG, "Critical operation failed: %d", result);  // Error
    return -1;
}

if (low_memory_condition) {
    LOGW(TAG, "Memory low: %d bytes remain", mem_left);  // Warning
}

LOGI(TAG, "Device connected to network");  // Important state change

LOGD(TAG, "Packet size: %d bytes", packet_size);  // Debug detail
```

### 3. Log at Function Entry/Exit Only When Needed

```c
// ✓ Good - Log at function boundaries for tracing
int init_device(void) {
    LOGD(TAG, "Initializing device");
    
    // ... initialization code ...
    
    LOGD(TAG, "Device initialized successfully");
    return 0;
}

// ✗ Avoid - Excessive logging in tight loops
void send_data(const uint8_t *data, int len) {
    // ✗ DON'T do this in a loop:
    for (int i = 0; i < len; i++) {
        LOGD(TAG, "Sending byte %d: 0x%02x", i, data[i]);  // Too much!
    }
    
    // ✓ Better - log summary
    LOGD(TAG, "Sent %d bytes", len);
}
```

### 4. Format Strings Properly

```c
// ✓ Good
LOGD(TAG, "Counter = %d", counter);
LOGD(TAG, "Value = %.2f", voltage);
LOGD(TAG, "Status: %s", status_str);

// ✗ Avoid
LOGD(TAG, "Counter = " + counter);  // String concatenation not safe
LOGD(TAG, counter);  // Missing format string
```

### 5. Check Conditions Before Logging Complex Data

```c
// ✓ Good
if (enable_debug) {
    LOGD(TAG, "Buffer contents: %.*s", len, buffer);
}

// Alternatively, use LOGV for very detailed info
LOGV(TAG, "Buffer hex: %02x %02x %02x ...", buf[0], buf[1], buf[2]);
```

## Examples from the Project

### main.c
```c
#include "common/log.h"
#define TAG "MAIN"

void *appimg_enter(void *param)
{
    LOGI(TAG, "Boot");
    // ... initialization ...
    return (void *)&user_callback;
}
```

### gps.c
```c
#include "common/log.h"
#define TAG "GPS"

int gps_init(void)
{
    LOGI(TAG, "Initializing GNSS");
    
    int ret = fibo_gnss_set_satellite(3);
    if (ret != 0) {
        LOGW(TAG, "Warning - set_satellite failed (%d)", ret);
    }
    
    // ... more initialization ...
    
    LOGE(TAG, "GNSS init failed after %d retries", GNSS_ENABLE_RETRY);
    return -1;
}

void gps_overspeed_check(void)
{
    if (!GPS.GPSFix) return;
    
    if (GPS.Speed > GPS_OVERSPEED_THRESHOLD) {
        LOGW(TAG, "OVERSPEED! %.1f km/h > %.1f km/h threshold",
             GPS.Speed, GPS_OVERSPEED_THRESHOLD);
    }
}
```

### sim.c
```c
#include "common/log.h"
#define TAG "SIM"

void sim_init(void)
{
    memset(&GSM, 0, sizeof(GSM_Typedef));
    LOGI(TAG, "Service initialized");
}

static void get_apn_from_sim(void)
{
    LOGI(TAG, "Reading APN from SIM card...");
    
    int32_t result = fibo_nw_get_apn(0, nPdpType, &apn_len, apn);
    
    if (result == 0 && apn_len > 0) {
        LOGI(TAG, "SIM APN found: %s", NetWork.APN);
    } else {
        LOGE(TAG, "No APN found on SIM (result=%ld)", result);
    }
}
```

## Configuration for Different Build Targets

### Production Build
```c
// In common/log.h
#define GLOBAL_LOG_LEVEL LOG_LEVEL_INFO
```
- Only INFO, WARN, and ERROR messages
- Minimal logging overhead
- Clean logs for end users

### Development Build
```c
// In common/log.h
#define GLOBAL_LOG_LEVEL LOG_LEVEL_DEBUG
```
- All messages except VERBOSE
- Detailed debugging information
- Good balance of information and overhead

### Debugging Build
```c
// In common/log.h
#define GLOBAL_LOG_LEVEL LOG_LEVEL_VERBOSE
#define LOG_INCLUDE_LOCATION 1
```
- All messages including VERBOSE
- Function names and line numbers
- Maximum information but some overhead

## Performance Impact

### Zero Overhead Example

With `GLOBAL_LOG_LEVEL = LOG_LEVEL_INFO`:

```c
LOGD(TAG, "This is debug info");  // Compiles to: ((void)0)
LOGV(TAG, "This is verbose");     // Compiles to: ((void)0)
```

No function call, no string formatting, no overhead!

### Minimal Overhead When Enabled

When log level matches, a single function call to `fibo_textTrace()`:

```c
LOGI(TAG, "Initialized");  
// Compiles to:
fibo_textTrace("[I][TAG] Initialized");
```

## Thread Safety

The framework is thread-safe because:
1. All formatting happens in compile-time macros
2. Only one call to `fibo_textTrace()` per log statement
3. No shared state or buffers

## Limitations and Design Constraints

1. **No Heap Usage** - All formatting is done in single `fibo_textTrace()` call
2. **No Dynamic Strings** - Use format specifiers (%d, %s, %f) only
3. **No Variadic Argument Processing** - Macros handle this directly
4. **No Log Callbacks** - Uses system's `fibo_textTrace()` backend

## Troubleshooting

### Problem: LOG macros not appearing in output
- **Solution**: Check `GLOBAL_LOG_LEVEL` is not set higher than your log level
- Example: If `GLOBAL_LOG_LEVEL = LOG_LEVEL_ERROR`, DEBUG logs won't appear

### Problem: Compiler errors about undefined TAG
- **Solution**: Ensure `#define TAG "MODULENAME"` appears after including log.h
- Check no semicolon after `#define TAG "..."`

### Problem: Unused variable warnings
- **Solution**: This is expected behavior when logs are disabled.They compile to `((void)0)` and the compiler may warn about unused parameters.
- These warnings are harmless and can be safely ignored.

## Migration Guide

If you're updating existing code:

1. Add `#include "common/log.h"` and `#define TAG "..."` to each module
2. Replace `fibo_textTrace()` calls:
   ```c
   // Old
   fibo_textTrace("GPS: Error initializing");
   
   // New
   LOGE(TAG, "Error initializing");
   ```
3. Compile and test - functionality should be identical
4. Adjust `GLOBAL_LOG_LEVEL` in log.h as needed

## Further Reading

- [log.h](common/log.h) - Complete implementation with detailed comments
- Log levels and their use cases
- Per-module customization options
