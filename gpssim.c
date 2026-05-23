#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#ifdef _WIN32
#include "getopt.h"
#else
#include <unistd.h>
#endif
#include "gpssim.h"
#include "sv_constants.h"

/* ------------- Static Global Variables ------------- */

// Sine table for 512 angles between 0 and 360
static int sinTable512[512] = {
	   2,   5,   8,  11,  14,  17,  20,  23,  26,  29,  32,  35,  38,  41,  44,  47,
	  50,  53,  56,  59,  62,  65,  68,  71,  74,  77,  80,  83,  86,  89,  91,  94,
	  97, 100, 103, 105, 108, 111, 114, 116, 119, 122, 125, 127, 130, 132, 135, 138,
	 140, 143, 145, 148, 150, 153, 155, 157, 160, 162, 164, 167, 169, 171, 173, 176,
	 178, 180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 205, 207,
	 209, 210, 212, 214, 215, 217, 218, 220, 221, 223, 224, 225, 227, 228, 229, 230,
	 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 241, 242, 243, 244, 244, 245,
	 245, 246, 247, 247, 248, 248, 248, 249, 249, 249, 249, 250, 250, 250, 250, 250,
	 250, 250, 250, 250, 250, 249, 249, 249, 249, 248, 248, 248, 247, 247, 246, 245,
	 245, 244, 244, 243, 242, 241, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232,
	 230, 229, 228, 227, 225, 224, 223, 221, 220, 218, 217, 215, 214, 212, 210, 209,
	 207, 205, 204, 202, 200, 198, 196, 194, 192, 190, 188, 186, 184, 182, 180, 178,
	 176, 173, 171, 169, 167, 164, 162, 160, 157, 155, 153, 150, 148, 145, 143, 140,
	 138, 135, 132, 130, 127, 125, 122, 119, 116, 114, 111, 108, 105, 103, 100,  97,
	  94,  91,  89,  86,  83,  80,  77,  74,  71,  68,  65,  62,  59,  56,  53,  50,
	  47,  44,  41,  38,  35,  32,  29,  26,  23,  20,  17,  14,  11,   8,   5,   2,
	  -2,  -5,  -8, -11, -14, -17, -20, -23, -26, -29, -32, -35, -38, -41, -44, -47,
	 -50, -53, -56, -59, -62, -65, -68, -71, -74, -77, -80, -83, -86, -89, -91, -94,
	 -97,-100,-103,-105,-108,-111,-114,-116,-119,-122,-125,-127,-130,-132,-135,-138,
	-140,-143,-145,-148,-150,-153,-155,-157,-160,-162,-164,-167,-169,-171,-173,-176,
	-178,-180,-182,-184,-186,-188,-190,-192,-194,-196,-198,-200,-202,-204,-205,-207,
	-209,-210,-212,-214,-215,-217,-218,-220,-221,-223,-224,-225,-227,-228,-229,-230,
	-232,-233,-234,-235,-236,-237,-238,-239,-240,-241,-241,-242,-243,-244,-244,-245,
	-245,-246,-247,-247,-248,-248,-248,-249,-249,-249,-249,-250,-250,-250,-250,-250,
	-250,-250,-250,-250,-250,-249,-249,-249,-249,-248,-248,-248,-247,-247,-246,-245,
	-245,-244,-244,-243,-242,-241,-241,-240,-239,-238,-237,-236,-235,-234,-233,-232,
	-230,-229,-228,-227,-225,-224,-223,-221,-220,-218,-217,-215,-214,-212,-210,-209,
	-207,-205,-204,-202,-200,-198,-196,-194,-192,-190,-188,-186,-184,-182,-180,-178,
	-176,-173,-171,-169,-167,-164,-162,-160,-157,-155,-153,-150,-148,-145,-143,-140,
	-138,-135,-132,-130,-127,-125,-122,-119,-116,-114,-111,-108,-105,-103,-100, -97,
	 -94, -91, -89, -86, -83, -80, -77, -74, -71, -68, -65, -62, -59, -56, -53, -50,
	 -47, -44, -41, -38, -35, -32, -29, -26, -23, -20, -17, -14, -11,  -8,  -5,  -2
};

// Cosine table for 512 angles between 0 and 360
static int cosTable512[512] = {
	 250, 250, 250, 250, 250, 249, 249, 249, 249, 248, 248, 248, 247, 247, 246, 245,
	 245, 244, 244, 243, 242, 241, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232,
	 230, 229, 228, 227, 225, 224, 223, 221, 220, 218, 217, 215, 214, 212, 210, 209,
	 207, 205, 204, 202, 200, 198, 196, 194, 192, 190, 188, 186, 184, 182, 180, 178,
	 176, 173, 171, 169, 167, 164, 162, 160, 157, 155, 153, 150, 148, 145, 143, 140,
	 138, 135, 132, 130, 127, 125, 122, 119, 116, 114, 111, 108, 105, 103, 100,  97,
	  94,  91,  89,  86,  83,  80,  77,  74,  71,  68,  65,  62,  59,  56,  53,  50,
	  47,  44,  41,  38,  35,  32,  29,  26,  23,  20,  17,  14,  11,   8,   5,   2,
	  -2,  -5,  -8, -11, -14, -17, -20, -23, -26, -29, -32, -35, -38, -41, -44, -47,
	 -50, -53, -56, -59, -62, -65, -68, -71, -74, -77, -80, -83, -86, -89, -91, -94,
	 -97,-100,-103,-105,-108,-111,-114,-116,-119,-122,-125,-127,-130,-132,-135,-138,
	-140,-143,-145,-148,-150,-153,-155,-157,-160,-162,-164,-167,-169,-171,-173,-176,
	-178,-180,-182,-184,-186,-188,-190,-192,-194,-196,-198,-200,-202,-204,-205,-207,
	-209,-210,-212,-214,-215,-217,-218,-220,-221,-223,-224,-225,-227,-228,-229,-230,
	-232,-233,-234,-235,-236,-237,-238,-239,-240,-241,-241,-242,-243,-244,-244,-245,
	-245,-246,-247,-247,-248,-248,-248,-249,-249,-249,-249,-250,-250,-250,-250,-250,
	-250,-250,-250,-250,-250,-249,-249,-249,-249,-248,-248,-248,-247,-247,-246,-245,
	-245,-244,-244,-243,-242,-241,-241,-240,-239,-238,-237,-236,-235,-234,-233,-232,
	-230,-229,-228,-227,-225,-224,-223,-221,-220,-218,-217,-215,-214,-212,-210,-209,
	-207,-205,-204,-202,-200,-198,-196,-194,-192,-190,-188,-186,-184,-182,-180,-178,
	-176,-173,-171,-169,-167,-164,-162,-160,-157,-155,-153,-150,-148,-145,-143,-140,
	-138,-135,-132,-130,-127,-125,-122,-119,-116,-114,-111,-108,-105,-103,-100, -97,
	 -94, -91, -89, -86, -83, -80, -77, -74, -71, -68, -65, -62, -59, -56, -53, -50,
	 -47, -44, -41, -38, -35, -32, -29, -26, -23, -20, -17, -14, -11,  -8,  -5,  -2,
	   2,   5,   8,  11,  14,  17,  20,  23,  26,  29,  32,  35,  38,  41,  44,  47,
	  50,  53,  56,  59,  62,  65,  68,  71,  74,  77,  80,  83,  86,  89,  91,  94,
	  97, 100, 103, 105, 108, 111, 114, 116, 119, 122, 125, 127, 130, 132, 135, 138,
	 140, 143, 145, 148, 150, 153, 155, 157, 160, 162, 164, 167, 169, 171, 173, 176,
	 178, 180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 205, 207,
	 209, 210, 212, 214, 215, 217, 218, 220, 221, 223, 224, 225, 227, 228, 229, 230,
	 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 241, 242, 243, 244, 244, 245,
	 245, 246, 247, 247, 248, 248, 248, 249, 249, 249, 249, 250, 250, 250, 250, 250
};

/* ------------- Static (Compilation-Time) Configuration ------------- */

// Receiver antenna attenuation in dB for boresight angle = 0:5:180 [deg]
double ant_pat_db[37] = {
	 0.00,  0.00,  0.22,  0.44,  0.67,  1.11,  1.56,  2.00,  2.44,  2.89,  3.56,  4.22,
	 4.89,  5.56,  6.22,  6.89,  7.56,  8.22,  8.89,  9.78, 10.67, 11.56, 12.44, 13.33,
	14.44, 15.56, 16.67, 17.78, 18.89, 20.00, 21.33, 22.67, 24.00, 25.56, 27.33, 29.33,
	31.56
};
#define ANT_PAT_EL_COUNT 37
#define ANT_PAT_AZ_COUNT 37

/* ------------- Helper Math Functions ------------- */

/*! 
 * \brief Subtract two 3-dimentional vectors of type double
 * \param[out] vout Stores the result of the subtraction
 * \param[in] x1 Minuend of subtraction
 * \param[in] x2 Subtrahend of subtraction
 */
void subVect(double vout[3], const double x1[3], const double x2[3])
{
	vout[0] = x1[0] - x2[0];
	vout[1] = x1[1] - x2[1];
	vout[2] = x1[2] - x2[2];

	return;
}

/*! 
 * \brief Compute the Norm of a 3-dimentional vector of type double
 * \param[in] x Input vector
 * \returns Length (Norm) of the input vector
 */
double normVect(const double x[3])
{
	return(sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]));
}

/*! 
 * \brief Compute dot-product of two 3-dimentional vectors of type double
 * \param[in] x1 First multiplicand
 * \param[in] x2 Second multiplicand
 * \returns Dot-product of both multiplicands
 */
double dotProd(const double x1[3], const double x2[3])
{
	return(x1[0] * x2[0] + x1[1] * x2[1] + x1[2] * x2[2]);
}

/*! 
 * \brief Compute the norm of a unit quaternion
 * \param[in] q Unit quaternion [qw, qx, qy, qz]
 * \returns Norm of the quaternion
 */
double quatNormVect(const double q[4])
{
	return(sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]));
}

/*! 
 * \brief Rotate a 3-dimentional vector of type double using a unit quaternion
 * \param[in]  q    Unit quaternion [qw, qx, qy, qz] (ECEF -> body)
 * \param[in]  v    Input vector in ECEF frame
 * \param[out] vout Rotated vector in body frame
 */
void quatRotVect(double vout[3], const double q[4], const double v[3])
{
    double qw = q[0], qx = q[1], qy = q[2], qz = q[3];

    // Equivalent rotation matrix R = f(q), applied as vout = R * v
    vout[0] = (1.0 - 2.0 * (qy * qy + qz * qz)) * v[0]
            + 2.0 * (qx * qy - qw * qz) * v[1]
            + 2.0 * (qx * qz + qw * qy) * v[2];

    vout[1] = 2.0 * (qx * qy + qw * qz) * v[0]
            + (1.0 - 2.0 * (qx * qx + qz * qz)) * v[1]
            + 2.0 * (qy * qz - qw * qx) * v[2];

    vout[2] = 2.0 * (qx * qz - qw * qy) * v[0]
            + 2.0 * (qy * qz + qw * qx) * v[1]
            + (1.0 - 2.0 * (qx * qx + qy * qy)) * v[2];
}

/* ------------- Helper Functions ------------- */

/*! 
 * \brief Generate the C/A code sequence for a given Satellite Vehicle PRN
 * \param[in] prn PRN number of the Satellite Vehicle
 * \param[out] ca Caller-allocated integer array of 1023 bytes
 * \details The C/A code sequence is generated using two 10-bit shift registers (G1 and G2) with specific feedback taps.
 * \details The output of the G1 register is combined with a delayed version of the G2 register to produce the final C/A code sequence for the specified PRN.
 * \details The delay for the G2 register is determined by the PRN number according to a predefined table.
 * 	\details For more information on the C/A code generation, refer to the GPS Interface Control Document (ICD-GPS-200) Section 3.3.2.3
 */
void codegen(int *ca, int prn)
{
	// PRN numbers are between 1 and 32, invalid PRN numbers are ignored
	if (prn < 1 || prn > 32)
		return;
	
	int delay[] = {
		  5,   6,   7,   8,  17,  18, 139, 140, 141, 251,
		252, 254, 255, 256, 257, 258, 469, 470, 471, 472,
		473, 474, 509, 512, 513, 514, 515, 516, 859, 860,
		861, 862
	};
		
	int r1[N_DWRD_SBF], r2[N_DWRD_SBF];

	// Initialize the shift registers with all bits set to -1
	for (int i = 0; i < N_DWRD_SBF; i++)
		r1[i] = r2[i] = -1;

	int g1[CA_SEQ_LEN], g2[CA_SEQ_LEN];

	for (int i = 0; i < CA_SEQ_LEN; i++)
	{
		g1[i] = r1[9];
		g2[i] = r2[9];

		int c1 = r1[2] * r1[9];
		int c2 = r2[1] * r2[2] * r2[5] * r2[7] * r2[8] * r2[9];

		for (int j = 9; j > 0; j--)
		{
			r1[j] = r1[j - 1];
			r2[j] = r2[j - 1];
		}
		r1[0] = c1;
		r2[0] = c2;
	}

	for (int i = 0, j = CA_SEQ_LEN - delay[prn - 1]; i < CA_SEQ_LEN; i++, j++)
		ca[i] = (1 - g1[i] * g2[j % CA_SEQ_LEN]) / 2;

	return;
}

// Date/Time Conversion Functions

/*! 
 * \brief Convert a UTC date into a GPS date
 * \param[in] t input date in UTC form
 * \param[out] g output date in GPS form
 * \details GPS time is defined as the number of weeks and seconds since the GPS epoch, which is January 6, 1980 at 00:00:00 UTC.
 * \details This function computes the GPS week number and the seconds into that week for a given UTC date.
 */
void date2gps(const datetime_t *t, gpstime_t *g)
{
	int doy[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

	// Number of years since 1980
	int year = t->y - 1980;

	// Compute the number of leap days since Jan 5/Jan 6, 1980.
	int lpdays = year / 4 + 1;
	if ((year % 4) == 0 && t->m <= 2)
		lpdays--;

	// Compute the number of days elapsed since Jan 5/Jan 6, 1980.
	int days_elapsed = year * 365 + doy[t->m - 1] + t->d + lpdays - 6;

	// Convert time to GPS weeks and seconds.
	g->week = days_elapsed / 7;
	g->sec = (double)(days_elapsed % 7) * SECONDS_IN_DAY + t->hh * SECONDS_IN_HOUR
		+ t->mm * SECONDS_IN_MINUTE + t->sec;

	return;
}

/*! 
 * \brief Convert a GPS date into a UTC date
 * \param[in] g input date in GPS form
 * \param[out] t output date in UTC form
 * \details GPS time is defined as the number of weeks and seconds since the GPS epoch, which is January 6, 1980 at 00:00:00 UTC.
 * \details This function computes the calendar date (year, month, day) and time (hour, minute, second) for a given GPS week number and seconds into that week.
 */
void gps2date(const gpstime_t *g, datetime_t *t)
{
	// Convert Julian day number to calendar date
	int c = (int)(7 * g->week + floor(g->sec / 86400.0) + 2444245.0) + 1537;
	int d = (int)((c - 122.1) / 365.25);
	int e = 365 * d + d / 4;
	int f = (int)((c - e) / 30.6001);

	t->d = c - e - (int)(30.6001 * f);
	t->m = f - 1 - 12*(f / 14);
	t->y = d - 4715 - ((7 + t->m) / 10);

	t->hh = ((int)(g->sec / 3600.0)) % 24;
	t->mm = ((int)(g->sec / 60.0)) % 60;
	t->sec = g->sec - 60.0 * floor(g->sec / 60.0);

	return;
}

// Coordinate Conversion Functions

/*! 
 * \brief Convert Earth-centered Earth-fixed (ECEF) into Lat/Long/Height
 * \param[in] xyz Input Array of X, Y and Z ECEF coordinates
 * \param[out] llh Output Array of Latitude, Longitude and Height
 */
void ecef2llh(const double *xyz, double *llh)
{
	double eps = 1.0e-3;

	// Check for zero-length ECEF vector to avoid division by zero in subsequent calculations
	if (normVect(xyz) < eps)
	{
		// Invalid ECEF vector
		llh[0] = 0.0;
		llh[1] = 0.0;
		llh[2] = -WGS84_RADIUS;

		return;
	}

	double x = xyz[0];
	double y = xyz[1];
	double z = xyz[2];

	double rho_sq = x * x + y * y;
	double dz = WGS84_ECCENTRICITY_SQ * z;


	double z_effective, dist_to_surface_normal, prime_vertical_radius;
	while (1)
	{
		z_effective = z + dz;
		dist_to_surface_normal = sqrt(rho_sq + z_effective * z_effective);

		double sin_lat = z_effective / dist_to_surface_normal;
		prime_vertical_radius = WGS84_RADIUS / sqrt(1.0 - WGS84_ECCENTRICITY_SQ * sin_lat * sin_lat);
		double dz_new = prime_vertical_radius * WGS84_ECCENTRICITY_SQ * sin_lat;

		if (fabs(dz - dz_new) < eps)
			break;

		dz = dz_new;
	}

	double rho = sqrt(rho_sq);
	llh[0] = atan2(z_effective, rho);
	llh[1] = atan2(y, x);
	llh[2] = dist_to_surface_normal - prime_vertical_radius;

	return;
}

/*! 
 * \brief Convert Lat/Long/Height into Earth-centered Earth-fixed (ECEF)
 * \param[in] llh Input Array of Latitude, Longitude and Height
 * \param[out] xyz Output Array of X, Y and Z ECEF coordinates
 */
void llh2ecef(const double *llh, double *xyz)
{
	double cos_lat = cos(llh[0]);
	double sin_lat = sin(llh[0]);
	double cos_lon = cos(llh[1]);
	double sin_lon = sin(llh[1]);
	double d = WGS84_ECCENTRICITY * sin_lat;

	double n = WGS84_RADIUS / sqrt(1.0 - d * d);
	double nph = n + llh[2];

	double tmp = nph * cos_lat;
	xyz[0] = tmp * cos_lon;
	xyz[1] = tmp * sin_lon;
	xyz[2] = ((1.0 - WGS84_ECCENTRICITY_SQ) * n + llh[2]) * sin_lat;

	return;
}

/*! 
 * \brief Compute the intermediate matrix for LLH to ECEF
 * \param[in] llh Input position in Latitude-Longitude-Height format
 * \param[out] t Three-by-Three output matrix
 */
void ltcmat(const double *llh, double t[3][3])
{
	double sin_lat = sin(llh[0]);
	double cos_lat = cos(llh[0]);
	double sin_lon = sin(llh[1]);
	double cos_lon = cos(llh[1]);

	t[0][0] = -sin_lat * cos_lon;
	t[0][1] = -sin_lat * sin_lon;
	t[0][2] = cos_lat;
	t[1][0] = -sin_lon;
	t[1][1] = cos_lon;
	t[1][2] = 0.0;
	t[2][0] = cos_lat * cos_lon;
	t[2][1] = cos_lat * sin_lon;
	t[2][2] = sin_lat;

	return;
}

/*! 
 * \brief Convert Earth-centered Earth-Fixed to North-East-Up
 * \param[in] xyz Input position as vector in ECEF format
 * \param[in] t Intermediate matrix computed by \ref ltcmat
 * \param[out] neu Output position as North-East-Up format
 */
void ecef2neu(const double *xyz, double t[3][3], double *neu)
{
	neu[0] = t[0][0] * xyz[0] + t[0][1] * xyz[1] + t[0][2] * xyz[2];
	neu[1] = t[1][0] * xyz[0] + t[1][1] * xyz[1] + t[1][2] * xyz[2];
	neu[2] = t[2][0] * xyz[0] + t[2][1] * xyz[1] + t[2][2] * xyz[2];

	return;
}

/*!
 * \brief Convert North-East-Up to Azimuth + Elevation
 * \param[in] neu Input position in North-East-Up format
 * \param[out] azel Output array of azimuth + elevation as double
 */
void neu2azel(double *azel, const double *neu)
{
	azel[0] = atan2(neu[1], neu[0]);
	if (azel[0] < 0.0)
		azel[0] += (2.0 * PI);

	double ne = sqrt(neu[0] * neu[0] + neu[1] * neu[1]);
	azel[1] = atan2(neu[2], ne);

	return;
}

// Satellite Position, Velocity and Clock Computation Functions

/*! 
 * \brief Compute Satellite position, velocity and clock at given time
 * \param[in] eph Ephemeris data of the satellite
 * \param[in] g GPS time at which position is to be computed
 * \param[out] pos Computed position (vector)
 * \param[out] vel Computed velocity (vector)
 * \param[clk] clk Computed clock
 */
void satpos(const ephem_t *eph, gpstime_t g, double *pos, double *vel, double *clk)
{
	// Computing Satellite Velocity using the Broadcast Ephemeris
	// http://www.ngs.noaa.gov/gps-toolbox/bc_velo.htm

	double tk = g.sec - eph->toe.sec;

	if (tk > SECONDS_IN_HALF_WEEK)
		tk -= SECONDS_IN_WEEK;
	else if (tk < -SECONDS_IN_HALF_WEEK)
		tk += SECONDS_IN_WEEK;

	double mk = eph->m0 + eph->n * tk;
	double ek = mk;
	double ekold = ek + 1.0;

	double OneMinusecosE = 0; // Suppress the uninitialized warning.
	while(fabs(ek-ekold) > 1.0E-14)
	{
		ekold = ek;
		OneMinusecosE = 1.0 - eph->ecc * cos(ekold);
		ek = ek + (mk - ekold + eph->ecc * sin(ekold)) / OneMinusecosE;
	}

	double sin_ek = sin(ek);
	double cos_ek = cos(ek);

	double ek_dot = eph->n / OneMinusecosE;

	double relativistic = -4.442807633E-10 * eph->ecc * eph->sqrta * sin_ek;

	double pk = atan2(eph->sq1e2 * sin_ek, cos_ek - eph->ecc) + eph->aop;
	double pk_dot = eph->sq1e2 * ek_dot / OneMinusecosE;

	double sin_sq_pk = sin(2.0 * pk);
	double cos_sq_pk = cos(2.0 * pk);

	double uk = pk + eph->cus * sin_sq_pk + eph->cuc * cos_sq_pk;
	double sin_uk = sin(uk);
	double cos_uk = cos(uk);
	double uk_dot = pk_dot * (1.0 + 2.0 * (eph->cus * cos_sq_pk - eph->cuc * sin_sq_pk));

	double rk = eph->A * OneMinusecosE + eph->crc * cos_sq_pk + eph->crs * sin_sq_pk;
	double rk_dot = eph->A * eph->ecc * sin_ek * ek_dot + 2.0 * pk_dot * (eph->crs * cos_sq_pk - eph->crc * sin_sq_pk);

	double ik = eph->inc0 + eph->idot*tk + eph->cic * cos_sq_pk + eph->cis * sin_sq_pk;
	double sin_ik = sin(ik);
	double cos_ik = cos(ik);
	double ik_dot = eph->idot + 2.0*pk_dot*(eph->cis * cos_sq_pk - eph->cic * sin_sq_pk);

	double xpk = rk * cos_uk;
	double ypk = rk * sin_uk;
	double xpk_dot = rk_dot * cos_uk - ypk * uk_dot;
	double ypk_dot = rk_dot * sin_uk + xpk * uk_dot;

	double ok = eph->omg0 + tk * eph->omgkdot - OMEGA_EARTH * eph->toe.sec;
	double sin_ok = sin(ok);
	double cos_ok = cos(ok);

	pos[0] = xpk * cos_ok - ypk * cos_ik * sin_ok;
	pos[1] = xpk * sin_ok + ypk * cos_ik * cos_ok;
	pos[2] = ypk * sin_ik;

	double tmp = ypk_dot * cos_ik - ypk * sin_ik * ik_dot;

	vel[0] = -eph->omgkdot * pos[1] + xpk_dot * cos_ok - tmp * sin_ok;
	vel[1] = eph->omgkdot * pos[0] + xpk_dot * sin_ok + tmp * cos_ok;
	vel[2] = ypk * cos_ik * ik_dot + ypk_dot * sin_ik;

	// Satellite clock correction
	tk = g.sec - eph->toc.sec;

	if (tk > SECONDS_IN_HALF_WEEK)
		tk -= SECONDS_IN_WEEK;
	else if (tk < -SECONDS_IN_HALF_WEEK)
		tk += SECONDS_IN_WEEK;

	clk[0] = eph->af0 + tk * (eph->af1 + tk * eph->af2) + relativistic - eph->tgd;
	clk[1] = eph->af1 + 2.0 * tk * eph->af2;

	return;
}

/*! 
 * \brief Compute Subframe from Ephemeris
 * \param[in] eph Ephemeris of given SV
 * \param[out] sbf Array of five sub-frames, 10 long words each
 */
void eph2sbf(const ephem_t eph, const ionoutc_t ionoutc, unsigned long sbf[5][N_DWRD_SBF])
{
	unsigned long wn;
	unsigned long toe;
	unsigned long toc;
	unsigned long iode;
	unsigned long iodc;
	long deltan;
	long cuc;
	long cus;
	long cic;
	long cis;
	long crc;
	long crs;
	unsigned long ecc;
	unsigned long sqrta;
	long m0;
	long omg0;
	long inc0;
	long aop;
	long omgdot;
	long idot;
	long af0;
	long af1;
	long af2;
	long tgd;
	int svhlth;
	int codeL2;

	unsigned long ura = 0UL;
	unsigned long dataId = 1UL;
	unsigned long sbf4_page25_svId = 63UL;
	unsigned long sbf5_page25_svId = 51UL;

	unsigned long wna;
	unsigned long toa;

	signed long alpha0,alpha1,alpha2,alpha3;
	signed long beta0,beta1,beta2,beta3;
	signed long A0,A1;
	signed long dtls;
	unsigned long tot,wnt,wnlsf,dtlsf,dn;
	unsigned long sbf4_page18_svId = 56UL;

	// FIXED: This has to be the "transmission" week number, not for the ephemeris reference time
	//wn = (unsigned long)(eph.toe.week%1024);
	wn = 0UL;
	toe = (unsigned long)(eph.toe.sec/16.0);
	toc = (unsigned long)(eph.toc.sec/16.0);
	iode = (unsigned long)(eph.iode);
	iodc = (unsigned long)(eph.iodc);
	deltan = (long)(eph.deltan/POW2_M43/PI);
	cuc = (long)(eph.cuc/POW2_M29);
	cus = (long)(eph.cus/POW2_M29);
	cic = (long)(eph.cic/POW2_M29);
	cis = (long)(eph.cis/POW2_M29);
	crc = (long)(eph.crc/POW2_M5);
	crs = (long)(eph.crs/POW2_M5);
	ecc = (unsigned long)(eph.ecc/POW2_M33);
	sqrta = (unsigned long)(eph.sqrta/POW2_M19);
	m0 = (long)(eph.m0/POW2_M31/PI);
	omg0 = (long)(eph.omg0/POW2_M31/PI);
	inc0 = (long)(eph.inc0/POW2_M31/PI);
	aop = (long)(eph.aop/POW2_M31/PI);
	omgdot = (long)(eph.omgdot/POW2_M43/PI);
	idot = (long)(eph.idot/POW2_M43/PI);
	af0 = (long)(eph.af0/POW2_M31);
	af1 = (long)(eph.af1/POW2_M43);
	af2 = (long)(eph.af2/POW2_M55);
	tgd = (long)(eph.tgd/POW2_M31);
	svhlth = (unsigned long)(eph.svhlth);
	codeL2 = (unsigned long)(eph.codeL2);

	wna = (unsigned long)(eph.toe.week%256);
	toa = (unsigned long)(eph.toe.sec/4096.0);

	alpha0 = (signed long)round(ionoutc.alpha0/POW2_M30);
	alpha1 = (signed long)round(ionoutc.alpha1/POW2_M27);
	alpha2 = (signed long)round(ionoutc.alpha2/POW2_M24);
	alpha3 = (signed long)round(ionoutc.alpha3/POW2_M24);
	beta0 = (signed long)round(ionoutc.beta0/2048.0);
	beta1 = (signed long)round(ionoutc.beta1/16384.0);
	beta2 = (signed long)round(ionoutc.beta2/65536.0);
	beta3 = (signed long)round(ionoutc.beta3/65536.0);
	A0 = (signed long)round(ionoutc.A0/POW2_M30);
	A1 = (signed long)round(ionoutc.A1/POW2_M50);
	dtls = (signed long)(ionoutc.dtls);
	tot = (unsigned long)(ionoutc.tot/4096);
	wnt = (unsigned long)(ionoutc.wnt%256);

	// 2016/12/31 (Sat) -> WNlsf = 1929, DN = 7 (http://navigationservices.agi.com/GNSSWeb/)
	// Days are counted from 1 to 7 (Sunday is 1).
	if (ionoutc.leapen==TRUE)
	{
		wnlsf = (unsigned long)(ionoutc.wnlsf%256);
		dn = (unsigned long)(ionoutc.dn);
		dtlsf = (unsigned long)(ionoutc.dtlsf);
	}
	else
	{
		wnlsf = 1929%256;
		dn = 7;
		dtlsf = 18;
	}
	// Subframe 1
	sbf[0][0] = 0x8B0000UL<<6;
	sbf[0][1] = 0x1UL<<8;
	sbf[0][2] = ((wn&0x3FFUL)<<20) | ((codeL2&0x3UL)<<18) | ((ura&0xFUL)<<14) | ((svhlth&0x3FUL)<<8) | (((iodc>>8)&0x3UL)<<6);
	sbf[0][3] = 0UL;
	sbf[0][4] = 0UL;
	sbf[0][5] = 0UL;
	sbf[0][6] = (tgd&0xFFUL)<<6;
	sbf[0][7] = ((iodc&0xFFUL)<<22) | ((toc&0xFFFFUL)<<6);
	sbf[0][8] = ((af2&0xFFUL)<<22) | ((af1&0xFFFFUL)<<6);
	sbf[0][9] = (af0&0x3FFFFFUL)<<8;

	// Subframe 2
	sbf[1][0] = 0x8B0000UL<<6;
	sbf[1][1] = 0x2UL<<8;
	sbf[1][2] = ((iode&0xFFUL)<<22) | ((crs&0xFFFFUL)<<6);
	sbf[1][3] = ((deltan&0xFFFFUL)<<14) | (((m0>>24)&0xFFUL)<<6);
	sbf[1][4] = (m0&0xFFFFFFUL)<<6;
	sbf[1][5] = ((cuc&0xFFFFUL)<<14) | (((ecc>>24)&0xFFUL)<<6);
	sbf[1][6] = (ecc&0xFFFFFFUL)<<6;
	sbf[1][7] = ((cus&0xFFFFUL)<<14) | (((sqrta>>24)&0xFFUL)<<6);
	sbf[1][8] = (sqrta&0xFFFFFFUL)<<6;
	sbf[1][9] = (toe&0xFFFFUL)<<14;

	// Subframe 3
	sbf[2][0] = 0x8B0000UL<<6;
	sbf[2][1] = 0x3UL<<8;
	sbf[2][2] = ((cic&0xFFFFUL)<<14) | (((omg0>>24)&0xFFUL)<<6);
	sbf[2][3] = (omg0&0xFFFFFFUL)<<6;
	sbf[2][4] = ((cis&0xFFFFUL)<<14) | (((inc0>>24)&0xFFUL)<<6);
	sbf[2][5] = (inc0&0xFFFFFFUL)<<6;
	sbf[2][6] = ((crc&0xFFFFUL)<<14) | (((aop>>24)&0xFFUL)<<6);
	sbf[2][7] = (aop&0xFFFFFFUL)<<6;
	sbf[2][8] = (omgdot&0xFFFFFFUL)<<6;
	sbf[2][9] = ((iode&0xFFUL)<<22) | ((idot&0x3FFFUL)<<8);

	if (ionoutc.vflg==TRUE)
	{
		// Subframe 4, page 18
		sbf[3][0] = 0x8B0000UL<<6;
		sbf[3][1] = 0x4UL<<8;
		sbf[3][2] = (dataId<<28) | (sbf4_page18_svId<<22) | ((alpha0&0xFFUL)<<14) | ((alpha1&0xFFUL)<<6);
		sbf[3][3] = ((alpha2&0xFFUL)<<22) | ((alpha3&0xFFUL)<<14) | ((beta0&0xFFUL)<<6);
		sbf[3][4] = ((beta1&0xFFUL)<<22) | ((beta2&0xFFUL)<<14) | ((beta3&0xFFUL)<<6);
		sbf[3][5] = (A1&0xFFFFFFUL)<<6;
		sbf[3][6] = ((A0>>8)&0xFFFFFFUL)<<6;
		sbf[3][7] = ((A0&0xFFUL)<<22) | ((tot&0xFFUL)<<14) | ((wnt&0xFFUL)<<6);
		sbf[3][8] = ((dtls&0xFFUL)<<22) | ((wnlsf&0xFFUL)<<14) | ((dn&0xFFUL)<<6);
		sbf[3][9] = (dtlsf&0xFFUL)<<22;

	}
	else
	{
		// Subframe 4, page 25
		sbf[3][0] = 0x8B0000UL<<6;
		sbf[3][1] = 0x4UL<<8;
		sbf[3][2] = (dataId<<28) | (sbf4_page25_svId<<22);
		sbf[3][3] = 0UL;
		sbf[3][4] = 0UL;
		sbf[3][5] = 0UL;
		sbf[3][6] = 0UL;
		sbf[3][7] = 0UL;
		sbf[3][8] = 0UL;
		sbf[3][9] = 0UL;
	}

	// Subframe 5, page 25
	sbf[4][0] = 0x8B0000UL<<6;
	sbf[4][1] = 0x5UL<<8;
	sbf[4][2] = (dataId<<28) | (sbf5_page25_svId<<22) | ((toa&0xFFUL)<<14) | ((wna&0xFFUL)<<6);
	sbf[4][3] = 0UL;
	sbf[4][4] = 0UL;
	sbf[4][5] = 0UL;
	sbf[4][6] = 0UL;
	sbf[4][7] = 0UL;
	sbf[4][8] = 0UL;
	sbf[4][9] = 0UL;

	return;
}

/*! 
 * \brief Count number of bits set to 1
 * \param[in] v long word in which bits are counted
 * \returns Count of bits set to 1
 */
unsigned long countBits(unsigned long v)
{
	unsigned long c;
	const int S[] = {1, 2, 4, 8, 16};
	const unsigned long B[] = {
		0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF
	};

	c = v;
	c = ((c >> S[0]) & B[0]) + (c & B[0]);
	c = ((c >> S[1]) & B[1]) + (c & B[1]);
	c = ((c >> S[2]) & B[2]) + (c & B[2]);
	c = ((c >> S[3]) & B[3]) + (c & B[3]);
	c = ((c >> S[4]) & B[4]) + (c & B[4]);

	return (c);
}

/*! 
 * \brief Compute the Checksum for one given word of a subframe
 * \param[in] source The input data
 * \param[in] nib Does this word contain non-information-bearing bits?
 * \returns Computed Checksum
 */
unsigned long computeChecksum(unsigned long source, int nib)
{
	/*
	Bits 31 to 30 = 2 LSBs of the previous transmitted word, D29* and D30*
	Bits 29 to  6 = Source data bits, d1, d2, ..., d24
	Bits  5 to  0 = Empty parity bits
	*/

	/*
	Bits 31 to 30 = 2 LSBs of the previous transmitted word, D29* and D30*
	Bits 29 to  6 = Data bits transmitted by the SV, D1, D2, ..., D24
	Bits  5 to  0 = Computed parity bits, D25, D26, ..., D30
	*/

	/*
	                  1            2           3
	bit    12 3456 7890 1234 5678 9012 3456 7890
	---    -------------------------------------
	D25    11 1011 0001 1111 0011 0100 1000 0000
	D26    01 1101 1000 1111 1001 1010 0100 0000
	D27    10 1110 1100 0111 1100 1101 0000 0000
	D28    01 0111 0110 0011 1110 0110 1000 0000
	D29    10 1011 1011 0001 1111 0011 0100 0000
	D30    00 1011 0111 1010 1000 1001 1100 0000
	*/

	// The bmask array contains the bits that are used to compute the parity bits D25 to D30.
	unsigned long bmask[6] = {
		0x3B1F3480UL, 0x1D8F9A40UL, 0x2EC7CD00UL,
		0x1763E680UL, 0x2BB1F340UL, 0x0B7A89C0UL
	};

	unsigned long d = source & 0x3FFFFFC0UL;
	unsigned long D29 = (source >> 31) & 0x1UL;
	unsigned long D30 = (source >> 30) & 0x1UL;

	if (nib) // Non-information bearing bits for word 2 and 10
	{
		/*
		Solve bits 23 and 24 to preserve parity check
		with zeros in bits 29 and 30.
		*/

		if ((D30 + countBits(bmask[4] & d)) % 2)
			d ^= (0x1UL<<6);
		if ((D29 + countBits(bmask[5] & d)) % 2)
			d ^= (0x1UL<<7);
	}

	unsigned long D = d;
	if (D30)
		D ^= 0x3FFFFFC0UL;

	D |= ((D29 + countBits(bmask[0] & d)) % 2) << 5;
	D |= ((D30 + countBits(bmask[1] & d)) % 2) << 4;
	D |= ((D29 + countBits(bmask[2] & d)) % 2) << 3;
	D |= ((D30 + countBits(bmask[3] & d)) % 2) << 2;
	D |= ((D30 + countBits(bmask[4] & d)) % 2) << 1;
	D |= ((D29 + countBits(bmask[5] & d)) % 2);

	D &= 0x3FFFFFFFUL;
	//D |= (source & 0xC0000000UL); // Add D29* and D30* from source data bits

	return (D);
}

/*! 
 * \brief Replace all 'D' exponential designators to 'E'
 * \param str String in which all occurrences of 'D' are replaced with 'E'
 * \param len Length of input string in bytes
 * \returns Number of characters replaced
 */
int replaceExpDesignator(char *str, int len)
{
	int n = 0;
	for (int i = 0; i < len; i++)
	{
		if (str[i] == 'D')
		{
			n++;
			str[i] = 'E';
		}
	}

	return (n);
}

/*! 
 * \brief Subtract two GPS times to compute the difference in seconds
 * \param[in] g1 First GPS time
 * \param[in] g0 Second GPS time
 * \returns Difference in seconds between the two GPS times
*/
double subGpsTime(gpstime_t g1, gpstime_t g0)
{
	double dt;

	dt = g1.sec - g0.sec;
	dt += (double)(g1.week - g0.week) * SECONDS_IN_WEEK;

	return (dt);
}

/*!
 * \brief Increment a GPS time by a given duration
 * \param[in] g0 Initial GPS time
 * \param[in] dt Duration to add in seconds
 * \returns Incremented GPS time
 */
gpstime_t incGpsTime(gpstime_t g0, double dt)
{
	gpstime_t g1;

	g1.week = g0.week;
	g1.sec = g0.sec + dt;

	g1.sec = round(g1.sec * 1000.0) / 1000.0; // Avoid rounding error

	while (g1.sec >= SECONDS_IN_WEEK)
	{
		g1.sec -= SECONDS_IN_WEEK;
		g1.week++;
	}

	while (g1.sec < 0.0)
	{
		g1.sec += SECONDS_IN_WEEK;
		g1.week--;
	}

	return (g1);
}

/*! 
 * \brief Read Ephemeris data from the RINEX Navigation file 
 * \param[out] eph Array of Output SV ephemeris data
 * \param[in] fname File name of the RINEX file
 * \returns Number of sets of ephemerides in the file
 */
int readRinexNavAll(ephem_t eph[][MAX_SAT], ionoutc_t *ionoutc, const char *fname)
{
	int ieph;

	int sv;
	char str[MAX_CHAR];
	char tmp[20];

	datetime_t t;
	gpstime_t g;
	gpstime_t g0;
	double dt;

	int flags = 0x0;

	FILE *fp;
	if ((fp = fopen(fname, "rt")) == NULL)
		return(-1);

	// Clear valid flag
	for (int i = 0; i < EPHEM_ARRAY_SIZE; i++)
		for (sv = 0; sv < MAX_SAT; sv++)
			eph[i][sv].vflg = 0;

	// Read header lines
	while (1)
	{
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;

		if (strncmp(str + 60, "END OF HEADER", 13) == 0)
			break;
		else if (strncmp(str + 60, "ION ALPHA", 9) == 0)
		{
			strncpy(tmp, str + 2, 12);
			tmp[12] = 0;
			replaceExpDesignator(tmp, 12);
			ionoutc->alpha0 = atof(tmp);

			strncpy(tmp, str + 14, 12);
			tmp[12] = 0;
			replaceExpDesignator(tmp, 12);
			ionoutc->alpha1 = atof(tmp);

			strncpy(tmp, str + 26, 12);
			tmp[12] = 0;
			replaceExpDesignator(tmp, 12);
			ionoutc->alpha2 = atof(tmp);

			strncpy(tmp, str + 38, 12);
			tmp[12] = 0;
			replaceExpDesignator(tmp, 12);
			ionoutc->alpha3 = atof(tmp);

			//read wntlsf, dn, and dtlsf from file

			flags |= 0x1;
		}
		else if (strncmp(str + 60, "ION BETA", 8) == 0)
		{
			strncpy(tmp, str + 2, 12);
			tmp[12] = 0;
			replaceExpDesignator(tmp, 12);
			ionoutc->beta0 = atof(tmp);

			strncpy(tmp, str + 14, 12);
			tmp[12] = 0;
			replaceExpDesignator(tmp, 12);
			ionoutc->beta1 = atof(tmp);

			strncpy(tmp, str + 26, 12);
			tmp[12] = 0;
			replaceExpDesignator(tmp, 12);
			ionoutc->beta2 = atof(tmp);

			strncpy(tmp, str + 38, 12);
			tmp[12] = 0;
			replaceExpDesignator(tmp, 12);
			ionoutc->beta3 = atof(tmp);

			flags |= 0x1 << 1;
		}
		else if (strncmp(str + 60, "DELTA-UTC", 9) == 0)
		{
			strncpy(tmp, str + 3, 19);
			tmp[19] = 0;
			replaceExpDesignator(tmp, 19);
			ionoutc->A0 = atof(tmp);

			strncpy(tmp, str + 22, 19);
			tmp[19] = 0;
			replaceExpDesignator(tmp, 19);
			ionoutc->A1 = atof(tmp);

			strncpy(tmp, str + 41, 9);
			tmp[9] = 0;
			ionoutc->tot = atoi(tmp);

			strncpy(tmp, str + 50, 9);
			tmp[9] = 0;
			ionoutc->wnt = atoi(tmp);

			if (ionoutc->tot % 4096 == 0)
				flags |= 0x1 << 2;
		}
		else if (strncmp(str + 60, "LEAP SECONDS", 12) == 0)
		{
			strncpy(tmp, str, 6);
			tmp[6] = 0;
			ionoutc->dtls = atoi(tmp);

			flags |= 0x1 << 3;
		}
	}

	ionoutc->vflg = FALSE;
	if (flags == 0xF) // Read all Iono/UTC lines
		ionoutc->vflg = TRUE;

	// Read ephemeris blocks
	g0.week = -1;
	ieph = 0;

	while (1)
	{
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;

		// PRN
		strncpy(tmp, str, 2);
		tmp[2] = 0;
		sv = atoi(tmp) - 1;

		// EPOCH
		strncpy(tmp, str + 3, 2);
		tmp[2] = 0;
		t.y = atoi(tmp) + 2000;

		strncpy(tmp, str + 6, 2);
		tmp[2] = 0;
		t.m = atoi(tmp);

		strncpy(tmp, str + 9, 2);
		tmp[2] = 0;
		t.d = atoi(tmp);

		strncpy(tmp, str + 12, 2);
		tmp[2] = 0;
		t.hh = atoi(tmp);

		strncpy(tmp, str + 15, 2);
		tmp[2] = 0;
		t.mm = atoi(tmp);

		strncpy(tmp, str + 18, 4);
		tmp[2] = 0;
		t.sec = atof(tmp);

		date2gps(&t, &g);

		if (g0.week == -1)
			g0 = g;

		// Check current time of clock
		dt = subGpsTime(g, g0);

		if (dt > SECONDS_IN_HOUR)
		{
			g0 = g;
			ieph++; // a new set of ephemerides

			if (ieph >= EPHEM_ARRAY_SIZE)
				break;
		}

		// Date and time
		eph[ieph][sv].t = t;

		// SV CLK
		eph[ieph][sv].toc = g;

		strncpy(tmp, str + 22, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19); // tmp[15]='E';
		eph[ieph][sv].af0 = atof(tmp);

		strncpy(tmp, str + 41, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].af1 = atof(tmp);

		strncpy(tmp, str + 60, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].af2 = atof(tmp);

		// BROADCAST ORBIT - 1
		if (NULL==fgets(str, MAX_CHAR, fp))
			break;

		strncpy(tmp, str + 3, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].iode = (int)atof(tmp);

		strncpy(tmp, str + 22, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].crs = atof(tmp);

		strncpy(tmp, str + 41, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].deltan = atof(tmp);

		strncpy(tmp, str + 60, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].m0 = atof(tmp);

		// BROADCAST ORBIT - 2
		if (NULL==fgets(str, MAX_CHAR, fp))
			break;

		strncpy(tmp, str + 3, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].cuc = atof(tmp);

		strncpy(tmp, str + 22, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].ecc = atof(tmp);

		strncpy(tmp, str + 41, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].cus = atof(tmp);

		strncpy(tmp, str + 60, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].sqrta = atof(tmp);

		// BROADCAST ORBIT - 3
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;

		strncpy(tmp, str + 3, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].toe.sec = atof(tmp);

		strncpy(tmp, str + 22, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].cic = atof(tmp);

		strncpy(tmp, str + 41, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].omg0 = atof(tmp);

		strncpy(tmp, str + 60, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].cis = atof(tmp);

		// BROADCAST ORBIT - 4
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;

		strncpy(tmp, str + 3, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].inc0 = atof(tmp);

		strncpy(tmp, str + 22, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].crc = atof(tmp);

		strncpy(tmp, str + 41, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].aop = atof(tmp);

		strncpy(tmp, str + 60, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].omgdot = atof(tmp);

		// BROADCAST ORBIT - 5
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;

		 strncpy(tmp, str + 3, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].idot = atof(tmp);

		strncpy(tmp, str + 22, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].codeL2 = (int)atof(tmp);

		strncpy(tmp, str + 41, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].toe.week = (int)atof(tmp);

		// BROADCAST ORBIT - 6
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;

		strncpy(tmp, str + 22, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].svhlth = (int)atof(tmp);
		if ((eph[ieph][sv].svhlth>0) && (eph[ieph][sv].svhlth<32))
			eph[ieph][sv].svhlth += 32; // Set MSB to 1

		strncpy(tmp, str + 41, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].tgd = atof(tmp);

		strncpy(tmp, str + 60, 19);
		tmp[19] = 0;
		replaceExpDesignator(tmp, 19);
		eph[ieph][sv].iodc = (int)atof(tmp);

		// BROADCAST ORBIT - 7
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;

		// Set valid flag
		eph[ieph][sv].vflg = 1;

		// Update the working variables
		eph[ieph][sv].A = eph[ieph][sv].sqrta * eph[ieph][sv].sqrta;
		eph[ieph][sv].n = sqrt(GM_EARTH / (eph[ieph][sv].A * eph[ieph][sv].A * eph[ieph][sv].A)) + eph[ieph][sv].deltan;
		eph[ieph][sv].sq1e2 = sqrt(1.0 - eph[ieph][sv].ecc * eph[ieph][sv].ecc);
		eph[ieph][sv].omgkdot = eph[ieph][sv].omgdot - OMEGA_EARTH;
	}

	fclose(fp);

	if (g0.week >= 0)
		ieph += 1; // Number of sets of ephemerides

	return (ieph);
}

/*! 
 * \brief Compute ionospheric delay using the Klobuchar model
 * \param[in] ionoutc Ionospheric parameters and UTC parameters
 * \param[in] g GPS time at time of receiving the signal
 * \param[in] llh Geodetic latitude, longitude, and height of the receiver in radians and meters
 * \param[in] azel Azimuth and elevation of the satellite in radians
 * \returns Ionospheric delay in meters
 */
double ionosphericDelay(const ionoutc_t *ionoutc, gpstime_t g, double *llh, double *azel)
{
	if (ionoutc->enable == FALSE)
		return (0.0); // No ionospheric delay
	
	double iono_delay = 0.0;

	double E = azel[1] / PI;
	double phi_u = llh[0] / PI;
	double lam_u = llh[1] / PI;

	// Obliquity factor
	double F = 1.0 + 16.0*pow((0.53 - E),3.0);

	if (ionoutc->vflg==FALSE)
		iono_delay = F * 5.0e-9 * SPEED_OF_LIGHT;
	else
	{
		double t,psi,phi_i,lam_i,phi_m,phi_m2,phi_m3;
		double AMP,PER,X,X2,X4;

		// Earth's central angle between the user position and the earth projection of
		// ionospheric intersection point (semi-circles)
		psi = 0.0137 / (E + 0.11) - 0.022;

		// Geodetic latitude of the earth projection of the ionospheric intersection point
		// (semi-circles)
		phi_i = phi_u + psi * cos(azel[0]);
		if(phi_i > 0.416)
			phi_i = 0.416;
		else if(phi_i < -0.416)
			phi_i = -0.416;

		// Geodetic longitude of the earth projection of the ionospheric intersection point
		// (semi-circles)
		lam_i = lam_u + psi * sin(azel[0]) / cos(phi_i*PI);

		// Geomagnetic latitude of the earth projection of the ionospheric intersection
		// point (mean ionospheric height assumed 350 km) (semi-circles)
		phi_m = phi_i + 0.064 * cos((lam_i - 1.617)*PI);
		phi_m2 = phi_m * phi_m;
		phi_m3 = phi_m2 * phi_m;

		AMP = ionoutc->alpha0 + ionoutc->alpha1 * phi_m
			+ ionoutc->alpha2 * phi_m2 + ionoutc->alpha3 * phi_m3;
		
		if (AMP < 0.0)
			AMP = 0.0;

		PER = ionoutc->beta0 + ionoutc->beta1 * phi_m
			+ ionoutc->beta2 * phi_m2 + ionoutc->beta3 * phi_m3;

		if (PER < 72000.0)
			PER = 72000.0;

		// Local time (sec)
		t = SECONDS_IN_DAY / 2.0 * lam_i + g.sec;
		while (t >= SECONDS_IN_DAY)
			t -= SECONDS_IN_DAY;
		while (t < 0)
			t += SECONDS_IN_DAY;

		// Phase (radians)
		X = 2.0 * PI * (t - 50400.0) / PER;

		if (fabs(X) < 1.57)
		{
			X2 = X * X;
			X4 = X2 * X2;
			iono_delay = F * (5.0e-9 + AMP * (1.0 - X2 / 2.0 + X4 / 24.0)) * SPEED_OF_LIGHT;
		}
		else
			iono_delay = F * 5.0e-9 * SPEED_OF_LIGHT;
	}

	return (iono_delay);
}

/*!
 * \brief Compute range between a satellite and the receiver
 * \param[out] rho The computed range
 * \param[in] eph Ephemeris data of the satellite
 * \param[in] g GPS time at time of receiving the signal
 * \param[in] xyz position of the receiver
 */
void computeRange(range_t *rho, ephem_t eph, ionoutc_t *ionoutc, gpstime_t g, double xyz[])
{
	// SV position at time of the pseudorange observation.
	double pos[3], vel[3], clk[2];
	satpos(&eph, g, pos, vel, clk);

	// Receiver to satellite vector and light-time.
	double los[3];
	subVect(los, pos, xyz);
	double tau = normVect(los)/SPEED_OF_LIGHT;

	// Extrapolate the satellite position backwards to the transmission time.
	pos[0] -= vel[0] * tau;
	pos[1] -= vel[1] * tau;
	pos[2] -= vel[2] * tau;

	// Earth rotation correction. The change in velocity can be neglected.
	double xrot = pos[0] + pos[1] * OMEGA_EARTH * tau;
	double yrot = pos[1] - pos[0] * OMEGA_EARTH * tau;
	pos[0] = xrot;
	pos[1] = yrot;

	// New observer to satellite vector and satellite range.
	subVect(los, pos, xyz);
	double range = normVect(los);
	rho->d = range;

	// Pseudorange.
	rho->range = range - SPEED_OF_LIGHT * clk[0];

	// Relative velocity of SV and receiver.
	double rate = dotProd(vel, los) / range;

	// Pseudorange rate.
	rho->rate = rate; // - SPEED_OF_LIGHT*clk[1];

	// Time of application.
	rho->g = g;

	// Azimuth and elevation angles.
	double llh[3];
	double tmat[3][3];
	double neu[3];
	ecef2llh(xyz, llh);
	ltcmat(llh, tmat);
	ecef2neu(los, tmat, neu);
	neu2azel(rho->azel, neu);

	// Add ionospheric delay
	rho->iono_delay = ionosphericDelay(ionoutc, g, llh, rho->azel);
	rho->range += rho->iono_delay;

	rho->los_ecef[0] = los[0] / range;
	rho->los_ecef[1] = los[1] / range;
	rho->los_ecef[2] = los[2] / range;

	return;
}

/*! 
 * \brief Compute the code phase for a given channel (satellite)
 * \param config Simulation configuration (used to determine the carrier frequency)
 * \param chan Channel on which we operate (is updated)
 * \param[in] rho1 Current range, after \a dt has expired
 * \param[in dt delta-t (time difference) in seconds
 */
void computeCodePhase(sim_config_t *config, channel_t *chan, range_t rho1, double dt)
{
	// Pseudorange rate.
	double rhorate = (rho1.range - chan->rho0.range) / dt;

	// Carrier and code frequency.
	chan->f_carr = -rhorate / LAMBDA[config->carr_freq_index];
	chan->f_code = CODE_FREQ + chan->f_carr * CARR_TO_CODE[config->carr_freq_index];

	// Initial code phase and data bit counters.
	double ms = ((subGpsTime(chan->rho0.g, chan->g0) + 6.0) - chan->rho0.range / SPEED_OF_LIGHT) * 1000.0;

	int ims = (int)ms;
	chan->code_phase = (ms - (double)ims) * CA_SEQ_LEN; // in chip

	chan->iword = ims / 600; // 1 word = 30 bits = 600 ms
	ims -= chan->iword * 600;

	chan->ibit = ims / 20; // 1 bit = 20 code = 20 ms
	ims -= chan->ibit * 20;

	chan->icode = ims; // 1 code = 1 ms

	chan->codeCA = chan->ca[(int)chan->code_phase] * 2 - 1;
	chan->dataBit = (int)((chan->dwrd[chan->iword] >> (29 - chan->ibit)) & 0x1UL) * 2 - 1;

	// Save current pseudorange
	chan->rho0 = rho1;

	return;
}

// File I/O functions

/*! 
 * \brief Read the list of user motions from the input file
 * \param[out] config Simulation configuration
 * Used output: config->has_attitude (set to TRUE if attitude quaternions were read from the file)
 * \param[in] fp File pointer to the opened user motion input file
 * \param[out] xyz ECEF position of the user motion (updated with data read from the file)
 * \param[out] quat Attitude quaternions of the user motion (updated with data read from the file, or set to identity if not available)
 * \returns Number of user data motion records read, -1 on error
 */
int readECEF(sim_config_t *config, FILE *fp, double xyz[USER_MOTION_SIZE][3], double quat[USER_MOTION_SIZE][4])
{
    config->has_attitude = FALSE;

	int numd;
    for (numd = 0; numd < USER_MOTION_SIZE; numd++)
    {
    	char str[MAX_CHAR];

        if (fgets(str, MAX_CHAR, fp) == NULL)
            break;
			
		double t, x, y, z;
		double qw, qx, qy, qz;

        // Try 8-column format first (with quaternion)
        int nread = sscanf(str, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                       &t, &x, &y, &z, &qw, &qx, &qy, &qz);

        if (nread == 8) // The line contains attitude quaternion data
        {
            // Normalise to guard against small numerical drift in the input
            double norm = sqrt(qw * qw + qx * qx + qy * qy + qz * qz);
            if (norm < 1e-9) 
			{
				fprintf(stderr, "ERROR: Zero quaternion at line %d.\n", numd + 1);
				break;
			}
			
            quat[numd][0] = qw / norm;
            quat[numd][1] = qx / norm;
            quat[numd][2] = qy / norm;
            quat[numd][3] = qz / norm;
            config->has_attitude = TRUE;
        }
        else if (nread == 4) // The line contains only position data, but we can still use it in the presence of attitude data (legacy format)
        {
			if (config->has_attitude && numd > 0) // If there was previously attitude data but the current line doesn't have it
			{
				fprintf(stderr, "WARNING: Inconsistent attitude quaternion at line %d, using previous quaternion.\n", numd+1);
				quat[numd][0] = quat[numd - 1][0];
				quat[numd][1] = quat[numd - 1][1];
				quat[numd][2] = quat[numd - 1][2];
				quat[numd][3] = quat[numd - 1][3];
			}
			else // Legacy 4-column format — identity quaternion (zenith-up fallback)
			{
				quat[numd][0] = 1.0;
				quat[numd][1] = quat[numd][2] = quat[numd][3] = 0.0;
			}
        }
        else if (nread == EOF)
		{
            break;
		}
		else
		{
			fprintf(stderr, "ERROR: Invalid file format at line %d.\n", numd+1);
			numd = 0; // Empty user motion
			break;
		}

        xyz[numd][0] = x;
        xyz[numd][1] = y;
        xyz[numd][2] = z;
    }

    if (config->has_attitude)
        fprintf(stderr, "Attitude quaternions loaded (%d records).\n", numd);

    return (numd);
}

/*!
 * \brief Read the list of user motions from the input file
 * \param[out] config Simulation configuration
 * Used output: config->has_attitude (set to TRUE if attitude quaternions were read from the file)
 * \param[in] fp File pointer to the opened user motion input file
 * \param[out] xyz ECEF position of the user motion (updated with data read from the file)
 * \param[out] quat Attitude quaternions of the user motion (updated with data read from the file, or set to identity if not available)
 * \returns Number of user data motion records read, -1 on error
 *
 * Added by romalvarezllorens@gmail.com
 */
int readLLH(sim_config_t *config, FILE *fp, double xyz[USER_MOTION_SIZE][3], double quat[USER_MOTION_SIZE][4])
{
	config->has_attitude = FALSE;

    int numd;
	for (numd = 0; numd < USER_MOTION_SIZE; numd++)
	{
		char str[MAX_CHAR];
		
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;
			
		double t, x, y, z;
		double qw, qx, qy, qz;

		// Try 8-column format first (with quaternion)
        int nread = sscanf(str, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                       &t, &x, &y, &z, &qw, &qx, &qy, &qz);

		if (nread == 8)
		{
			// Normalise to guard against small numerical drift in the input
			double norm = sqrt(qw * qw + qx * qx + qy * qy + qz * qz);
			if (norm < 1e-9) 
			{
				fprintf(stderr, "ERROR: Zero quaternion at line %d.\n", numd + 1);
				break;
			}

			quat[numd][0] = qw / norm;
			quat[numd][1] = qx / norm;
			quat[numd][2] = qy / norm;
			quat[numd][3] = qz / norm;
			config->has_attitude = TRUE;
		}
		else if (nread == 4)
		{
			if (config->has_attitude && numd > 0) // If there was previously attitude data but the current line doesn't have it
			{
				fprintf(stderr, "WARNING: Inconsistent attitude quaternion at line %d, using previous quaternion.\n", numd+1);
				quat[numd][0] = quat[numd - 1][0];
				quat[numd][1] = quat[numd - 1][1];
				quat[numd][2] = quat[numd - 1][2];
				quat[numd][3] = quat[numd - 1][3];
			}
			else // Legacy 4-column format — identity quaternion (zenith-up fallback)
			{
				quat[numd][0] = 1.0;
				quat[numd][1] = quat[numd][2] = quat[numd][3] = 0.0;
			}
		}
		else if (nread == EOF)
		{
			break;
		}
		else
		{
			fprintf(stderr, "ERROR: Invalid file format at line %d.\n", numd+1);
			numd = 0; // Empty user motion
			break;
		}

		double llh[3];
		llh[0] = x; // Latitude in degrees
		llh[1] = y; // Longitude in degrees
		llh[2] = z; // Height in meters

		if (llh[0] > 90.0 || llh[0] < -90.0 || llh[1] > 180.0 || llh[1] < -180.0)
		{
			fprintf(stderr, "ERROR: Invalid file format (time[s], latitude[deg], longitude[deg], height [m].\n");
			numd = 0; // Empty user motion
			break;
		}

		llh[0] /= R2D; // convert to RAD
		llh[1] /= R2D; // convert to RAD

		llh2ecef(llh, xyz[numd]);
	}

	if (config->has_attitude)
		fprintf(stderr, "Attitude quaternions loaded (%d records).\n", numd);

	return (numd);
}

/*!
 * \brief Read the list of user motions from the input file in NMEA GGA format
 * Note: Attitude information is not available in NMEA GGA format, so config->has_attitude is not updated.
 * \param[out] config Simulation configuration (updated with attitude information if available)
 * Used output: config->has_attitude (set to FALSE since NMEA GGA does not contain attitude information)
 * \param[in] fp File pointer to the opened user motion input file in NMEA GGA format
 * \param[out] xyz ECEF position of the user motion (updated with data read from the file)
 * \param[out] quat Attitude quaternions of the user motion (not updated since NMEA GGA does not contain attitude information)
 * \returns Number of user data motion records read, -1 on error
 */
int readNmeaGGA(sim_config_t *config, FILE *fp, double xyz[USER_MOTION_SIZE][3], double quat[USER_MOTION_SIZE][4])
{
	config->has_attitude = FALSE;

	int numd = 0;
	while (1)
	{
		char str[MAX_CHAR];
		if (fgets(str, MAX_CHAR, fp) == NULL)
			break;

		char *token = strtok(str, ",");

		double llh[3], pos[3];

		if (strncmp(token + 3, "GGA", 3) == 0)
		{
			token = strtok(NULL, ","); // Date and time

			token = strtok(NULL, ","); // Latitude
			
			char tmp[8];
			strncpy(tmp, token, 2);

			tmp[2] = 0;

			llh[0] = atof(tmp) + atof(token + 2) / 60.0;

			token = strtok(NULL, ","); // North or south
			if (token[0] == 'S')
				llh[0] *= -1.0;

			llh[0] /= R2D; // in radian

			token = strtok(NULL, ","); // Longitude
			strncpy(tmp, token, 3);
			tmp[3] = 0;

			llh[1] = atof(tmp) + atof(token + 3) / 60.0;

			token = strtok(NULL, ","); // East or west
			if (token[0] == 'W')
				llh[1] *= -1.0;

			llh[1] /= R2D; // in radian

			token = strtok(NULL, ","); // GPS fix
			token = strtok(NULL, ","); // Number of satellites
			token = strtok(NULL, ","); // HDOP

			token = strtok(NULL, ","); // Altitude above meas sea level

			llh[2] = atof(token);

			token = strtok(NULL, ","); // in meter

			token = strtok(NULL, ","); // Geoid height above WGS84 ellipsoid

			llh[2] += atof(token);

			// Convert geodetic position into ECEF coordinates
			llh2ecef(llh, pos);

			xyz[numd][0] = pos[0];
			xyz[numd][1] = pos[1];
			xyz[numd][2] = pos[2];

			// Update the number of track points
			numd++;

			if (numd >= USER_MOTION_SIZE)
				break;
		}
	}

	return (numd);
}

/*!
 * \brief Read the list of user motions from the input file
 * \param[inout] config Simulation configuration (updated with attitude information if available)
 * Used input: config->umfile (file name of user motion input file)
 * Used output: config->has_attitude (set to TRUE if attitude quaternions were read from the file)
 * \param[out] state Simulation state (updated with attitude information if available)
 * Used output: state->xyz (ecef position of the user motion), state->quat (attitude quaternions of the user motion, if available)
 * \returns Number of user data motion records read, -1 on error
 */
int readUserMotion(sim_config_t *config, sim_state_t *state)
{
	FILE *fp;
	if ((fp = fopen(config->umfile, "rt")) == NULL)
		return (-1);

	int user_motion_count = 0;
	if (config->um_nmeaGGA == TRUE)
		user_motion_count = readNmeaGGA(config, fp, state->xyz, state->quat);
	else if (config->um_LLH == TRUE)
		user_motion_count = readLLH(config, fp, state->xyz, state->quat);
	else if (config->um_ECEF == TRUE)
		user_motion_count = readECEF(config, fp, state->xyz, state->quat);
	else
	{
		fprintf(stderr, "ERROR: No user motion input method specified.\n");
		exit(1);
	}

	fclose(fp);

	return (user_motion_count);
}

/*!
 * \brief Generate navigation messages for a given GPS time and channel
 * \param g GPS time
 * \param chan Pointer to the channel structure
 * \param init Initialization flag
 * \returns 1 on success, 0 on failure
 */
int generateNavMsg(gpstime_t g, channel_t *chan, int init)
{
	unsigned long prevwrd = 0UL;
	int nib;

	gpstime_t g0;
	g0.week = g.week;
	g0.sec = (double)(((unsigned long)(g.sec + 0.5)) / 30UL) * 30.0; // Align with the full frame length = 30 sec
	chan->g0 = g0; // Data bit reference time

	unsigned long wn = (unsigned long)(g0.week%1024);
	unsigned long tow = ((unsigned long)g0.sec)/6UL;

	if (init == 1) // Initialize subframe 5
	{
		for (int iwrd = 0; iwrd < N_DWRD_SBF; iwrd++)
		{
			unsigned long sbfwrd = chan->sbf[4][iwrd];

			// Add TOW-count message into HOW
			if (iwrd == 1)
				sbfwrd |= ((tow & 0x1FFFFUL) << 13);

			// Compute checksum
			sbfwrd |= (prevwrd << 30) & 0xC0000000UL; // 2 LSBs of the previous transmitted word
			nib = ((iwrd == 1) || (iwrd == 9)) ? 1 : 0; // Non-information bearing bits for word 2 and 10
			chan->dwrd[iwrd] = computeChecksum(sbfwrd, nib);

			prevwrd = chan->dwrd[iwrd];
		}
	}
	else // Save subframe 5
	{
		for (int iwrd = 0; iwrd < N_DWRD_SBF; iwrd++)
		{
			chan->dwrd[iwrd] = chan->dwrd[N_DWRD_SBF * N_SBF + iwrd];

			prevwrd = chan->dwrd[iwrd];
		}
		/*
		// Sanity check
		if (((chan->dwrd[1])&(0x1FFFFUL<<13)) != ((tow&0x1FFFFUL)<<13))
		{
			fprintf(stderr, "\nWARNING: Invalid TOW in subframe 5.\n");
			return(0);
		}
		*/
	}

	for (int isbf = 0; isbf < N_SBF; isbf++)
	{
		tow++;

		for (int iwrd = 0; iwrd < N_DWRD_SBF; iwrd++)
		{
			unsigned long sbfwrd = chan->sbf[isbf][iwrd];

			// Add transmission week number to Subframe 1
			if ((isbf == 0) && (iwrd == 2))
				sbfwrd |= (wn & 0x3FFUL) << 20;

			// Add TOW-count message into HOW
			if (iwrd == 1)
				sbfwrd |= ((tow & 0x1FFFFUL) << 13);

			// Compute checksum
			sbfwrd |= (prevwrd << 30) & 0xC0000000UL; // 2 LSBs of the previous transmitted word
			nib = ((iwrd == 1) || (iwrd == 9)) ? 1 : 0; // Non-information bearing bits for word 2 and 10
			chan->dwrd[(isbf + 1) * N_DWRD_SBF + iwrd] = computeChecksum(sbfwrd, nib);

			prevwrd = chan->dwrd[(isbf + 1) * N_DWRD_SBF + iwrd];
		}
	}

	return (1);
}

/*!
 * \brief Check satellite visibility from the receiver position
 * \param eph Ephemeris data of the satellite
 * \param g GPS time at time of receiving the signal
 * \param xyz position of the receiver in ECEF coordinates
 * \param elvMask Elevation mask in degrees
 * \param[out] azel Azimuth and elevation of the satellite in radians (output)
 * \returns 1 if the satellite is visible, 0 if it is not visible
 */
int checkSatVisibility(const ephem_t *eph, gpstime_t g, const double xyz[3], double elvMask, double *azel)
{
	if (eph->vflg != 1)
		return (-1); // Invalid

	double llh[3];
	double tmat[3][3];
	ecef2llh(xyz, llh);
	ltcmat(llh, tmat);

	double pos[3], vel[3], clk[3], los[3], neu[3];
	satpos(eph, g, pos, vel, clk);
	subVect(los, pos, xyz);
	ecef2neu(los, tmat, neu);
	neu2azel(azel, neu);

	if (azel[1] * R2D > elvMask)
		return (1); // Visible
	
	return (0); // Invisible
}

/*! 
 * \brief Calculates the L1 C/A EIRP for a specific GPS satellite
 * \param svn The Space Vehicle Number (1 to 82)
 * \param off_boresight_deg The calculated angle between nadir and the LEO receiver (in degrees)
 * \return The L1 C/A EIRP in dBW, or -999.0 if the SVN is invalid or if the block type is unknown
 */
double lerp_eirp(int svn, double off_boresight_deg)
{
	if (svn < 1 || svn >= TOTAL_SV_COUNT) return -999.0;

	enum BLOCK_TYPE block_type = SVN_TO_BLOCK_TYPE[svn];
	if (block_type == INVALID) return -999.0;

	// Maybe return -999?
	if (off_boresight_deg >= TRANSMITTER_DIRECTIVITY_ACCURACY_L1) return -999.0;

	int lower_idx = (int)floor(off_boresight_deg);
	int upper_idx = (int)ceil(off_boresight_deg);

	double lower_eirp = TRANSMITTER_DIRECTIVITY_PATTERN_L1[block_type][lower_idx];
	double upper_eirp = TRANSMITTER_DIRECTIVITY_PATTERN_L1[block_type][upper_idx];

	double weight = off_boresight_deg - lower_idx;

	double directivity = lower_eirp + weight * (upper_eirp - lower_eirp);

	double gain;
	if (GAIN_CORRECTION_FACTOR_L1[svn] != 0.0) {
		gain = directivity + GAIN_CORRECTION_FACTOR_L1[svn];
	}
	else {
		// If no specific gain correction factor is provided for this SVN, use assume -1
		gain = directivity - 1.0; // Default correction factor
	}

	double total_eirp = gain + AMPLIFIER_TRANSMIT_POWER_L1[block_type];

	double ca_eirp = total_eirp - NON_CA_POWER_COMPONENT_L1;

	return ca_eirp;
}

/*!
 * \brief Allocate channels for visible satellites
 * \param config Pointer to the simulation configuration structure
 * \param state Pointer to the simulation state structure
 * \param pos_index Index of the user position in the motion array
 * \param elvMask Elevation mask in degrees
 * \returns Number of visible satellites
 */
int allocateChannel(sim_config_t *config, sim_state_t *state, int pos_index, double elvMask) // CONFIG
{
	int nsat = 0;

	for (int sv = 0; sv < MAX_SAT; sv++)
	{
		double azel[2];
		const ephem_t *eph = &state->eph[state->current_eph_index][sv];

		if (checkSatVisibility(eph, state->current_gps_time, state->xyz[pos_index], elvMask, azel) == 1)
		{
			nsat++; // Number of visible satellites

			if (state->allocatedSat[sv] == -1) // Visible but not allocated
			{
				// Allocated new satellite
				int i;
				for (i = 0; i < MAX_CHAN; i++)
				{
					if (state->channels[i].prn == 0)
					{
						// Initialize channel
						state->channels[i].prn = sv + 1;
						state->channels[i].azel[0] = azel[0];
						state->channels[i].azel[1] = azel[1];

						// C/A code generation
						codegen(state->channels[i].ca, state->channels[i].prn);

						// Generate subframe
						eph2sbf(*eph, config->ionosphere_model, state->channels[i].sbf);

						// Generate navigation message
						generateNavMsg(state->current_gps_time, &state->channels[i], 1);

						// Initialize pseudorange
						range_t rho;
						computeRange(&rho, *eph, &config->ionosphere_model, state->current_gps_time, state->xyz[pos_index]);
						state->channels[i].rho0 = rho;

						computeRange(&rho, *eph, &config->ionosphere_model, state->current_gps_time, state->xyz[pos_index]);

						double phase_ini = 0.0; // TODO: Must initialize properly
						//double phase_ini = (2.0 * r_ref - r_xyz) / LAMBDA[carrFreqIndex];
#ifdef FLOAT_CARR_PHASE
						state->channels[i].carr_phase = phase_ini - floor(phase_ini);
#else
						phase_ini -= floor(phase_ini);
						state->channels[i].carr_phase = (unsigned int)(512.0 * 65536.0 * phase_ini);
#endif
						// Done.
						break;
					}
				}

				// Set satellite allocation channel
				if (i < MAX_CHAN)
					state->allocatedSat[sv] = i;
			}
		}
		else if (state->allocatedSat[sv] >= 0) // Not visible but allocated
		{
			// Clear channel
			state->channels[state->allocatedSat[sv]].prn = 0;

			// Clear satellite allocation flag
			state->allocatedSat[sv] = -1;
		}
	}

	return (nsat);
}

// Utility functions
/*!
 * \brief Print usage information for the program
 */
void usage(void)
{
	fprintf(stderr, "Usage: gps-sdr-sim [options]\n"
		"Options:\n"
		"  -e <gps_nav>     RINEX navigation file for GPS ephemerides (required)\n"
		"  -u <user_motion> User motion file in ECEF x, y, z format (dynamic mode)\n"
		"  -x <user_motion> User motion file in lat, lon, height format (dynamic mode)\n"
		"  -g <nmea_gga>    NMEA GGA stream (dynamic mode)\n"
		"  -c <location>    ECEF X,Y,Z in meters (static mode) e.g. 3967283.154,1022538.181,4872414.484\n"
		"  -l <location>    Lat, lon, height (static mode) e.g. 35.681298,139.766247,10.0\n"
		"  -L <wnslf,dn,dtslf> User leap future event in GPS week number, day number, next leap second e.g. 2347,3,19\n"
		"  -t <date,time>   Scenario start time YYYY/MM/DD,hh:mm:ss\n"
		"  -T <date,time>   Overwrite TOC and TOE to scenario start time\n"
		"  -d <duration>    Duration [sec] (dynamic mode max: %.0f, static mode max: %d)\n"
		"  -o <output>      I/Q sampling data file (default: gpssim.bin)\n"
		"  -s <frequency>   Sampling frequency [Hz] (default: 2600000)\n"
		"  -f <frequency>   Supported carrier frequency [L1/L2/L5] (default: L1)\n"
		"  -b <iq_bits>     I/Q data format [1/8/16] (default: 16)\n"
		"  -i               Disable ionospheric delay for spacecraft scenario\n"
		"  -p [fixed_gain]  Disable path loss and hold power level constant\n"
		"  -v               Show details about simulated channels\n",
		((double)USER_MOTION_SIZE) / 10.0, STATIC_MAX_DURATION);

	return;
}

/*!
 * \brief Parse command-line arguments and initialize simulation configuration and state
 * \param argc Number of command-line arguments
 * \param argv Array of command-line arguments
 * \param config Pointer to the simulation configuration structure
 * \param state Pointer to the simulation state structure
 */
void parseArguments(int argc, char *argv[], sim_config_t *config, sim_state_t *state)
{
	int result;

	while ((result = getopt(argc, argv, "e:u:x:g:c:l:o:s:b:L:T:t:d:f:ipv")) != -1)
	{
		switch (result)
		{
		case 'e': // File name of RINEX navigation file for GPS ephemerides
			{
				strcpy(config->navfile, optarg);

				break;
			}
		case 'u': // File name of user motion in ECEF x, y, z format
			{
				strcpy(config->umfile, optarg);

				config->um_ECEF = TRUE;

				break;
			}
		case 'x': // File name of user motion in lat, lon, height format, Added by romalvarezllorens@gmail.com
			{
				strcpy(config->umfile, optarg);

				config->um_LLH = TRUE;

				break;
			}
		case 'g': // File name of user motion in NMEA GGA format
			{
				strcpy(config->umfile, optarg);

				config->um_nmeaGGA = TRUE;
				
				break;
			}
		case 'c': // Static ECEF coordinates input mode
			{
				config->static_ECEF = TRUE;

				sscanf(optarg, "%lf,%lf,%lf", &state->xyz[0][0], &state->xyz[0][1], &state->xyz[0][2]);

				break;
			}
		case 'l': // Static geodetic coordinates input mode, Added by scateu@gmail.com
			{
				config->static_LLH = TRUE;
				
				double llh[3];
				sscanf(optarg, "%lf,%lf,%lf", &llh[0], &llh[1], &llh[2]);
				
				llh[0] = llh[0] / R2D; // convert to RAD
				llh[1] = llh[1] / R2D; // convert to RAD

				llh2ecef(llh, state->xyz[0]); // Convert llh to xyz

				break;
			}
		case 'o': // Output file name for I/Q sampling 
			{
				strcpy(config->outfile, optarg);

				break;
			}
		case 's': // Sampling frequency
			{
				config->sample_frequency = atof(optarg);

				if (config->sample_frequency < 1.0e6)
				{
					fprintf(stderr, "ERROR: Invalid sampling frequency.\n");
					exit(1);
				}

				break;
			}
		case 'b': // I/Q data format (1-bit, 8-bit, or 16-bit)
			{
				config->data_format = atoi(optarg);

				if (config->data_format != SC01 && config->data_format != SC08 && config->data_format != SC16)
				{
					fprintf(stderr, "ERROR: Invalid I/Q data format.\n");
					exit(1);
				}

				break;
			}
		case 'L':
			{
				// enable custom Leap Event
				config->ionosphere_model.leapen = TRUE;

				sscanf(optarg,"%d,%d,%d", &config->ionosphere_model.wnlsf, &config->ionosphere_model.dn, &config->ionosphere_model.dtlsf);

				if (config->ionosphere_model.dn < 1 || config->ionosphere_model.dn > 7)
				{
					fprintf(stderr, "ERROR: Invalid GPS day number");
					exit(1);
				}
				if (config->ionosphere_model.wnlsf < 0)
				{
					fprintf(stderr, "ERROR: Invalid GPS week number");
					exit(1);
				}
				if (config->ionosphere_model.dtlsf < -128 || config->ionosphere_model.dtlsf > 127)
				{
					fprintf(stderr, "ERROR: Invalid delta leap second");
					exit(1);
				}

				break;
			}
		case 'T':
			{
				config->time_overwrite = TRUE;

				if (strncmp(optarg, "now", 3) == 0)
				{
					time_t timer;
					time(&timer);

					struct tm *gmt = gmtime(&timer);

					datetime_t t0 = {
						.y = gmt->tm_year + 1900,
						.m = gmt->tm_mon + 1,
						.d = gmt->tm_mday,

						.hh = gmt->tm_hour,
						.mm = gmt->tm_min,
						.sec = (double)gmt->tm_sec
					};

					date2gps(&t0, &config->start_time);

					break;
				}
				// Intentional Fall-Through
			}
		case 't':
			{
				datetime_t t0;

				sscanf(optarg, "%d/%d/%d,%d:%d:%lf", &t0.y, &t0.m, &t0.d, &t0.hh, &t0.mm, &t0.sec);

				if (t0.y <= 1980 || t0.m < 1 || t0.m > 12 || t0.d < 1 || t0.d > 31 ||
					t0.hh < 0 || t0.hh > 23 || t0.mm < 0 || t0.mm > 59 || t0.sec < 0.0 || t0.sec >= 60.0)
				{
					fprintf(stderr, "ERROR: Invalid date and time.\n");
					exit(1);
				}

				t0.sec = floor(t0.sec);

				date2gps(&t0, &config->start_time);
				
				break;
			}
		case 'd':
			{
				config->max_duration = atof(optarg);

				if (config->max_duration < 0.0)
				{
					fprintf(stderr, "ERROR: Invalid duration.\n");
					exit(1);
				}

				break;
			}
		case 'f':
			{
				if (strcmp(optarg, "L1") == 0)
					config->carr_freq_index = CARR_L1_INDEX;
				else if (strcmp(optarg, "L2") == 0)
					config->carr_freq_index = CARR_L2_INDEX;
				else if (strcmp(optarg, "L5") == 0)
					config->carr_freq_index = CARR_L5_INDEX;
				else
				{
					fprintf(stderr, "ERROR: Invalid carrier frequency.\n");
					exit(1);
				}
				break;
			}
		case 'i':
			{
				config->ionosphere_model.enable = FALSE; // Disable ionospheric correction
				break;
			}
		case 'p':
			{
				if (optind < argc && argv[optind][0] != '-') // Check if next item is an argument
				{
					config->fixed_gain = atoi(argv[optind]);
					if (config->fixed_gain < 1 || config->fixed_gain > 128)
					{
						fprintf(stderr, "ERROR: Fixed gain must be between 1 and 128.\n");
						exit(1);
					}
					optind++;  // Move past this argument for next iteration
				}
				config->path_loss_enable = FALSE; // Disable path loss
				break;
			}
		case 'v':
			{
				config->verbose = TRUE;
				break;
			}
		case ':':
		case '?':
			{
				usage();
				exit(1);
			}
		default:
			break;
		}
	}

	return;
}

/*!
 * \brief Verify the command-line arguments to ensure they are valid and consistent
 * \param config Pointer to the simulation configuration structure.
 * \param state Pointer to the simulation state structure.
 */
void verifyArguments(sim_config_t *config, sim_state_t *state)
{
	// Ensure the required RINEX navigation file is specified
	if (config->navfile[0] == 0)
	{
		fprintf(stderr, "ERROR: GPS ephemeris file (RINEX navigation file) is not specified.\n");
		exit(1);
	}

	// Ensure only one input method for user motion is specified
	int user_motion_inputs = 0;
	if (config->um_ECEF)			user_motion_inputs++;
	if (config->um_LLH)				user_motion_inputs++;
	if (config->um_nmeaGGA)			user_motion_inputs++;
	if (config->static_LLH)			user_motion_inputs++;
	if (config->static_ECEF)		user_motion_inputs++;
	if (user_motion_inputs > 1)
	{
		fprintf(stderr, "ERROR: Multiple user motion input methods specified. Please specify only one of -u, -x, -g, -c, or -l.\n");
		exit(1);
	}
	
	// If no user motion file is specified, use default static location (Tokyo)
	if (user_motion_inputs == 0)
	{
		fprintf(stderr, "No user motion input specified, using default static location (Tokyo).\n");

		config->static_LLH = TRUE;

		double llh[3];
		llh[0] = 35.681298 / R2D;
		llh[1] = 139.766247 / R2D;
		llh[2] = 10.0;

		llh2ecef(llh, state->xyz[0]);
	}

	int staticLocationMode = config->static_ECEF || config->static_LLH;
	double max_allowed = staticLocationMode ? (double)STATIC_MAX_DURATION : ((double)USER_MOTION_SIZE) / 10.0;

	if (config->max_duration > max_allowed)
	{
		fprintf(stderr, "ERROR: Duration %.1f exceeds maximum %.1f for %s mode.\n",
			config->max_duration, max_allowed,
			staticLocationMode ? "static" : "dynamic");
		exit(1);
	}

	// Floor the frequency to the nearest 10 Hz to ensure it is compatible with the internal processing
	config->sample_frequency = floor(config->sample_frequency / 10.0) * 10.0;
}

/*!
 * \brief Initialize the simulation state based on the configuration
 * \param config Pointer to the simulation configuration structure
 * \param state Pointer to the simulation state structure
 * Notes:
 *  - Loads user motion data if in dynamic mode, otherwise sets user motion data based on static location input
 *  - Loads ephemeris data from the RINEX navigation file and determines the GPS time range covered by the ephemeris data
 *  - Validates the scenario start time against the GPS time range covered by the ephemeris data and initializes it if not set
 */
void initializeSimulation(sim_config_t *config, sim_state_t *state)
{
	int staticLocationMode = config->static_ECEF || config->static_LLH;

	// Load user motion data if in dynamic mode, otherwise the user motion data will be set later based on the static location input
	int user_motion_count = -1;
	if (!staticLocationMode)
	{
		user_motion_count = readUserMotion(config, state);

		if (user_motion_count == -1)
		{
			fprintf(stderr, "ERROR: Failed to open user motion file.\n");
			exit(1);
		}
		else if (user_motion_count == 0)
		{
			fprintf(stderr, "ERROR: Failed to read user motion data.\n");
			exit(1);
		}
	}
	else
	{
		user_motion_count = (int)(config->max_duration * 10.0 + 0.5);

		fprintf(stderr, "Using static location mode.\n");
	}
	
	// Compare the number of user motion records read with maximum duration specified
	if (user_motion_count < config->max_duration * 10.0)
	{
		config->max_duration = ((double)user_motion_count) / 10.0;
	}
	// Truncate if necessary
	else if (user_motion_count > config->max_duration * 10.0)
	{
		user_motion_count = (int)(config->max_duration * 10.0 + 0.5);

		fprintf(stderr, "Truncating user motion data to %d records based on specified duration.\n", user_motion_count);
	}

	// Number of ephemeris records read from the RINEX navigation file, and ionospheric/UTC parameters
	state->eph_count = readRinexNavAll(state->eph, &config->ionosphere_model, config->navfile);
	
	// Check if ephemeris data is available
	if (state->eph_count == -1)
	{
		fprintf(stderr, "ERROR: ephemeris file not found.\n");
		exit(1);
	}
	else if (state->eph_count == 0)
	{
		fprintf(stderr, "ERROR: No ephemeris available.\n");
		exit(1);
	}


	// Determine the minimum and maximum GPS time covered by the ephemeris data to validate the scenario start time and to initialize satellite allocation
	datetime_t tmin = {0}, tmax = {0};
	gpstime_t gmin = {0}, gmax = {0};

	// Loop through the ephemeris data to find the earliest and latest GPS times
	// Consider changing how RINEX ephemeris data is stored to make this more efficient
	for (int sat_idx = 0; sat_idx < MAX_SAT; sat_idx++)
	{
		if (state->eph[0][sat_idx].vflg == 1)
		{
			gmin = state->eph[0][sat_idx].toc;
			tmin = state->eph[0][sat_idx].t;
			break;
		}
	}

	// Loop through the ephemeris data in reverse order to find the latest GPS time
	for (int sat_idx = 0; sat_idx < MAX_SAT; sat_idx++)
	{
		if (state->eph[state->eph_count - 1][sat_idx].vflg == 1)
		{
			gmax = state->eph[state->eph_count - 1][sat_idx].toc;
			tmax = state->eph[state->eph_count - 1][sat_idx].t;
			break;
		}
	}

	// Long complicated section, but it basically checks if the scenario start time (state->gps_time) is within the range of GPS times covered by the ephemeris data
	// If the scenario start time is not set, it initializes it to the minimum GPS time covered by the ephemeris data 
	if (config->start_time.week >= 0) // Scenario start time has been set.
	{
		if (config->time_overwrite == TRUE)
		{
			gpstime_t gtmp;
			datetime_t ttmp;
			double dsec;

			gtmp.week = config->start_time.week;
			gtmp.sec = (double)(((int)(config->start_time.sec)) / 7200) * 7200.0;

			dsec = subGpsTime(gtmp, gmin);

			// Overwrite the UTC reference week number
			config->ionosphere_model.wnt = gtmp.week;
			config->ionosphere_model.tot = (int)gtmp.sec;

			// Iono/UTC parameters may no longer valid
			//config->ionosphere_model.vflg = FALSE;

			// Overwrite the TOC and TOE to the scenario start time
			for (int sat_idx = 0; sat_idx < MAX_SAT; sat_idx++)
			{
				for (int i = 0; i < state->eph_count; i++)
				{
					if (state->eph[i][sat_idx].vflg == 1)
					{
						gtmp = incGpsTime(state->eph[i][sat_idx].toc, dsec);
						gps2date(&gtmp,&ttmp);
						state->eph[i][sat_idx].toc = gtmp;
						state->eph[i][sat_idx].t = ttmp;

						gtmp = incGpsTime(state->eph[i][sat_idx].toe, dsec);
						state->eph[i][sat_idx].toe = gtmp;
					}
				}
			}
		}
		else
		{
			if (subGpsTime(config->start_time, gmin) < 0.0 || subGpsTime(gmax, config->start_time) < 0.0)
			{
				fprintf(stderr, "ERROR: Invalid start time.\n");
				fprintf(stderr, "tmin = %4d/%02d/%02d,%02d:%02d:%02.0f (%d:%.0f)\n",
					tmin.y, tmin.m, tmin.d, tmin.hh, tmin.mm, tmin.sec,
					gmin.week, gmin.sec);
				fprintf(stderr, "tmax = %4d/%02d/%02d,%02d:%02d:%02.0f (%d:%.0f)\n",
					tmax.y, tmax.m, tmax.d, tmax.hh, tmax.mm, tmax.sec,
					gmax.week, gmax.sec);
				exit(1);
			}
		}
	}
	else
	{
		config->start_time = gmin;
	}

	// Open output file for I/Q sampling data, or set to stdout if output file is "-"
	if (strcmp("-", config->outfile))
	{
		if ((state->fp_out = fopen(config->outfile, "wb")) == NULL)
		{
			fprintf(stderr, "ERROR: Failed to open output file.\n");
			exit(1);
		}
	}
	else 
	{
		state->fp_out = stdout;
	}

	// Identify the number of bytes needed for the I/Q buffer based on the specified data format and sampling frequency
	state->iq_buff_size = config->sample_frequency / 10; // Number of samples for 0.1 second duration

	if      (config->data_format == SC16) state->iq_buff_bytes = 4 * state->iq_buff_size; // 2 * short
	else if (config->data_format == SC08) state->iq_buff_bytes = 2 * state->iq_buff_size; // 2 * char
	else if (config->data_format == SC01)  state->iq_buff_bytes = state->iq_buff_size / 4;  // 2 bits/sample packed
	else
	{
		fprintf(stderr, "ERROR: Invalid I/Q data format.\n");
		exit(1);
	}

	// Allocate I/Q buffer
	state->iq_buff = calloc(state->iq_buff_bytes, 1);
	if (state->iq_buff == NULL)
	{
		fprintf(stderr, "ERROR: Failed to allocate %d-bit I/Q buffer.\n", config->data_format);
		exit(1);
	}

	// Select the current set of ephemerides
	for (int i = 0; i < state->eph_count; i++)
	{
		for (int sat_idx = 0; sat_idx < MAX_SAT; sat_idx++)
		{
			if (state->eph[i][sat_idx].vflg == 1)
			{
				double dt = subGpsTime(config->start_time, state->eph[i][sat_idx].toc);
				if (dt >= -SECONDS_IN_HOUR && dt < SECONDS_IN_HOUR)
				{
					state->current_eph_index = i;
					break;
				}
			}
		}

		if (state->current_eph_index >= 0) // current_eph_index has been set
			break;
	}

	if (state->current_eph_index == -1)
	{
		fprintf(stderr, "ERROR: No current set of ephemerides has been found.\n");
		exit(1);
	}

	// Clear all channels
	for (int i = 0; i < MAX_CHAN; i++)
		state->channels[i].prn = 0;

	// Clear satellite allocation flag
	// Setting to default config should be sufficient, but doing it here for clarity and safety
	for (int i = 0; i < MAX_SAT; i++)
		state->allocatedSat[i] = -1;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		usage();
		exit(1);
	}
	
	sim_config_t config = DEFAULT_SIM_CONFIG;
	sim_state_t state = DEFAULT_SIM_STATE;

	// Parse user arguments to populate the config and state structures
	parseArguments(argc, argv, &config, &state);
	
	// Input validation
	verifyArguments(&config, &state);

	// Initialize the simulation state based on the configuration and user motion input
	initializeSimulation(&config, &state);

	// Determine if we are in static location mode (either static ECEF or static LLH)
	int staticLocationMode = config.static_ECEF || config.static_LLH;

	// Log initial position
	double llh[3];
	ecef2llh(state.xyz[0], llh);
	fprintf(stderr, "Initial position:\n");
	fprintf(stderr, "xyz = %11.1f, %11.1f, %11.1f\n", state.xyz[0][0], state.xyz[0][1], state.xyz[0][2]);
	fprintf(stderr, "llh = %11.6f, %11.6f, %11.1f\n", llh[0] * R2D, llh[1] * R2D, llh[2]);

	// Log ionospheric model parameters if enabled and verbose mode is on
	if ((config.verbose == TRUE) && (config.ionosphere_model.vflg == TRUE))
	{
		fprintf(stderr, "  %12.3e %12.3e %12.3e %12.3e\n",
			config.ionosphere_model.alpha0, config.ionosphere_model.alpha1, config.ionosphere_model.alpha2, config.ionosphere_model.alpha3);
		fprintf(stderr, "  %12.3e %12.3e %12.3e %12.3e\n",
			config.ionosphere_model.beta0, config.ionosphere_model.beta1, config.ionosphere_model.beta2, config.ionosphere_model.beta3);
		fprintf(stderr, "   %19.11e %19.11e  %9d %9d\n",
			config.ionosphere_model.A0, config.ionosphere_model.A1, config.ionosphere_model.tot, config.ionosphere_model.wnt);
		fprintf(stderr, "%6d\n", config.ionosphere_model.dtls);
	}
	
	// Log scenario start time and duration
	datetime_t t0;
	gps2date(&config.start_time, &t0);
	fprintf(stderr, "Start time = %4d/%02d/%02d,%02d:%02d:%02.0f (%d:%.0f)\n",
		t0.y, t0.m, t0.d, t0.hh, t0.mm, t0.sec, config.start_time.week, config.start_time.sec);
	fprintf(stderr, "Duration = %.1f [sec]\n", config.max_duration);

	// Initial reception time
	state.current_gps_time = incGpsTime(config.start_time, 0.0);
	
	// Elevation mask for satellite visibility check during channel allocation
	// Currently assuming constant height
	double h = llh[2] > 0.0 ? llh[2] : 0.0; // Ensure non-negative height for mask calculation
	double elv_mask = -acos(WGS84_RADIUS / (WGS84_RADIUS + h)) * R2D; // in degrees
	allocateChannel(&config, &state, 0, elv_mask);

	// Log allocated channels
	for (int i = 0; i < MAX_CHAN; i++)
	{
		channel_t chan = state.channels[i];
		if (chan.prn > 0)
			fprintf(stderr, "%02d %6.1f %5.1f %11.1f %5.1f\n", chan.prn,
				chan.azel[0] * R2D, chan.azel[1] * R2D, chan.rho0.d, chan.rho0.iono_delay);
	}

	////////////////////////////////////////////////////////////
	// Receiver antenna gain pattern
	////////////////////////////////////////////////////////////

	// Load the antenna gain pattern in dB for boresight angles 0, 5, ..., 180 degrees
	double ant_pat[37];
	for (int i = 0; i < 37; i++)
		ant_pat[i] = pow(10.0, -ant_pat_db[i] / 20.0);

	////////////////////////////////////////////////////////////
	// Run the simulation loop
	////////////////////////////////////////////////////////////

	// Timing variables for runtime performance measurement
	clock_t tstart, tend;

	tstart = clock();

	// Update receiver time by 0.1 seconds
	state.current_gps_time = incGpsTime(state.current_gps_time, 0.1);

	// Iterate through each user motion record (or static location) for each 0.1 second step
	float gain[MAX_CHAN];
	for (int time_step = 1; time_step < config.max_duration * 10; time_step++)
	{
		double delt = 1.0 / config.sample_frequency;

		// Simulate this time step for each allocated channel
		for (int i = 0; i < MAX_CHAN; i++)
		{
			if (state.channels[i].prn <= 0) // Skip unallocated channels
				continue;

			int boresight_idx = -1;
			int sat_idx = state.channels[i].prn - 1;

			// Refresh code phase and data bit counters

			range_t rho;
			// index = 0 if static location mode, otherwise index = time_step for dynamic user motion
			int index = time_step * (!staticLocationMode);
			computeRange(&rho, state.eph[state.current_eph_index][sat_idx], &config.ionosphere_model, state.current_gps_time, state.xyz[index]);

			range_t rho_next;
			gpstime_t next_gps_time = incGpsTime(state.current_gps_time, 0.1);
			int next_index = time_step + 1 < config.max_duration * 10 ? (time_step + 1) * (!staticLocationMode) : index;
			computeRange(&rho_next, state.eph[state.current_eph_index][sat_idx], &config.ionosphere_model, next_gps_time, state.xyz[next_index]);

			double v_rx[3];
			v_rx[0] = (state.xyz[next_index][0] - state.xyz[index][0]) / 0.1;
			v_rx[1] = (state.xyz[next_index][1] - state.xyz[index][1]) / 0.1;
			v_rx[2] = (state.xyz[next_index][2] - state.xyz[index][2]) / 0.1;

			// Project the receiver velocity onto the LOS vector (receiver -> satellite)
			double v_rx_los_0 = v_rx[0] * rho.los_ecef[0] + v_rx[1] * rho.los_ecef[1] + v_rx[2] * rho.los_ecef[2];
			double v_rx_los_1 = v_rx[0] * rho_next.los_ecef[0] + v_rx[1] * rho_next.los_ecef[1] + v_rx[2] * rho_next.los_ecef[2];

			double true_rate_0 = rho.rate - v_rx_los_0;
			double true_rate_1 = rho_next.rate - v_rx_los_1;

			double f_carr_0 = -(true_rate_0 / LAMBDA[config.carr_freq_index]); 
			double f_carr_1 = -(true_rate_1 / LAMBDA[config.carr_freq_index]);

			double f_rate = (f_carr_1 - f_carr_0) / 0.1;

			state.channels[i].azel[0] = rho.azel[0];
			state.channels[i].azel[1] = rho.azel[1];

			state.channels[i].carr_phase_step = f_carr_0 * delt + 0.5 * f_rate * delt * delt;
			state.channels[i].carr_phase_accel = f_rate * delt * delt;

			// Update code phase and data bit counters
			computeCodePhase(&config, &state.channels[i], rho, 0.1);

#ifndef FLOAT_CARR_PHASE
			// Scale the float steps into the 32-bit fixed-point accumulator format
			state.channels[i].carr_phasestep_int = (int)round(512.0 * 65536.0 * state.channels[i].carr_phase_step);
			state.channels[i].carr_phase_accel_int = (int)round(512.0 * 65536.0 * state.channels[i].carr_phase_accel);
#endif

			// If path loss is disabled, use fixed gain for all channels and skip the rest of the calculations
			if (!config.path_loss_enable)
			{
				gain[i] = config.fixed_gain;
				continue;
			}

			// Calculate the LOS from satellite to receiver to find the off-boresight angle for the antenna gain pattern
			double sat_pos[3];
			sat_pos[0] = state.xyz[index][0] + (rho.los_ecef[0] * rho.d);
			sat_pos[1] = state.xyz[index][1] + (rho.los_ecef[1] * rho.d);
			sat_pos[2] = state.xyz[index][2] + (rho.los_ecef[2] * rho.d);

			// cos(a) where a is the angle between LOS and satellite position vector
			double cos_alpha = dotProd(rho.los_ecef, sat_pos) / normVect(sat_pos);

			// Clamp
			cos_alpha = (cos_alpha > 1.0) ? 1.0 : ((cos_alpha < -1.0) ? -1.0 : cos_alpha);

			double off_boresight_deg = acos(cos_alpha) * R2D;
			
			// Get the PRN and corresponding SVN for this channel to look up the EIRP pattern for this satellite
			int prn = state.channels[i].prn;
			int svn = PRN_TO_SVN[prn];
			if (svn <= 0 || svn >= TOTAL_SV_COUNT || SVN_TO_BLOCK_TYPE[svn] == INVALID) {
				fprintf(stderr, "WARNING: Invalid SVN %d for PRN %d. Setting gain to 0.\n", svn, prn);
				gain[i] = 0.0;
				continue;
			}

			double ca_eirp = lerp_eirp(svn, off_boresight_deg);
			if (ca_eirp < -160.0)
			{
				gain[i] = 0.0;
				continue;
			}

			// Friis Path Loss calculation
			double FSPL = 20 * log10(rho.d) + PATH_LOSS_DB_OFFSETS[config.carr_freq_index];

			// Receiver antenna gain
			if (config.has_attitude && !staticLocationMode)
			{
				/*
				* Rotate the unit LOS vector (receiver -> satellite, ECEF) into the
				* receiver body frame using the attitude quaternion for this epoch.
				*
				* Convention: antenna boresight = +Z body axis.
				* For a nadir-pointing spacecraft swap to -Z: use -los_body[2].
				*
				* Boresight angle θ = acos( dot(los_body, [0,0,1]) ) = acos(los_body[2])
				*/
				double los_body[3];
				quatRotVect(los_body, state.quat[time_step], rho.los_ecef);

				double cos_bs = los_body[2]; // dot with +Z boresight
				cos_bs = (cos_bs >  1.0) ?  1.0 :
						(cos_bs < -1.0) ? -1.0 : cos_bs; // numerical clamp
				double boresight_deg = acos(cos_bs) * R2D;

				boresight_idx = (int)(boresight_deg / 5.0);
				if (boresight_idx > 36) boresight_idx = 36; // pattern table bound
			}
			else
			{
				// Terrestrial / static fallback: zenith-up, symmetric pattern
				boresight_idx = (int)((90.0 - rho.azel[1] * R2D) / 5.0);
				if (boresight_idx < 0)  boresight_idx = 0;
				if (boresight_idx > 36) boresight_idx = 36;
			}
			
			double prx_dbw_at_antenna = ca_eirp - FSPL; // Received power at the antenna input in dBW

			// Convert dBW to linear scale and apply scaling factor for I/Q generation
			double baseline_dbw = MINIMUM_RECEIVED_DBW;
			double relative_power_db = prx_dbw_at_antenna - baseline_dbw;

			double physical_multiplier = pow(10.0, relative_power_db / 20.0);
			
			double antenna_gain = ant_pat[boresight_idx];

			// Signal gain
			gain[i] = physical_multiplier * antenna_gain * 128.0;
		}

		// Reconstruct the I/Q stream for this 0.1 second step
		for (int sample_idx = 0; sample_idx < state.iq_buff_size; sample_idx++)
		{
			float i_acc = 0;
			float q_acc = 0;

			for (int i = 0; i < MAX_CHAN; i++)
			{
				if (state.channels[i].prn <= 0) // Skip unallocated channels
					continue;

#ifdef FLOAT_CARR_PHASE
				int table_idx = (int)floor(state.channels[i].carr_phase * 512.0);
#else
				int table_idx = (state.channels[i].carr_phase >> 16) & 0x1ff; // 9-bit index
#endif
				float ip = state.channels[i].dataBit * state.channels[i].codeCA * cosTable512[table_idx] * gain[i];
				float qp = state.channels[i].dataBit * state.channels[i].codeCA * sinTable512[table_idx] * gain[i];

				// Accumulate for all visible satellites
				i_acc += ip;
				q_acc += qp;

				// Update code phase
				state.channels[i].code_phase += state.channels[i].f_code * delt;

				if (state.channels[i].code_phase >= CA_SEQ_LEN)
				{
					state.channels[i].code_phase -= CA_SEQ_LEN;

					state.channels[i].icode++;

					if (state.channels[i].icode >= 20) // 20 C/A codes = 1 navigation data bit
					{
						state.channels[i].icode = 0;
						state.channels[i].ibit++;

						if (state.channels[i].ibit >= 30) // 30 navigation data bits = 1 word
						{
							state.channels[i].ibit = 0;
							state.channels[i].iword++;
							/*
							if (state.channels[i].iword>=N_DWRD)
								fprintf(stderr, "\nWARNING: Subframe word buffer overflow.\n");
							*/
						}

						// Set new navigation data bit
						state.channels[i].dataBit = (int)((state.channels[i].dwrd[state.channels[i].iword] >> (29 - state.channels[i].ibit)) & 0x1UL) * 2 - 1;
					}
				}

				// Set current code chip
				state.channels[i].codeCA = state.channels[i].ca[(int)state.channels[i].code_phase] * 2 - 1;

				// Update carrier phase
#ifdef FLOAT_CARR_PHASE
				state.channels[i].carr_phase += state.channels[i].carr_phase_step;
				
				state.channels[i].carr_phase_step += state.channels[i].carr_phase_accel;

				if (state.channels[i].carr_phase >= 1.0)
					state.channels[i].carr_phase -= 1.0;
				else if (state.channels[i].carr_phase < 0.0)
					state.channels[i].carr_phase += 1.0;
#else
				state.channels[i].carr_phase += state.channels[i].carr_phasestep_int;
				state.channels[i].carr_phasestep_int += state.channels[i].carr_phase_accel_int;
#endif
			}

			// Scaled by 2^7
			i_acc = i_acc / 128.0;
			q_acc = q_acc / 128.0;

			// Store I/Q samples into buffer
			if (config.data_format == SC16)
			{
				((short *)state.iq_buff)[sample_idx * 2]     = (short)i_acc;
				((short *)state.iq_buff)[sample_idx * 2 + 1] = (short)q_acc;
			}
			else if (config.data_format == SC08)
			{
				((signed char *)state.iq_buff)[sample_idx * 2]     = (signed char)i_acc;
				((signed char *)state.iq_buff)[sample_idx * 2 + 1] = (signed char)q_acc;
			}
			else if (config.data_format == SC01) // pack 2 bits (I then Q) per sample into bytes
			{ // Might be an inssue with scaling r/ now
				if (sample_idx % 4 == 0)
					((unsigned char *)state.iq_buff)[sample_idx / 4] = 0x00;
				((unsigned char *)state.iq_buff)[sample_idx / 4] |= (i_acc > 0 ? 1 : 0) << (7 - (sample_idx * 2)     % 8);
				((unsigned char *)state.iq_buff)[sample_idx / 4] |= (q_acc > 0 ? 1 : 0) << (7 - (sample_idx * 2 + 1) % 8);
			}
			else
			{
				fprintf(stderr, "ERROR: Invalid data format.\n");
				exit(1);
			}
		}

		fwrite(state.iq_buff, 1, state.iq_buff_bytes, state.fp_out);

		//
		// Update navigation message and channel allocation every 30 seconds
		//

		int igrx = (int)(state.current_gps_time.sec * 10.0 + 0.5);

		if (igrx % 300 == 0) // Every 30 seconds
		{
			// Update navigation message
			for (int i = 0; i < MAX_CHAN; i++)
			{
				if (state.channels[i].prn > 0)
					generateNavMsg(state.current_gps_time, &state.channels[i], 0);
			}

			// Refresh ephemeris and subframes
			// Quick and dirty fix. Need more elegant way.
			for (int sat_idx = 0; sat_idx < MAX_SAT; sat_idx++)
			{
				if (state.eph[state.current_eph_index + 1][sat_idx].vflg == 1)
				{
					double dt = subGpsTime(state.eph[state.current_eph_index + 1][sat_idx].toc, state.current_gps_time);
					if (dt < SECONDS_IN_HOUR)
					{
						state.current_eph_index++;

						for (int i = 0; i < MAX_CHAN; i++)
						{
							// Generate new subframes if allocated
							if (state.channels[i].prn != 0)
								eph2sbf(state.eph[state.current_eph_index][state.channels[i].prn - 1], config.ionosphere_model, state.channels[i].sbf);
						}
					}

					break;
				}
			}

			// Update channel allocation
			// if (!staticLocationMode)
			// 	allocateChannel(&config, &state, state.eph[state.current_eph_index], time_step, elv_mask);
			// else
			// 	allocateChannel(&config, &state, state.eph[state.current_eph_index], 0, elv_mask);

			// Acts exactly like the code above
			int index = time_step * (!staticLocationMode);
			allocateChannel(&config, &state, index, elv_mask);

			// Show details about simulated channels
			if (config.verbose == TRUE)
			{
				fprintf(stderr, "\n");
				for (int i = 0; i < MAX_CHAN; i++)
				{
					if (state.channels[i].prn > 0)
						fprintf(stderr, "%02d %6.1f %5.1f %11.1f %5.1f\n", state.channels[i].prn,
							state.channels[i].azel[0] * R2D, state.channels[i].azel[1] * R2D, state.channels[i].rho0.d, state.channels[i].rho0.iono_delay);
				}
			}
		}

		// Update receiver time
		state.current_gps_time = incGpsTime(state.current_gps_time, 0.1);

		// Update time counter
		fprintf(stderr, "\rTime into run = %4.1f", subGpsTime(state.current_gps_time, config.start_time));
		fflush(stderr);
	}

	tend = clock();

	fprintf(stderr, "\nDone!\n");

	// Free I/Q buffer
	free(state.iq_buff);

	// Close file
	fclose(state.fp_out);

	// Process time
	fprintf(stderr, "Process time = %.1f [sec]\n", (double)(tend - tstart) / CLOCKS_PER_SEC);

	return(0);
}
