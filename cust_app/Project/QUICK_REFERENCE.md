# Logging Framework Quick Reference Card

## One-Page Cheat Sheet

### Setup (in each .c file)
```c
#include "common/log.h"
#define TAG "MODULENAME"
```

### Logging Macros
```c
LOGE(TAG, "Error message");              // 🔴 Error
LOGW(TAG, "Warning message");            // 🟡 Warning
LOGI(TAG, "Info message");               // 🔵 Info
LOGD(TAG, "Debug message");              // ⚪ Debug
LOGV(TAG, "Verbose message");            // ⚪⚪ Verbose (rare)
```

### With Arguments
```c
LOGD(TAG, "Counter: %d", count);
LOGD(TAG, "Status: %s", status);
LOGD(TAG, "Voltage: %.2f V", voltage);
LOGD(TAG, "Value: %d, Status: %s", val, status);
```

### Output Format
```
[E][GPS] Error message
[W][GPS] Warning message
[I][GPS] Info message
[D][GPS] Debug message
```

### With Location (optional, edit log.h)
```c
#define LOG_INCLUDE_LOCATION 1
// Output becomes:
[E][GPS][gps_init:110] Error message
[I][GPS][update_epo:566] Info message
```

### Global Log Level (in common/log.h)
```c
#define GLOBAL_LOG_LEVEL LOG_LEVEL_ERROR      // Only errors
#define GLOBAL_LOG_LEVEL LOG_LEVEL_WARN       // + warnings
#define GLOBAL_LOG_LEVEL LOG_LEVEL_INFO       // + info (production)
#define GLOBAL_LOG_LEVEL LOG_LEVEL_DEBUG      // + debug (development)
#define GLOBAL_LOG_LEVEL LOG_LEVEL_VERBOSE    // + verbose (deep debug)
```

### Log Levels

| Level | Value | Includes | Use |
|-------|-------|----------|-----|
| ERROR | 5 | - | Critical failures |
| WARN | 4 | ERROR | Warnings |
| INFO | 3 | ERROR, WARN | State changes |
| DEBUG | 2 | All above | Detailed info |
| VERBOSE | 1 | All | Very detailed |

### Real-World Examples

#### GPS Module
```c
#include "common/log.h"
#define TAG "GPS"

int gps_init(void) {
    LOGI(TAG, "Initializing GNSS");
    
    if (fibo_gnss_enable(1) != 0) {
        LOGE(TAG, "GNSS init failed");
        return -1;
    }
    
    LOGD(TAG, "GNSS initialization complete");
    return 0;
}
```

#### SIM Module
```c
#include "common/log.h"
#define TAG "SIM"

void sim_init(void) {
    LOGI(TAG, "Initializing SIM");
    
    if (check_sim_presence() == 0) {
        LOGW(TAG, "SIM not detected");
        return;
    }
    
    LOGD(TAG, "Reading IMSI...");
    // read IMSI
    LOGI(TAG, "SIM initialized successfully");
}
```

#### Main Application
```c
#include "common/log.h"
#define TAG "MAIN"

void main_loop(void) {
    LOGI(TAG, "Application started");
    
    if (init_device() != 0) {
        LOGE(TAG, "Device initialization failed");
        return;
    }
    
    LOGD(TAG, "Device initialized");
    
    while(1) {
        if (low_memory()) {
            LOGW(TAG, "Memory low");
        }
        // main loop
    }
}
```

### Compile-Time Filtering

Given: `#define GLOBAL_LOG_LEVEL LOG_LEVEL_INFO`

```c
LOGE(TAG, "Error");      // ✓ Included
LOGW(TAG, "Warning");    // ✓ Included
LOGI(TAG, "Info");       // ✓ Included
LOGD(TAG, "Debug");      // ✗ Compiles to ((void)0) - NO OVERHEAD
LOGV(TAG, "Verbose");    // ✗ Compiles to ((void)0) - NO OVERHEAD
```

### Best Practices ✓

```c
// ✓ Good - clear level
LOGE(TAG, "Critical error in init");

// ✓ Good - appropriate level
LOGW(TAG, "Retrying connection, attempt %d", attempt);

// ✓ Good - state changes use INFO
LOGI(TAG, "Connected to network");

// ✓ Good - detailed info for debugging
LOGD(TAG, "Received packet: %d bytes", len);

// ✓ Good - function entry/exit for tracing
LOGD(TAG, "Processing GPS data...");
// ... do work ...
LOGD(TAG, "GPS processing complete");
```

### Anti-Patterns ✗

```c
// ✗ Excessive detail
LOGD(TAG, "Loop iteration %d", i);  // In tight loop - too much!

// ✗ Wrong level
LOGD(TAG, "Critical failure");  // Should be LOGE()

// ✗ Missing format string
LOGD(TAG, variable);  // Format string required

// ✗ String concatenation (not safe)
LOGD(TAG, "Value: " + str);  // Use format specifier instead
```

### Performance Impact

**Disabled logs: ZERO overhead**
```c
LOGD(TAG, "Debug info");  // Compiles to: ((void)0)
                          // No function call, no string processing
```

**Enabled logs: Single function call**
```c
LOGI(TAG, "Info message");  // One call to fibo_textTrace()
LOGI(TAG, "Value: %d", 42); // Still one call with formatted string
```

### Production Configuration

```c
// common/log.h - Production
#define GLOBAL_LOG_LEVEL LOG_LEVEL_INFO
// Only ERROR, WARN, INFO shown
// Clean logs, minimal overhead
```

### Development Configuration

```c
// common/log.h - Development
#define GLOBAL_LOG_LEVEL LOG_LEVEL_DEBUG
#define LOG_INCLUDE_LOCATION 0
// All levels except VERBOSE
// Detailed debugging info
```

### Deep Debugging Configuration

```c
// common/log.h - Deep Debug
#define GLOBAL_LOG_LEVEL LOG_LEVEL_VERBOSE
#define LOG_INCLUDE_LOCATION 1
// All levels including VERBOSE
// Shows function names and line numbers
```

### Common Pitfalls

**Problem:** Logs not appearing  
**Solution:** Check `GLOBAL_LOG_LEVEL` is not higher than your log level
```c
// If GLOBAL_LOG_LEVEL = LOG_LEVEL_ERROR
// Then LOGD() and LOGI() won't appear - this is correct behavior!
```

**Problem:** Compiler warning about unused variables  
**Solution:** Normal and harmless when logs are disabled
```c
LOGD(TAG, "Value: %d", unused_var);  // If DEBUG logs are off, var might be unused
// This is expected - just ignore the warning
```

**Problem:** Slow output or dropped logs  
**Solution:** Reduce GLOBAL_LOG_LEVEL to minimize logging
```c
#define GLOBAL_LOG_LEVEL LOG_LEVEL_WARN  // Only errors and warnings
```

### File Locations

- **Framework:** `common/log.h`
- **Usage in:** `main.c`, `service/gps.c`, `service/sim.c`
- **Documentation:** `LOGGING_GUIDE.md`
- **Examples:** `CODE_EXAMPLES.md`
- **Summary:** `INTEGRATION_SUMMARY.md`

### Quick Integration for New Files

1. Copy this at the top of any new .c file:
```c
#include "common/log.h"
#define TAG "MYMODULE"
```

2. Use LOG macros:
```c
LOGI(TAG, "Starting module");
LOGD(TAG, "Debug info: %d", value);
LOGE(TAG, "Error occurred");
```

3. Done! No other setup needed.

---

## Integration Status

| Component | Status |
|-----------|--------|
| log.h | ✅ Created |
| main.c | ✅ Updated |
| gps.c | ✅ Updated |
| sim.c | ✅ Updated |
| Documentation | ✅ Complete |
| Compilation | ✅ Clean |
| Functionality | ✅ Unchanged |
| Production Ready | ✅ Yes |

---

**Last Updated:** February 23, 2026  
**Framework Version:** 1.0 (Production)
