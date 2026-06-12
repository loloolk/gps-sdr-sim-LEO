# GPS-SDR-SIM (LEO Edition)

GPS-SDR-SIM generates GPS baseband signal data streams, which can be transmitted as RF using a software-defined radio (SDR). This fork adds Low Earth Orbit (LEO) simulation support, quaternion-based antenna attitude control, and custom antenna gain models.

Supported SDR platforms: [ADALM-Pluto](https://wiki.analog.com/university/tools/pluto), [bladeRF](http://nuand.com/), [HackRF](https://github.com/mossmann/hackrf/wiki), [LimeSDR](https://limemicro.com/), and [USRP](http://www.ettus.com/).

---

## Requirements

- **GCC 13 or higher** — required for C23 `constexpr` support (`-std=gnu2x`). The build will fail with a clear error if your compiler is too old.
- A RINEX navigation file (broadcast ephemeris) is required for all simulations. Sample files are included in `input_files/ephemeris/`, and the Makefile includes rules to download daily ephemeris from the CDDIS FTP server.

---

## Building

### GCC (Linux / macOS / MinGW)

```bash
gcc gpssim.c -lm -O3 -std=gnu2x -o gps-sdr-sim
```

Or use the Makefile (recommended — it checks your GCC version first):

```bash
make
```

#### Extending the dynamic mode buffer

The default buffer holds 3000 samples (300 seconds at 10 Hz). To simulate longer trajectories, increase `USER_MOTION_SIZE` at build time:

```bash
make USER_MOTION_SIZE=4000
# or directly:
gcc gpssim.c -lm -O3 -std=gnu2x -o gps-sdr-sim -DUSER_MOTION_SIZE=4000
```

#### Benchmarking

```bash
make time
```

Runs a quick benchmark across all three I/Q formats (1-bit, 8-bit, 16-bit).

### Windows (Visual Studio)

1. Open Visual Studio and create an empty console application project.
2. Add `gpssim.c` and `getopt.c` to **Source Files**.
3. Set the configuration to **Release**.
4. Build the solution.

---

## Usage

```text
Usage: gps-sdr-sim [options]

Options:
  -e <gps_nav>          RINEX navigation file for GPS ephemerides (required)

  Input (exactly one required):
  -u <user_motion>      User motion file, ECEF x,y,z [m] at 10 Hz (dynamic)
  -x <user_motion>      User motion file, lat,lon,height [deg/m] at 10 Hz (dynamic)
  -g <nmea_gga>         NMEA GGA stream (dynamic)
  -c <X,Y,Z>            Static ECEF position in meters
                          e.g. 3967283.154,1022538.181,4872414.484
  -l <lat,lon,height>   Static geodetic position
                          e.g. 35.681298,139.766247,10.0

  Timing:
  -t <YYYY/MM/DD,hh:mm:ss>   Scenario start time (UTC)
  -T <YYYY/MM/DD,hh:mm:ss>   Overwrite ephemeris TOC/TOE to scenario start time
      (also accepts "now")
  -d <duration>         Duration in seconds
                          Dynamic max: 300 s | Static max: 86400 s (24 h)
  -L <wnslf,dn,dtslf>   User-defined leap second event:
                          GPS week number, day of week (1–7), delta leap seconds

  Signal:
  -s <frequency>        Sampling frequency in Hz (default: 2600000, min: 1000000)
  -f <L1|L2|L5>         Carrier frequency band (default: L1 = 1575.42 MHz)
  -b <1|8|16>           I/Q sample bit depth (default: 16)
  -i                    Disable ionospheric delay correction
                          (recommended for LEO / spacecraft scenarios)
  -p [1–128]            Disable path loss; optionally set a fixed gain value

  Antenna:
  -A <csv_file>         Custom antenna gain model (see Antenna Models section)

  Output:
  -o <output>           Output filename (default: gpssim.bin)
  -v                    Verbose: print per-channel simulation details
```

If no position or motion input is given, the simulator defaults to a static position in Tokyo (35.681298°N, 139.766247°E, 10 m).

---

## Simulation Modes

### Static Mode

The receiver position is fixed. Use `-c` (ECEF) or `-l` (lat/lon/height). Maximum duration is 86400 seconds (24 hours).

```bash
./gps-sdr-sim -e input_files/ephemeris/brdc0010.22n -l 35.681298,139.766247,10.0 -d 3600
```

### Dynamic Mode

A trajectory CSV file or NMEA GGA stream drives the receiver position at **10 Hz**. Maximum duration is 300 seconds (5 minutes) by default; increase `USER_MOTION_SIZE` at compile time to extend this.

**ECEF format** (`-u`):

```
time[s], x[m], y[m], z[m]
```

**LLH format** (`-x`):

```
time[s], lat[deg], lon[deg], height[m]
```

Both formats optionally accept four quaternion columns to specify antenna attitude:

```
time[s], x, y, z, qw, qx, qy, qz
```

The quaternion describes the rotation from the NEU frame to the antenna body frame. If the columns are absent, the antenna is assumed to point toward zenith. Quaternion support is not available in NMEA GGA mode.

### LEO Simulation

For LEO and spacecraft scenarios, disable the ionospheric delay model with `-i`. The included `LEO_gen.py` script generates circular orbit trajectories:

```bash
python LEO_gen.py            # writes input_files/LEO/LEO.csv
./gps-sdr-sim -e input_files/ephemeris/brdc0010.22n -u input_files/LEO/LEO.csv -i
```

Sample files with quaternion attitude data are provided in `input_files/elv_mask/from_leo.csv`.

---

## Antenna Models

Supply a custom antenna gain pattern with `-A`. The CSV must have three columns with integer-degree resolution:

| Column | Range | Unit |
|--------|-------|------|
| `phi` | −180 to 180 | degrees (azimuth) |
| `theta` | 0 to 180 | degrees (elevation) |
| `gain` | any | dB |

```bash
./gps-sdr-sim -e input_files/ephemeris/brdc0010.22n -u motion.csv -A antenna_models/antenna_model.csv
```

A pre-computed model (`antenna_models/antenna_model.csv`) is included. If your source data has coarser resolution, `antenna_models/interpolate_model.py` upsamples it to 1-degree resolution via bilinear interpolation. Points not covered by the model default to −1000 dB.

If no model is provided, the simulator uses the built-in per-block GPS satellite transmitter gain patterns.

---

## SDR Playback

| Platform | Required format | Sample rate |
|----------|----------------|-------------|
| bladeRF | 16-bit signed int (`-b 16`) | 2.6 MHz |
| ADALM-Pluto | 16-bit signed int (`-b 16`) | 2.6 MHz |
| HackRF | 8-bit signed byte (`-b 8`) | 2.6 MHz |
| LimeSDR | 16-bit signed int (`-b 16`) | 2.6 MHz |
| USRP | 8-bit signed byte (`-b 8`) | 2.5 MHz (`-s 2500000`) |

Playback tools and scripts live in `sdr_tools/`:

```bash
# bladeRF
bladeRF-cli -s sdr_tools/bladerf.script

# USRP (Python / UHD)
python sdr_tools/gps-sdr-sim-uhd.py -t gpssim.bin -s 2500000 -x 0

# Compiled players (bladeRF, HackRF, ADALM-Pluto, LimeSDR)
make -C sdr_tools/player
```

External clock configuration files for HackRF are in `sdr_tools/extclk/`.

---

## Repository Structure

```
gpssim.c / gpssim.h     Core simulation engine
getopt.c / getopt.h     Cross-platform CLI argument parsing
sv_constants.h          PRN→SVN mappings and per-block transmitter patterns
LEO_gen.py              Generates LEO orbit trajectory CSV files
Makefile                Build system (enforces GCC ≥ 13)
IS-GPS-200.pdf          GPS ICD reference document

antenna_models/         Custom antenna gain pattern files
  antenna_model.csv       Pre-computed gain pattern (1° resolution)
  interpolate_model.py    Upsamples coarser antenna data to 1°

input_files/
  ephemeris/              RINEX navigation files (brdc*.n)
  LEO/                    Sample LEO trajectory
  elv_mask/               Elevation mask CSVs (incl. quaternion example)
  *.csv / *.NMEA          Ground motion trajectory samples

sdr_tools/
  bladerf.script          bladeRF CLI playback script
  gps-sdr-sim-uhd.py      USRP/UHD playback script
  player/                 Source for compiled playback tools
  extclk/                 HackRF external clock config files

satgen/                 Satellite trajectory generation utilities
rtk/                    RTK base station reference files
outputs/                Sample GNSS-SDR configuration files
images/                 Documentation screenshots
```

---

## License

Copyright &copy; 2026 Marc Oger  
Copyright &copy; 2015-2025 Takuji Ebinuma  
Distributed under the [MIT License](http://www.opensource.org/licenses/mit-license.php).
