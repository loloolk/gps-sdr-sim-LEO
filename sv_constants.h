#ifndef SV_CONSTANTS_H
#define SV_CONSTANTS_H

// Total number of SVNs in the GPS constellation as of May 20 2026
#define TOTAL_SV_COUNT (83)

// Number of Active SVs in the GPS constellation as of May 20 2026
#define ACTIVE_SV_COUNT (32)

// Minumum signal received acording to IS-GPS-200
#define MINIMUM_RECEIVED_DBW -158.5

// Assumed component of power not used for C/A signal (in dbw)
#define NON_CA_POWER_COMPONENT_L1 1.76

enum BLOCK_TYPE {
    INVALID = 0,
    IIR = 1,
    IIR_M = 2,
    IIF = 3,
    III = 4,
    BLOCK_TYPE_COUNT = 5
};

/*! \brief Mapping of PRN to SVN as of May 20 2026 */
static constexpr int PRN_TO_SVN[ACTIVE_SV_COUNT + 1] = {
    [0] = 0,
    [1] = 80,
    [2] = 61,
    [3] = 69,
    [4] = 74,

    [5] = 50,
    [6] = 67,
    [7] = 48,
    [8] = 72,
    [9] = 68,
    
    [10] = 73,
    [11] = 78,
    [12] = 58,
    [13] = 43,
    [14] = 77,

    [15] = 55,
    [16] = 56,
    [17] = 53,
    [18] = 75,
    [19] = 59,

    [20] = 82,
    [21] = 81,
    [22] = 44,
    [23] = 76,
    [24] = 65,

    [25] = 62,
    [26] = 71,
    [27] = 66,
    [28] = 79,
    [29] = 57,

    [30] = 64,
    [31] = 52,
    [32] = 70
};

// According to the folliwing link, as of May 20 2026
// https://www.navcen.uscg.gov/gps-constellation?order=field_gps_svn&sort=asc
/*! \brief Mapping of SVN to Block Type */
static constexpr enum BLOCK_TYPE SVN_TO_BLOCK_TYPE[TOTAL_SV_COUNT] = {
    [43] = IIR,
    [44] = IIR,
    [48] = IIR_M,
    [50] = IIR_M,
    [52] = IIR_M,
    [53] = IIR_M,
    [55] = IIR_M,
    [56] = IIR,
    [57] = IIR_M,
    [58] = IIR_M,
    [59] = IIR,
    [61] = IIR,
    [62] = IIF,
    [64] = IIF,
    [65] = IIF,
    [66] = IIF,
    [67] = IIF,
    [68] = IIF,
    [69] = IIF,
    [70] = IIF,
    [71] = IIF,
    [72] = IIF,
    [73] = IIF,
    [74] = III,
    [75] = III,
    [76] = III,
    [77] = III,
    [78] = III,
    [79] = III,
    [80] = III,
    [81] = III,
    [82] = III
};

/*! \brief Gain correction factors in dB for GPS L1 frequency for each SVN in the constellation as of May 20 2026 */
static constexpr double GAIN_CORRECTION_FACTOR_L1[TOTAL_SV_COUNT] = {
    // The following were gotten from https://onlinelibrary.wiley.com/doi/epdf/10.1002/navi.123?saml_referrer
    [41] = -0.9, [43] = -0.9, [44] = -1.1, [45] = -1.1, [46] = -1.0, [47] = -1.3, [48] = -1.4, [49] = -1.3, [50] = -1.3, [51] = -0.7, 
    [52] = -1.2, [53] = -1.4, [54] = -0.8, [55] = -1.3, [56] = -0.7, [57] = -1.3, [58] = -1.3, [59] = -1.3, [60] = -1.3, [61] = -1.2,

    // All of III will be assumed to be -1.0, as done in https://www.sciencedirect.com/science/article/pii/S0273117723009493
    [74] = -1.0, [75] = -1.0, [76] = -1.0, [77] = -1.0, [78] = -1.0, [79] = -1.0, [80] = -1.0, [81] = -1.0, [82] = -1.0,

    // And might as well do the same for all of IIF, since I cant find any information on those gain correction factors
    [62] = -1.0, [64] = -1.0, [65] = -1.0, [66] = -1.0, [67] = -1.0, [68] = -1.0, [69] = -1.0, [70] = -1.0, [71] = -1.0, [72] = -1.0,
    [73] = -1.0
};

/*! \brief Number of angles for which the transmitter gain pattern is (approximately) radially symmetric */
#define TRANSMITTER_DIRECTIVITY_ACCURACY_L1 21

// Values approximated from the following links as of May 20 2026
// IIR and IIR_M: https://www.navcen.uscg.gov/sites/default/files/pdf/gps/GPS_Block_IIR_IIR_M_Antenna_Panel_Pattern_Marquis_Aug2015_revised.pdf
// IIF: https://www.navcen.uscg.gov/gps-technical-references (downloaded each satellite's antenna pattern and averaged the gain values across the satellites of the same block type)
// Potentially useful paper for expanding this table
// https://ntrs.nasa.gov/api/citations/20180006897/downloads/20180006897.pdf
/*! \brief Transmitter directivity pattern for GPS L1 frequency */
static constexpr double TRANSMITTER_DIRECTIVITY_PATTERN_L1[BLOCK_TYPE_COUNT][TRANSMITTER_DIRECTIVITY_ACCURACY_L1] = {
    [INVALID] = {0.0}, // Default to 0 dB gain for invalid block types
    [IIR] =   {12.64, 12.83, 12.96, 13.27, 13.58, 14.28, 14.65, 15.28, 15.53, 15.79, 16.10, 15.85, 15.60, 14.65, 14.34, 12.89, 12.20,  9.05,  6.92,  4.40, 2.77},
    [IIR_M] = {14.81, 14.68, 14.61, 14.55, 14.36, 14.55, 14.68, 15.06, 15.38, 15.70, 16.02, 16.15, 16.09, 15.77, 15.25, 14.49, 13.78, 12.18, 10.51,  8.52, 6.92},
    [IIF] =   {14.88, 14.92, 14.97, 15.02, 15.13, 15.25, 15.37, 15.48, 15.54, 15.55, 15.48, 15.35, 15.07, 14.67, 14.20, 13.55, 12.75, 11.71, 10.46,  8.92, 7.08},
    [III] =   {12.32, 12.39, 12.46, 12.48, 12.50, 12.78, 13.05, 13.58, 14.10, 14.58, 15.07, 15.30, 15.53, 15.41, 15.29, 14.76, 14.23, 13.20, 12.16, 10.55, 8.94}
};

// These are all assumptions since I couldnt find the actual values
static constexpr double AMPLIFIER_TRANSMIT_POWER_L1[BLOCK_TYPE_COUNT] = {
    [INVALID] = 0.0, // Default to 0 dBW for invalid block types
    [IIR] = 15.0,
    [IIR_M] = 14.3,
    [IIF] = 14.3,
    [III] = 14.5
};

#endif // SV_CONSTANTS_H
