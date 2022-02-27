#ifndef __JOBSHOP_H__
#define __JOBSHOP_H__

/* Event detail */
/* Event type for arrival of a job to the system. */
#define EVENT_ARRIVAL         1
/* Event type for departure of a job from aparticular station. */	
#define EVENT_DEPARTURE       2	
/* Event type for end of the simulation. */
#define EVENT_END_SIMULATION  3	

/* Jobshop specification */
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

/* Variable*/
extern int 
	num_stations, 
	num_job_types, 
	i, 
	j, 
	num_machines[MAX_NUM_STATIONS + 1],
	num_tasks[MAX_NUM_JOB_TYPES + 1],
	route[MAX_NUM_JOB_TYPES + 1][MAX_NUM_STATIONS + 1], 
	num_machines_busy[MAX_NUM_STATIONS + 1], job_type, task;

extern double  
	mean_interarrival, 
	length_simulation, 
	prob_distrib_job_type[26],
	mean_service[MAX_NUM_JOB_TYPES + 1][MAX_NUM_STATIONS + 1];

extern FILE *infile, *outfile;

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
 */
void arrive (int new_job);

/**
 * @brief Departure job from particular station.
 * 
 */
void depart (void);

/**
 * @brief Print the simulation result.
 * 
 */
void report (void);

#endif