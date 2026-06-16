# Dual Calendar Core Engine (Gregorian & Tabular Hijri)

A production-grade, highly modular **C++11** library designed for multi-century calendar transformation, interval intersection calculus, and persistent chronological configuration. The architecture enforces a strict separation between solar-tropical and tabular-lunar mathematical models.

### ⚠️ CRITICAL TECHNICAL WARNING & DISCLOSURE

> [!WARNING]
> 
> **TABULAR ALGORITHMIC LIMITATIONS & COMPLIANCE NOTICE**
> 
> 1. **Mathematical vs. Empirical Models:** This engine implements the **Tabular Islamic Calendar** (based on a deterministic 30-year arithmetic cycle). It does **not** query real-time observational astronomy, planetary topocentric coordinate metrics, or physical crescent moon sightings (_Hilal_).
>     
> 2. **Sighting Deviations:** Civil and religious calendars (e.g., the official _Umm al-Qura_ calendar) adapt dynamically to empirical visual verification. Consequently, this engine's raw astronomical conversions may experience a structural variance of **$\pm1$ to $\pm3$ days** depending on geographic boundaries and regional lunar horizons.
>     
> 3. **High-Risk Environment Restriction:** **DO NOT** deploy unadjusted raw outputs from this library in mission-critical synchronization tracks, legal financial execution frameworks, automated standard trading intervals, or supply-chain logistics directly tied to strict religious observations (e.g., _Ramadan_ or _Hajj_ absolute baselines). 
>     
> 4. **Runtime Realignment Protocol:** To resolve theoretical variance, systems engineers must leverage the internal dynamic runtime adjustment architecture (`Settings Console`) to inject calculated local spatial corrections into the physical database state configuration.
>     

## 📑 Detailed Chronological Logic Specifications

### 1. Fundamental Calendar Divergence (Gregorian vs. Tabular Hijri)

The library handles two fundamentally incompatible astronomical tracks using absolute day-count mapping to avoid floating-point rounding degradation across long eras:

- **The Gregorian Track (Solar):** Modeled around the Earth's tropical revolution ($\approx 365.2422$ days). It uses a standard baseline of **365 days**, corrected via a 400-year leap rule ($Year \% 400 == 0 \lor (Year \% 100 != 0 \land Year \% 4 == 0)$) which injects an extra day into February, expanding a **Leap Year to 366 days**.
    
- **The Tabular Hijri Track (Lunar):** Modeled around the synodic lunar month ($\approx 29.5306$ days). It uses a standard baseline of **354 days** divided into alternating 30 and 29-day months. To reconcile the fractional day drift ($\approx 11$ days shorter than the solar year), it applies a 30-year cyclic shift containing 11 specialized leap years—historically referred to as **The Karimah (Noble/Leap) Year (السنة الكريمة)**—which expands the cycle layer to **355 days** by adding a 30th day to _Zhu al-Hijjah_.
    

### 2. The Runtime Adjustment & Manual Offset Engine

To bypass the limitations of pure mathematical calculation without binding the software to constant web-API queries, the system introduces a **Manual Offset Architecture**:

- **Mechanism:** The runtime state tracks `GlobalHijriOffset` (bounded between $-3$ and $+3$ days). This integer value acts as an absolute linear shift vector applied directly to the intermediary absolute day count ($AbsHijri = AbsGreg - 227015 + HijriOffset$).
    
- **Utility:** This allows end-users or automated operators to alter the calculated timeline instantly to match localized optical moon sightings declared by regional authorities, ensuring immediate synchronicity without rewriting or recompiling the underlying astronomical formula.
    

### 3. File-Based System Settings (Configuration Persistence)

The state layer implements persistent configuration handling using text-based disk profiles:

- `HijriOffset.txt`: Retains the manual operational shift integer across application boots.
    
- `ShowHijriDayName.txt`: Maintains user preference flags regarding nominal day presentation verbosity. If these parameters are missing during startup compilation execution, the engine auto-initializes safe defaults ($0$ offset, and $true$ verbosity) and creates the configuration files dynamically.
    

## 🏗️ Architectural Class & File Blueprint

Plaintext

```
┌──────────────────────────────────────────────────────────┐
│                     MyGlobalTypes.h                      │
│   Central Type System: stDate, stPeriod, enDateFormat    │
└────────────┬────────────────────────────────┬────────────┘
             │                                │
             ▼                                ▼
┌────────────────────────┐       ┌────────────────────────┐
│      MyDateLib.h       │       │    MyHijriDateLib.h    │
│  • Tomohiko Sakamoto   │       │  • Tabular 30-Yr Matrix│
│  • Solar Leap (366d)   │       │  • Karimah Leap (355d) │
└────────────────────────┘       └────────────┬───────────┘
             │                                │
             │     Cross-Validation Callback  │
             └───────────────────────────────►│
                                              ▼
                                 ┌────────────────────────┐
                                 │      MyInputLib.h      │
                                 │   Stream Recovery Guard│
                                 └────────────┬───────────┘
                                              ▼
                                 ┌────────────────────────┐
                                 │        main.cpp        │
                                 │ State Loop & Disk I/O  │
                                 └────────────────────────┘
```

### Module Breakdown

1. **`MyGlobalTypes.h`:** Eliminates compilation coupling and circular dependencies. Declares separate structures for `stPeriod` and `stHijriPeriod` to prevent cross-contamination of time intervals during database evaluations.
    
2. **`MyDateLib.h`:** Houses Gregorian computation routines. Employs a zero-lookup variation of **Zeller's Congruence (Sakamoto's Variant)** to resolve day-of-week orders in $O(1)$ constant time.
    
3. **`MyHijriDateLib.h`:** Houses Lunar computation routines. Features `ConvertGregorianToHijriWeekdayCorrect()`, which maps the tabular date approximation to the exact absolute weekday index computed by the authoritative Gregorian baseline, eliminating structural 1-day calendar slips.
    
4. **`MyInputLib.h`:** Input-stream validation shield. Explicitly intercepts malformed keystrokes or string injections by trapping `cin.fail()`, executing stream flushes, and discarding buffer artifacts to eliminate infinite console loops.
    

## 🚀 Compiling and Running the Engine

The library is strictly self-contained and does not require external build systems or shared library links.

### Compile using GCC (C++11 standard or higher):

```Bash
g++ -std=c++11 Dual-Calendar-System-MiladiAndHijiri.cpp -o CalendarSystem
```

### Execution:

```Bash
./CalendarSystem
```
