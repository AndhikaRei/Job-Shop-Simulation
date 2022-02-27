#ifndef __JOBSHOP_H__
#define __JOBSHOP_H__

/* Event detail */
/* Event type for arrival of a job to the system. */
#define EVENT_ARRIVAL         1
/* Event type for departure of a job from aparticular station. */	
#define EVENT_DEPARTURE       2	
/* Event type for end of the simulation. */
#define EVENT_END_SIMULATION  3	
/* Event type for Arrival in jobshop 1. */
#define EVENT_ARRIVAL_JOBSHOP_1       4
/* Event type for Departure in jobshop 1. */
#define EVENT_DEPARTURE_JOBSHOP_1     5
/* Event type for Arrival in jobshop 2. */
#define EVENT_ARRIVAL_JOBSHOP_2       6
/* Event type for Departure in jobshop 2. */
#define EVENT_DEPARTURE_JOBSHOP_2     7
/* Event type for Arrival in jobshop 3. */
#define EVENT_ARRIVAL_JOBSHOP_3       8
/* Event type for Departure in jobshop 3. */
#define EVENT_DEPARTURE_JOBSHOP_3     9

/* Jobshop specification */
/* Number of jobshop */
#define NUM_JOBSHOP 				 3
/* Maximum number of stations in each jobshop. */	
#define MAX_NUM_STATIONS      5
/* Maximum number of job types. */	
#define MAX_NUM_JOB_TYPES     3	
/* Random-number stream for interarrivals. */
#define STREAM_INTERARRIVAL   1
/* Random-number stream for job types. */	
#define STREAM_JOB_TYPE       2
/* Random-number stream for service times. */	
#define STREAM_SERVICE        3
/* Random-number stream for jobshop arrival*/
#define STREAM_JOBSHOP_ARRIVAL 2

/* Variable*/
/* Number of station */
extern int num_stations; 
/* Number of job types */
extern int num_job_types; 
/* Loop index */
extern int i;
/* Loop index */
extern int j; 
/* Number of machines in each station */
extern int num_machines[MAX_NUM_STATIONS + 1];
/* Number of tasks in each job type */
extern int num_tasks[MAX_NUM_JOB_TYPES + 1];
/* Route matrix */
extern int route[MAX_NUM_JOB_TYPES + 1][MAX_NUM_STATIONS + 1]; 
/* Number of used machine in each station */
extern int num_machines_busy[NUM_JOBSHOP + 1][MAX_NUM_STATIONS + 1], job_type, task;
/* Type of the job*/
extern int job_type;
/* Task of the job (one job can have multiple task)*/
extern int task;
/*Current jobshop number*/
extern int jobshop_number;
/* Previous JobShop Number */
extern int previous_jobshop_number;
/* Previous Job Type*/
extern int previous_job_type;

/* Mean interarrival time of each job */
extern double mean_interarrival;
/* Length of simulation (in hour)*/
extern double length_simulation; 
/* Probability distribution for each job type */
extern double prob_distrib_job_type[26];
/* Mean service time for each job type and station */
extern double mean_service[MAX_NUM_JOB_TYPES + 1][MAX_NUM_STATIONS + 1];
/*Probability distribution of arriving in JobShop 1 or 2*/
extern double prob_distrib_job_shop[3];

/* File to read input */
extern FILE *infile;
/* File to write output */
extern FILE *outfile;

/* Function */
/**
 * @brief Read JobsShop specification from the input file. The specification includes the 
 * number of job types, the number of tasks for each job type, the number of stations, 
 * the number of machines at each station, the mean interarrival time, the mean service time for 
 * each job type and task, and the probability distribution for each job type.
 *  
 */
void readJobShopParameter(void);

/**
 * @brief Initialize the JobShop simulation. 
 * 
 */
void initJobShop(void);

/**
 * @brief Arrive a new job to the system or to new station.
 * 
 * @param int new_job - 1 if a new job arrives, 0 if a new station arrives.
 * @param int arrival_jobshop_number - jobshop number.
 * 
 */
void arrive (int new_job, int arrival_jobshop_number);

/**
 * @brief Departure job from particular station.
 * 
 * @param int departure_jobshop_number - jobshop number.
 */
void depart (int departure_jobshop_number);

/**
 * @brief Print the simulation result.
 * 
 */
void report (void);

/**
 * @brief Get event arrival Jobshop Number.
 * 
 * @param int jobshop_number
 * @return int
 */
int getArrivalJobShop(int jobshop_number){
	switch (jobshop_number) {
		case 1:
			return EVENT_ARRIVAL_JOBSHOP_1;
		case 2:
			return EVENT_ARRIVAL_JOBSHOP_2;
		case 3:
			return EVENT_ARRIVAL_JOBSHOP_3;
	}
}

/**
 * @brief Get event departure Jobshop Number.
 * 
 * @param int jobshop_number
 * @return int
 */
int getDepartureJobShop(int jobshop_number){
	switch (jobshop_number) {
		case 1:
			return EVENT_DEPARTURE_JOBSHOP_1;
		case 2:
			return EVENT_DEPARTURE_JOBSHOP_2;
		case 3:
			return EVENT_DEPARTURE_JOBSHOP_3;
	}
}

/**
 * @brief Get list queue of specific station in specific JobShop.
 * 
 * @param int jobshop_number
 * @param int station_number
 * @return int
 */
int getListQueue(int jobshop_number, int station_number){
	// The rule is: listQueue = (jobshop_number - 1) * num_stations + station_number
	return (jobshop_number - 1) * num_stations + station_number;
}
#endif