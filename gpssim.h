#ifndef GPSSIM_H
#define GPSSIM_H

//#define FLOAT_CARR_PHASE // For RKT simulation. Higher computational load, but smoother carrier phase.

#define TRUE	(1)
#define FALSE	(0)

/*! \brief Maximum length of a line in a text file (RINEX, motion) */
#define MAX_CHAR (200)

/*! \brief Maximum number of satellites in RINEX file */
#define MAX_SAT (32)

/*! \brief Maximum number of channels we simulate */
#define MAX_CHAN (16)

/*! \brief Maximum number of user motion points */
#ifndef USER_MOTION_SIZE
#define USER_MOTION_SIZE (3000) // max duration at 10Hz
#endif

/*! \brief Maximum duration for static mode*/
#define STATIC_MAX_DURATION (86400) // second // Are we sure this is SECOND?

/*! \brief Number of subframes */
#define N_SBF (5) // 5 subframes per frame

/*! \brief Number of words per subframe */
#define N_DWRD_SBF (10) // 10 word per subframe

/*! \brief Number of words */
#define N_DWRD ((N_SBF + 1) * N_DWRD_SBF) // Subframe word buffer size

/*! \brief C/A code sequence length */
// C/A: Coarse/Acquisition code - Used to approximate the pseudorange and for signal acquisition
#define CA_SEQ_LEN (1023)

#define SECONDS_IN_WEEK 604800.0
#define SECONDS_IN_HALF_WEEK 302400.0
#define SECONDS_IN_DAY 86400.0
#define SECONDS_IN_HOUR 3600.0
#define SECONDS_IN_MINUTE 60.0

#define POW2_M5  0.03125
#define POW2_M19 1.907348632812500e-6
#define POW2_M29 1.862645149230957e-9
#define POW2_M31 4.656612873077393e-10
#define POW2_M33 1.164153218269348e-10
#define POW2_M43 1.136868377216160e-13
#define POW2_M55 2.775557561562891e-17

#define POW2_M50 8.881784197001252e-016
#define POW2_M30 9.313225746154785e-010
#define POW2_M27 7.450580596923828e-009
#define POW2_M24 5.960464477539063e-008

// Conventional values employed in GPS ephemeris model (ICD-GPS-200)
#define GM_EARTH 3.986005e14
#define OMEGA_EARTH 7.2921151467e-5
#define PI 3.1415926535898

#define WGS84_RADIUS	6378137.0
#define WGS84_ECCENTRICITY 0.0818191908426
#define WGS84_ECCENTRICITY_SQ (WGS84_ECCENTRICITY * WGS84_ECCENTRICITY)

#define R2D 57.2957795131

#define SPEED_OF_LIGHT 2.99792458e8

/*! \brief C/A code frequency */
#define CODE_FREQ (1.023e6)

// NOTE: constexpr is a C23 feature. To compile, use -std=gnu2x flag on gcc 13 or later
/*! \brief Enumerate the indices for the carrier frequencies */
enum CARRY_FREQUENCIES_INDEX {
	CARR_L1_INDEX = 0,
	CARR_L2_INDEX = 1,
	CARR_L5_INDEX = 2,
	CARR_NUM_FREQS = 3 // Useful trick to keep track of the array size
};
/*! \brief Carrier frequencies for GPS L1, L2, and L5 bands indexed based on CARRY_FREQUENCIES_INDEX */
// If changing this, make sure to update PATH_LOSS_DB_OFFSETS as well
static constexpr double CARR_FREQS[CARR_NUM_FREQS] = {
	/*! \brief GPS L1 Carrier frequency */
	[CARR_L1_INDEX] = 1575.42e6,
	/*! \brief GPS L2 Carrier frequency */
	[CARR_L2_INDEX] = 1227.60e6,
	/*! \brief GPS L5 Carrier frequency */
	[CARR_L5_INDEX] = 1176.45e6
};
/*! \brief Pre-computed path loss offsets for GPS L1, L2, and L5 frequencies indexed based on CARRY_FREQUENCIES_INDEX */
static constexpr double PATH_LOSS_DB_OFFSETS[CARR_NUM_FREQS] = {
	// Follows the formula: 20 * log10(frequency) - 147.55
	// where frequency is in Hz
	// and the constant 147.55 is derived from the free-space path loss formula
	// at a reference distance of 1 meter and a reference frequency of 1 GHz.

	/*! \brief Path loss offset for GPS L1 frequency (20 * log10(CARR_FREQS[CARR_L1_FREQ]) - 147.55)*/
	[CARR_L1_INDEX] = 20.0 * 9.197396354 - 147.55,
	/*! \brief Path loss offset for GPS L2 frequency (20 * log10(CARR_FREQS[CARR_L2_FREQ]) - 147.55)*/
	[CARR_L2_INDEX] = 20.0 * 9.089056879 - 147.55,
	/*! \brief Path loss offset for GPS L5 frequency (20 * log10(CARR_FREQS[CARR_L5_FREQ]) - 147.55)*/
	[CARR_L5_INDEX] = 20.0 * 9.070573474 - 147.55
};
/*! \brief Pre-computed wavelengths for GPS L1, L2, and L5 frequencies indexed based on CARRY_FREQUENCIES_INDEX */
static constexpr double LAMBDA[CARR_NUM_FREQS] = {
	/*! \brief Wavelength of GPS L1 frequency (SPEED_OF_LIGHT / CARR_FREQS[CARR_L1_FREQ]) */
	[CARR_L1_INDEX] = SPEED_OF_LIGHT / CARR_FREQS[CARR_L1_INDEX],
	/*! \brief Wavelength of GPS L2 frequency (SPEED_OF_LIGHT / CARR_FREQS[CARR_L2_FREQ]) */
	[CARR_L2_INDEX] = SPEED_OF_LIGHT / CARR_FREQS[CARR_L2_INDEX],
	/*! \brief Wavelength of GPS L5 frequency (SPEED_OF_LIGHT / CARR_FREQS[CARR_L5_FREQ]) */
	[CARR_L5_INDEX] = SPEED_OF_LIGHT / CARR_FREQS[CARR_L5_INDEX]
};
/*! \brief Pre-computed carrier to code frequency ratios for GPS L1, L2, and L5 frequencies indexed based on CARRY_FREQUENCIES_INDEX */
static constexpr double CARR_TO_CODE[CARR_NUM_FREQS] = {
	/*! \brief Carrier to code frequency ratio for GPS L1 frequency (1.0 / (CARR_FREQS[CARR_L1_FREQ] / CODE_FREQ)) */
	[CARR_L1_INDEX] = 1.0 / (CARR_FREQS[CARR_L1_INDEX] / CODE_FREQ),
	/*! \brief Carrier to code frequency ratio for GPS L2 frequency (1.0 / (CARR_FREQS[CARR_L2_INDEX] / CODE_FREQ)) */
	[CARR_L2_INDEX] = 1.0 / (CARR_FREQS[CARR_L2_INDEX] / CODE_FREQ),
	/*! \brief Carrier to code frequency ratio for GPS L5 frequency (1.0 / (CARR_FREQS[CARR_L5_INDEX] / CODE_FREQ)) */
	[CARR_L5_INDEX] = 1.0 / (CARR_FREQS[CARR_L5_INDEX] / CODE_FREQ)
};

// Sampling data format
enum SAMPLE_DATA_FORMAT {
	SC01 = 1,
	SC08 = 8,
	SC16 = 16
};

#define EPHEM_ARRAY_SIZE (15) // for daily GPS broadcast ephemers file (brdc)

/*! \brief Structure representing GPS time */
typedef struct
{
	int week;	/*!< GPS week number (since January 1980) */
	double sec; 	/*!< second inside the GPS \a week */
} gpstime_t;
static constexpr gpstime_t DEFAULT_GPSTIME = {
	.week = -1,
	.sec = 0.0
};

/*! \brief Structure repreenting UTC time */
typedef struct
{
	int y; 		/*!< Calendar year */
	int m;		/*!< Calendar month */
	int d;		/*!< Calendar day */
	int hh;		/*!< Calendar hour */
	int mm;		/*!< Calendar minutes */
	double sec;	/*!< Calendar seconds */
} datetime_t;


typedef struct
{
	int enable;
	int vflg;
	double alpha0, alpha1, alpha2, alpha3;
	double beta0, beta1, beta2, beta3;
	double A0, A1;
	int dtls, tot, wnt;
	int dtlsf, dn, wnlsf;
	// enable custom leap event
	int leapen;
} ionoutc_t;
static constexpr ionoutc_t DEFAULT_IONOUTC = {
	.enable = TRUE, // Set before args are read. By default, ionospheric correction is enabled
	.vflg = FALSE,
	.alpha0 = 0.0,
	.alpha1 = 0.0,
	.alpha2 = 0.0,
	.alpha3 = 0.0,
	.beta0 = 0.0,
	.beta1 = 0.0,
	.beta2 = 0.0,
	.beta3 = 0.0,
	.A0 = 0.0,
	.A1 = 0.0,
	.dtls = 0,
	.tot = 0,
	.wnt = 0,
	.dtlsf = 0,
	.dn = 0,
	.wnlsf = 0,
	.leapen = FALSE // Set before args are read
};

/*! \brief Structure representing ephemeris of a single satellite */
typedef struct
{
	int vflg;	/*!< Valid Flag */
	datetime_t t;
	gpstime_t toc;	/*!< Time of Clock */
	gpstime_t toe;	/*!< Time of Ephemeris */
	int iodc;	/*!< Issue of Data, Clock */
	int iode;	/*!< Isuse of Data, Ephemeris */
	double deltan;	/*!< Delta-N (radians/sec) */
	double cuc;	/*!< Cuc (radians) */
	double cus;	/*!< Cus (radians) */
	double cic;	/*!< Correction to inclination cos (radians) */
	double cis;	/*!< Correction to inclination sin (radians) */
	double crc;	/*!< Correction to radius cos (meters) */
	double crs;	/*!< Correction to radius sin (meters) */
	double ecc;	/*!< e Eccentricity */
	double sqrta;	/*!< sqrt(A) (sqrt(m)) */
	double m0;	/*!< Mean anamoly (radians) */
	double omg0;	/*!< Longitude of the ascending node (radians) */
	double inc0;	/*!< Inclination (radians) */
	double aop;
	double omgdot;	/*!< Omega dot (radians/s) */
	double idot;	/*!< IDOT (radians/s) */
	double af0;	/*!< Clock offset (seconds) */
	double af1;	/*!< rate (sec/sec) */
	double af2;	/*!< acceleration (sec/sec^2) */
	double tgd;	/*!< Group delay L2 bias */
	int svhlth;
	int codeL2;
	// Working variables follow
	double n; 	/*!< Mean motion (Average angular velocity) */
	double sq1e2;	/*!< sqrt(1-e^2) */
	double A;	/*!< Semi-major axis */
	double omgkdot; /*!< OmegaDot-OmegaEdot */
} ephem_t;

typedef struct
{
    gpstime_t g;
    double range;       // pseudorange
    double rate;
    double d;           // geometric distance
    double azel[2];
    double iono_delay;
    double los_ecef[3]; // unit LOS vector, receiver→satellite, ECEF frame
} range_t;

/*! \brief Structure representing a Channel */
typedef struct
{
    int prn;    /*< PRN Number */

    int ca[CA_SEQ_LEN]; /*< C/A Sequence */

    double f_carr;  /*< Carrier frequency */
    double f_code;  /*< Code frequency */

#ifdef FLOAT_CARR_PHASE
    double carr_phase;
#else
    unsigned int carr_phase;      /*< Carrier phase (32-bit accumulator) */
    int carr_phasestep_int;       /*< Fixed-point Carrier phase step */
    int carr_phase_accel_int;     /*< Fixed-point Carrier phase acceleration */
#endif
    
    double code_phase; /*< Code phase */
    gpstime_t g0;   /*!< GPS time at start */

    unsigned long sbf[5][N_DWRD_SBF]; /*!< current subframe */
    unsigned long dwrd[N_DWRD]; /*!< Data words of sub-frame */
    int iword;  /*!< initial word */
    int ibit;   /*!< initial bit */
    int icode;  /*!< initial code */
    int dataBit;    /*!< current data bit */
    int codeCA; /*!< current C/A code */
    double azel[2];
    range_t rho0;

    double carr_phase_step;  
    double carr_phase_accel; 
} channel_t;

typedef struct
{
    double gain[361][181];
} custom_antenna_model_t;

/*! \brief Structure representing the runtime configuration of the simulation */
typedef struct
{
	/* 
	 * File name of the RINEX navigation file, which contains the ephemeris data for the GPS satellites.
	 * The file should be in RINEX 3 format and contain the broadcast ephemeris for the satellites to be simulated.
	 * The ephemeris data will be used to compute the satellite positions and velocities at each time step of the simulation.
	 */
	char navfile[MAX_CHAR];

	/* 
	 * File name of the user motion input file, which contains the trajectory and optionally attitude of the user receiver.
	 * The file should be a text file with either 4 columns (time, x, y, z) or 8 columns (time, x, y, z, qw, qx, qy, qz).
	 * The time column should be in seconds since the start of the simulation.
	 * The position columns should be in ECEF coordinates (meters).
	 * The attitude columns (if present) should be in quaternion format (qw, qx, qy, qz).
	 */
	char umfile[MAX_CHAR];

	/* 
	 * File name of the output I/Q sampling data, which will contain the simulated GPS signal samples.
	 * The file will be in binary format and contain interleaved I and Q samples according to the specified data format (1-bit, 8-bit, or 16-bit).
	 * The output file can be used for testing GPS signal processing algorithms or for playback in software-defined radio applications.
	 * @default "gpssim.bin"
	 */
	char outfile[MAX_CHAR];

	/*
	 * File name of the custom antenna model CSV file
	 */
	char antenna_model_file[MAX_CHAR];

	/* Pointer to the loaded custom antenna model */
	custom_antenna_model_t *antenna_model;

	/* 
	 * Integer value to keep track of which frequency band to simulate.
	 * The value is set based on the -f flag, which can be L1, L2, or L5.
	 * @default 2.6e6 Hz 
	 */
	double sample_frequency; // consider making this an int to avoid floating point issues

	/*
	 * Double value to keep track of the simulation duration.
	 * The value is set based on the -d flag.
	 * @default is 300 seconds
	 */
	double max_duration;

	/* 
	 * Integer value to keep track of which frequency band to simulate.
	 * The value is set based on the -f flag, which can be L1, L2, or L5.
	 * @default L1 frequency 
	 */
	int carr_freq_index;

	/* 
	 * Integer value to keep track of the I/Q data format.
	 * The value is set based on the -b flag, which can be 1, 8, or 16.
	 * @default 16-bit I/Q data format 
	 */
	int data_format;

	/*
	 * Structure representing the ionospheric model parameters.
	 * The parameters are read from the RINEX navigation file and stored in this structure for use in ionospheric delay calculations during the simulation.
	 * The structure includes the alpha and beta parameters for the Klobuchar ionospheric model, as well as the A0 and A1 parameters for the ionospheric delay linear model.
	 * The structure also includes flags to indicate whether the ionospheric parameters are valid and whether the ionospheric correction should be applied in the simulation.
	 * The ionospheric model parameters are used to compute the ionospheric delay for each satellite signal at each time step of the simulation, which affects the pseudorange and carrier phase measurements at the user receiver.
	 * The ionospheric model can be disabled with the -i flag, in which case no ionospheric delay will be applied to the simulated signals.
	 * @default All parameters initialized to zero/FALSE, and enable flag set to TRUE (ionospheric correction enabled by default)
	 */
	ionoutc_t ionosphere_model;

	/* 
	 * Boolean value to keep track of whether attitude data is available in the user motion file.
	 * Automatically detected by the `readUserMotion` function.
	 * The value is set to true if the user motion file contains 8 columns (time, x, y, z, qw, qx, qy, qz).
	 * @default false
	 */
	int has_attitude;

	/* 
	 * Boolean value to enable path loss calculation, which applies a distance-based attenuation to the signal strength.
	 * The value is set to false with the -p flag, which holds the power level constant regardless of distance.
	 * @default true 
	 */
	int path_loss_enable;
	/*
	 * Integer value to set the fixed gain for the simulation.
	 * The value is set based on the -p flag, which can be a value between 1 and 128.
	 * Only used if path_loss_enable is set to false.
	 * @default 128 
	 */
	int fixed_gain;

	/*
	 * Boolean value to enable overwriting the TOC and TOE in the RINEX navigation file with the scenario start time.
	 * This can be useful for simulating scenarios that do not align with the ephemeris reference times, such as spacecraft scenarios.
	 * The value is set to true with the -T flag.
	 * @default false 
	 */
	int time_overwrite; // probably not needed because it's never used, but keeping it for the sake of its existence

	/*
	 * Boolean value to enable input of user position in ECEF x, y, z format.
	 * The value is set to true with the -u flag.
	 * The value is set to false if the -g, -c, or -l flag is used.
	 * @default false 
	 */
	int um_ECEF;

	/* 
	 * Boolean value to enable input of user position in Lat/Long/Height format.
	 * The value is set to false with the -u flag.
	 * The value is set to true if the -l flag is used.
	 * @default false 
	 */
	int um_LLH;

	/* 
	 * Boolean value to enable input of user position in NMEA GGA format 
	 * The value is set to false with the -u flag.
	 * The value is set to true if the -g flag is used.
	 * @default false 
	 */
	int um_nmeaGGA;

	/* 
	 * Boolean value to enable static location mode in ECEF, where the user is assumed to be stationary 
	 * at the initial position specified in the arguments.
	 * The value is set to true with the -c flag.
	 * @default false 
	 */
	int static_ECEF;

	/* 
	 * Boolean value to enable static location mode in LLH, where the user is assumed to be stationary 
	 * at the initial position specified in the arguments.
	 * The value is set to true with the -l flag.
	 * @default false 
	 */
	int static_LLH;

	/*
	 * GPS time structure to represent the start time of the simulation.
	 * The GPS time is represented by a week number and seconds into the week.
	 */
	gpstime_t start_time;

	/* 
	 * Boolean value to enable verbose output.
	 * The value is set to true with the -v flag.
	 * @default false 
	 */
	int verbose;
} sim_config_t;
static const sim_config_t DEFAULT_SIM_CONFIG = { // should be constexpr but C/C++ extention doesen't like it
	.navfile = "",
	.umfile = "",
	.outfile = "gpssim.bin",
	.antenna_model_file = "",
	.antenna_model = NULL,

	.sample_frequency = 2.6e6,
	.max_duration = USER_MOTION_SIZE / 10.0, // Default duration based on user motion array size and 10Hz sampling rate

	.carr_freq_index = CARR_L1_INDEX,
	.data_format = SC16,
	.ionosphere_model = DEFAULT_IONOUTC,

	.time_overwrite = FALSE,
	.has_attitude = FALSE,

	.path_loss_enable = TRUE,
	.fixed_gain = 128,

	.um_ECEF = FALSE,
	.um_LLH = FALSE,
	.um_nmeaGGA = FALSE,
	.static_ECEF = FALSE,
	.static_LLH = FALSE,

	.start_time = DEFAULT_GPSTIME,

	.verbose = FALSE,
};

typedef struct {
	/*
	 * Ephemeris data arrays, which store the orbital parameters of the satellites.
	 * Indexed by ephemeris set and satellite PRN number.
	 */
	ephem_t eph[EPHEM_ARRAY_SIZE][MAX_SAT];
	int eph_count;
	int current_eph_index;

	/* 
	 * User motion data arrays, which store the trajectory and optionally attitude of the user receiver.
	 * The xyz array contains the position of the user in ECEF coordinates (meters) at each time step.
	 * The quat array contains the attitude of the user in quaternion format (qw, qx, qy, qz) at each time step, if available.
	 * The number of valid records in these arrays is determined by the numd variable, which is set based on the user motion file or static mode duration.
	 * The arrays are indexed by time step, where the time step is determined by the sampling frequency and the duration of the simulation.
	 * The maximum size of these arrays is defined by USER_MOTION_SIZE, which should be large enough to accommodate the desired simulation duration at the given sampling frequency.
	 */
	double xyz[USER_MOTION_SIZE][3]; // consider moving these to their own struct?
	double quat[USER_MOTION_SIZE][4];   // [qw, qx, qy, qz]  ECEF -> body


	/* 
	 * Integer array to keep track of which satellites have been allocated to channels.
	 * The value is set to 1 for satellites that are currently allocated to a channel, and 0 for those that are not.
	 * This array is used internally by the `allocateChannel` function to ensure that each satellite is only allocated to one channel at a time.
	 * Indexed by PRN number (1-32) 
	 */
	int allocatedSat[MAX_SAT];

	/*
	 * Array to store the channel information for each active channel.
	 */
	channel_t channels[MAX_CHAN];

	/* 
	 * GPS time structure to represent the current GPS time at each time step of the simulation.
	 * This variable is updated at each time step based on the start time and the elapsed time since the start of the simulation.
	 * The current GPS time is used as a reference for computing satellite positions and user positions at each time step, as well as for determining the validity of the ephemeris data for the current time.
	 * The current GPS time is also used in the generation of the navigation message and in the computation of the ionospheric delay for each satellite signal.
	 */
	gpstime_t current_gps_time;

	/* 
	 * Buffer to store the generated I/Q samples for each time step of the simulation.
	 * The buffer is allocated based on the specified data format (1-bit, 8-bit, or 16-bit) and the number of samples per time step, which is determined by the sampling frequency.
	 * The buffer is used to store the interleaved I and Q samples for all active channels at each time step before writing them to the output file.
	 * The size of the buffer in bytes is stored in the iq_buff_bytes variable, and the total number of samples that can be stored in the buffer is stored in the iq_buff_size variable.
	 */
	short *iq_buff;

	/* 
	 * Integer variable to store the size of the I/Q buffer in bytes.
	 * This variable is computed based on the specified data format and the number of samples per time step.
	 * The value of this variable is used for memory allocation and for writing the correct number of bytes to the output file at each time step.
	 */
	int iq_buff_bytes;
	/* 
	 * Integer variable to store the total number of samples that can be stored in the I/Q buffer.
	 * This variable is computed based on the sampling frequency and the duration of each time step (e.g., 0.1 seconds).
	 * The value of this variable is used for memory allocation and for iterating through the buffer when generating and writing the I/Q samples at each time step.
	 */
	int iq_buff_size;

	/*
	 * File pointer for the output I/Q sampling data file.
	 * This file is opened during the initialization phase of the simulation and is used to write the simulated I/Q samples at each time step.
	 * The file is closed at the end of the simulation after all samples have been written.
	 * The file is opened in binary mode and the samples are written according to the specified data format (1-bit, 8-bit, or 16-bit).
	 */
	FILE *fp_out;
} sim_state_t;
static const sim_state_t DEFAULT_SIM_STATE = {
	.eph = {{{0}}}, // Initialize all ephemeris data to zero
	.eph_count = -1,
	.current_eph_index = -1,

	.xyz = {{0.0, 0.0, 0.0}}, // Initialize all positions to zero
	.quat = {[0 ... USER_MOTION_SIZE - 1] = {1.0, 0.0, 0.0, 0.0}}, // Initialize all quaternions to identity (no rotation)
	
	.allocatedSat = {[0 ... MAX_SAT - 1] = -1}, // Initialize all to -1 (not allocated)
	.channels = {{0}}, // Initialize all channel data to zero

	.current_gps_time = DEFAULT_GPSTIME,

	.iq_buff = NULL,
	.iq_buff_bytes = 0,
	.iq_buff_size = 0,

	.fp_out = NULL
};

#endif
