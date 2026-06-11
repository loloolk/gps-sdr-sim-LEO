# GPS-SDR-SIM

GPS-SDR-SIM generates GPS baseband signal data streams, which can be converted to RF using software-defined radio (SDR) platforms. This version is enhanced for Low Earth Orbit (LEO) simulation and supports custom antenna models.

Supported platforms include [ADALM-Pluto](https://wiki.analog.com/university/tools/pluto), [bladeRF](http://nuand.com/), [HackRF](https://github.com/mossmann/hackrf/wiki), and [USRP](http://www.ettus.com/).

---

## 🛠 Building the Project

### Windows Build Instructions
1. Start Visual Studio.
2. Create an empty project for a console application.
3. On the Solution Explorer, add `gpssim.c` and `getopt.c` to the **Source Files** folder.
4. Select **Release** in the Solution Configurations dropdown.
5. Build the solution.

### Building with GCC
```bash
$ gcc gpssim.c -lm -O3 -std=gnu2x -o gps-sdr-sim
```
Alternatively, use the provided `Makefile`:
```bash
$ make
```

### Customizing Buffer Size
For user motion files with more than 30,000 samples (at 10Hz), set the `USER_MOTION_SIZE` variable to the maximum duration in seconds.
```bash
$ make USER_MOTION_SIZE=4000
```
Or via GCC:
```bash
$ gcc gpssim.c -lm -O3 -std=gnu2x -o gps-sdr-sim -DUSER_MOTION_SIZE=4000
```

---

## 🚀 Usage & Options

```text
Usage: gps-sdr-sim [options]
Options:
  -e <gps_nav>     RINEX navigation file for GPS ephemerides (required)
  -u <user_motion> User motion file in ECEF x, y, z format (dynamic mode)
  -x <user_motion> User motion file in lat, lon, height format (dynamic mode)
  -g <nmea_gga>    NMEA GGA stream (dynamic mode)
  -c <location>    ECEF X,Y,Z in meters (static mode) e.g. 3967283.154,1022538.181,4872414.484
  -l <location>    Lat, lon, height (static mode) e.g. 35.681298,139.766247,10.0
  -L <wnslf,dn,dtslf> User leap future event in GPS week number, day number, next leap second
  -t <date,time>   Scenario start time YYYY/MM/DD,hh:mm:ss
  -T <date,time>   Overwrite TOC and TOE to scenario start time
  -d <duration>    Duration [sec] (dynamic max: 300, static max: 86400)
  -o <output>      I/Q sampling data file (default: gpssim.bin)
  -s <frequency>   Sampling frequency [Hz] (default: 2600000)
  -f <frequency>   Supported carrier frequency [L1/L2/L5] (default: L1)
  -b <iq_bits>     I/Q data format [1/8/16] (default: 16)
  -i               Disable ionospheric delay (useful for spacecraft scenarios)
  -p [fixed_gain]  Disable path loss and hold power level constant
  -A <csv_file>    Custom antenna model CSV file (phi, theta, gain)
  -v               Show details about simulated channels
```

---

## 📡 Simulation Modes

### Static Mode
The receiver position is fixed. Specify location using `-c` (ECEF) or `-l` (Lat/Lon/Height).

### Dynamic Mode
Specify a trajectory via a CSV (ECEF or LLH) or NMEA GGA stream. The sampling rate must be **10Hz**.
CSV files can include optional quaternion columns (`qw, qx, qy, qz`) to specify antenna attitude. If omitted, the antenna points towards the zenith.

### Custom Antenna Models
You can provide a custom antenna gain model using the `-A` flag. The model must be a CSV with three columns: `phi` (-180 to 180), `theta` (0 to 180), and `gain` (dB).
Example:
```bash
./gps-sdr-sim -e brdc0010.22n -u motion.csv -A antenna_models/my_antenna.csv
```
Gain values are mapped to the nearest integer degree. If no model is provided, the simulation falls back to the default internal pattern.

---

## 📁 Repository Structure

- `gpssim.c` / `gpssim.h`: Core simulation logic.
- `antenna_models/`: Directory for custom antenna gain patterns.
- `sdr_tools/`: Contains scripts and tools for various SDR platforms (BladeRF, HackRF, USRP, etc.).
- `images/`: Project screenshots and documentation images.
- `input_files/`: Sample trajectory and ephemeris files.
- `outputs/`: Sample GNSS-SDR configuration files.

---

## 📻 SDR Integration

Playback requirements vary by device:
- **BladeRF / ADALM-Pluto**: Requires 16-bit signed integers (`-b 16`).
- **HackRF / USRP**: Supports signed bytes (`-b 8`).
- **Sample Rate**: Most devices accept 2.6MHz; USRP2 requires 2.5MHz (`-s 2500000`).

Scripts for playback can be found in the `sdr_tools` directory. For example, to use BladeRF:
```bash
> bladeRF-cli -s sdr_tools/bladerf.script
```

---

## ⚖️ License

Copyright &copy; 2026 Marc Oger  
Copyright &copy; 2015-2025 Takuji Ebinuma  
Distributed under the [MIT License](http://www.opensource.org/licenses/mit-license.php).
