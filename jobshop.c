/* External definitions for job-shop model. */

/* Required for use of simlib.c. */
#include "simlib.h"		

/* Header for jobshop.c*/
#include "jobshop.h"

/* Declare non-simlib global variables. */

/* Number of station */
int num_stations; 
/* Number of job types */
int num_job_types; 
/* Loop index */
int i;
/* Loop index */
int j; 
/* Number of machines in each station */
int num_machines[MAX_NUM_STATIONS + 1];
/* Number of tasks in each job type */
int num_tasks[MAX_NUM_JOB_TYPES + 1];
/* Route matrix */
int route[MAX_NUM_JOB_TYPES + 1][MAX_NUM_STATIONS + 1]; 
/* Number of used machine in each station */
int num_machines_busy[NUM_JOBSHOP+1][MAX_NUM_STATIONS + 1]; 
/* Type of the job*/
int job_type;
/* Task of the job (one job can have multiple task)*/
int task;
/*Current jobshop number*/
int jobshop_number;
/* Previous JobShop Number */
int previous_jobshop_number;
/* Previous Job Type*/
int previous_job_type = -1;

/* Mean interarrival time of each job */
double mean_interarrival;
/* Length of simulation (in hour)*/
double length_simulation; 
/* Probability distribution for each job type */
double prob_distrib_job_type[26];
/* Mean service time for each job type and station */
double mean_service[MAX_NUM_JOB_TYPES + 1][MAX_NUM_STATIONS + 1];
/*Probability distribution of arriving in JobShop 1 or 2*/
/*The probability is equal*/
double prob_distrib_job_shop[3] = {0, 0.5, 1.0};

/* File to read input */
FILE *infile;
/* File to write output */
FILE *outfile;


void arrive (int new_job, int arrival_jopshop_number) {
	/* Function to serve as both an arrival event of a job to the system, as well as the non-event 
	   of a job's arriving to a subsequent station along its route. */
	
	int station, station_queue;

	/* If this is a new arrival to the system, generate the time of the next arrival and determine 
		 the job type and task number of the arriving job. */
	if (new_job == 1) {
		jobshop_number = random_integer (prob_distrib_job_shop, STREAM_JOBSHOP_ARRIVAL);
		event_schedule (sim_time + expon (mean_interarrival, STREAM_INTERARRIVAL), 
			getArrivalJobShop(jobshop_number));
		// TODO: Remove old code.
		// event_schedule (sim_time + expon (mean_interarrival, STREAM_INTERARRIVAL), EVENT_ARRIVAL);
		job_type = random_integer (prob_distrib_job_type, STREAM_JOB_TYPE);
		task = 1;
	}

	if (arrival_jopshop_number == 3 && previous_job_type != -1) {
		job_type = previous_job_type;
		task = 1;
	}


	/* Determine the station from the route matrix. */
	station = route[job_type][task];
	station_queue = getListQueue(arrival_jopshop_number, station);
	
	/* LOG to file if the person just arrived in current jobshop */
	if (task == 1) {
		if (arrival_jopshop_number == 3) {
			fprintf (outfile, "[ %.2lf ] A Person just arrived in Jobshop %d, the job type are %d, previously from JobShop %d \n", 
				sim_time, arrival_jopshop_number, job_type, previous_jobshop_number);	
				previous_job_type = -1;
		} else {
			fprintf (outfile, "[ %.2lf ] A Person just arrived in Jobshop %d, the job type are %d \n", 
				sim_time, arrival_jopshop_number, job_type);
		}
		
	}
	

	/* Check to see whether all machines in this station are busy. */
	if (num_machines_busy[arrival_jopshop_number][station] == num_machines[station]) {
		/* All machines in this station are busy, so place the arriving job at the end of the 
		   appropriate queue. Note that the following data are stored in the record for each job:
		 	 1. Time of arrival to this station.
			 2. Job type.
			 3. Current task number. */
		
		transfer[1] = sim_time;
		transfer[2] = job_type;
		transfer[3] = task;
		list_file (LAST, station_queue);
		
		fprintf (outfile, "[ %.2lf ] The machine in station %d, in jobshop %d are busy, the job type is %d. Adding person in queue %d (%d person in queue) \n", 
			sim_time, station, arrival_jopshop_number, job_type, station_queue, list_size[station_queue]);

	} else {
		/* A machine in this station is idle, so start service on the arriving
			 job (which has a delay of zero). */

		// TODO: Remove old code and evaluate our new code (sampst and timest).
		// sampst (0.0, station);	/* For station. */
		// sampst (0.0, num_stations + job_type);	/* For job type. */
		// timest ((double) num_machines_busy[station], station);

		sampst (0.0, station_queue);	/* For station. */
		sampst (0.0, 3 * num_stations + job_type);	/* For overall job type. */
		sampst (0.0, 3 * (num_stations + arrival_jopshop_number) + job_type);	/* For job type of individual jobshop. */
		++num_machines_busy[arrival_jopshop_number][station];
		timest ((double) num_machines_busy[arrival_jopshop_number][station], station_queue);

		/* Schedule a service completion.  Note defining attributes beyond the
			first two for the event record before invoking event_schedule. */

		transfer[3] = job_type;
		transfer[4] = task;
		event_schedule (sim_time + erlang (2, mean_service[job_type][task], 
			STREAM_SERVICE), getDepartureJobShop(arrival_jopshop_number));
	}
}

void depart (int departure_jobshop_number) {
	/* Event function for departure of a job from a particular station. */
	int station, job_type_queue, task_queue, station_queue;

	/* Determine the station from which the job is departing. */
	job_type = transfer[3];
	task = transfer[4];
	station = route[job_type][task];
	station_queue = getListQueue(departure_jobshop_number, station);

	/* Check to see whether the queue for this station is empty. */
	if (list_size[station_queue] == 0) {
		/* The queue for this station is empty, so make a machine in this
			 station idle. */
		--num_machines_busy[departure_jobshop_number][station];
		// TODO: Remove old code and evaluate our new code (timest).
		// timest ((double) num_machines_busy[station], station);
		timest((double) num_machines_busy[departure_jobshop_number][station], station_queue);

	} else {
		/* The queue is nonempty, so start service on first job in queue. */
		list_remove (FIRST, station_queue);

		fprintf (outfile, "[ %.2lf ] Process person from queue %d. Resuming job in jobshop %d station %d (%d person in queue) \n", 
			sim_time, station_queue, departure_jobshop_number, station, list_size[station_queue]);

		// TODO: Remove old code and evaluate our new code (sampst).
		/* Tally this delay for this station. */
		// sampst (sim_time - transfer[1], station);
		sampst(sim_time - transfer[1], station_queue);

		/* Tally this same delay for this job type. */
		job_type_queue = transfer[2];
		task_queue = transfer[3];
		sampst (sim_time - transfer[1], 3 * num_stations + job_type_queue);
		sampst (sim_time - transfer[1], 3 * (num_stations + departure_jobshop_number)+ job_type_queue);

		/* Schedule end of service for this job at this station.  Note defining attributes beyond 
				the first two for the event record before invoking event_schedule. */
		transfer[3] = job_type_queue;
		transfer[4] = task_queue;
		event_schedule (sim_time + erlang (2, mean_service[job_type_queue][task_queue], 
			STREAM_SERVICE), getDepartureJobShop(departure_jobshop_number));
	}

	/* If the current departing job has one or more tasks yet to be done, send the job to the next 
		 station on its route. */
	if (task < num_tasks[job_type]) {
		++task;
		arrive (0, departure_jobshop_number);
	} else if (task == num_tasks[job_type]) {
		/* Determine if the person will depart from current JobShop */
		fprintf (outfile, "[ %.2lf ] A Person just departed from Jobshop %d, the job type are %d \n", 
		sim_time, departure_jobshop_number, job_type);
		/* If the current person has no more task and not from jobshop 3 then assign it to jobshop 3 */
		if (departure_jobshop_number != 3) {
			// int *temp;
			// temp = &previous_job_type;
			// *temp = job_type;
			previous_job_type = job_type;
			previous_jobshop_number = departure_jobshop_number;
			event_schedule (sim_time, getArrivalJobShop(3));
		}
	}
	
}


void report (void) {
	// TODO: Check the correctness of this function.
	/* Report generator function. */
	
	int station_queue;
	double overall_avg_job_tot_delay, avg_job_tot_delay, sum_probs;

	/* Compute the average total delay in queue for each job type and the overall average job total 
		delay. */
	fprintf (outfile, "\n\n\nOverall statistics for jobshop system");
	fprintf (outfile, "\nJob type     Average total delay in queue");
	overall_avg_job_tot_delay = 0.0;
	sum_probs = 0.0;
	for (i = 1; i <= num_job_types; ++i) {
		avg_job_tot_delay = sampst (0.0, -(3 * num_stations + i)) * num_tasks[i];
		fprintf (outfile, "\n\n%4d%27.3f", i, avg_job_tot_delay);
		overall_avg_job_tot_delay += (prob_distrib_job_type[i] - sum_probs) * avg_job_tot_delay;
		sum_probs = prob_distrib_job_type[i];
	}

	fprintf (outfile, "\n\nOverall average job total delay =%10.3f", overall_avg_job_tot_delay);

	for (i = 1; i <= 3; ++i) {
		/* Compute the average number in queue, the average utilization, and the
			average delay in queue for each station for each jobshop. */
		fprintf (outfile, "\n\n\nStatistics for jobshop %d", i);
		fprintf (outfile, "\n\nJob type     Average total delay in queue");
		
		overall_avg_job_tot_delay = 0.0;
		sum_probs = 0.0;
		for (j = 1; j <= num_job_types; ++j) {
			avg_job_tot_delay = sampst (0.0, -(3 * (num_stations + i) + j)) * num_tasks[j];
			fprintf (outfile, "\n\n%4d%27.3f", j, avg_job_tot_delay);
			overall_avg_job_tot_delay += (prob_distrib_job_type[j] - sum_probs) * avg_job_tot_delay;
			sum_probs = prob_distrib_job_type[j];
		}

		fprintf (outfile, "\n\nOverall average job total delay =%10.3f", overall_avg_job_tot_delay);

		fprintf (outfile, "\n\n Work      Average number      Average       Average delay");
		fprintf (outfile, "\nstation       in queue       utilization        in queue");
		for (j = 1; j <= num_stations; ++j) {
			station_queue = getListQueue(i, j);
			fprintf (outfile, "\n\n%4d%17.3f%17.3f%17.3f", j, filest (station_queue), 
				timest (0.0, -station_queue) / num_machines[j], sampst (0.0, -station_queue));
		}
	}
		
}


void readJobShopParameter(void) {
	/* Open input and output files. */
	infile = fopen ("jobshop.in", "r");
	outfile = fopen ("jobshop.out", "w");

	/* Read input parameters. */
	/* Read basic data, such as number of station, jobtype, mean interarival and length simulation*/
	fscanf (infile, "%d %d %lg %lg", &num_stations, &num_job_types, &mean_interarrival, &length_simulation);
	
	/* Read the number of machine in each station. */
	for (j = 1; j <= num_stations; ++j){
		fscanf (infile, "%d", &num_machines[j]);
	}
	
	/* Read the number of tasks to be done in each job type. */
	for (i = 1; i <= num_job_types; ++i){
		fscanf (infile, "%d", &num_tasks[i]);
	}

	/* Foreach job type, read it's job task attribute */
	for (i = 1; i <= num_job_types; ++i) {
		/* Read the job station */
		for (j = 1; j <= num_tasks[i]; ++j) {
			fscanf (infile, "%d", &route[i][j]);
		}
		/* Read the job service time for each station */
		for (j = 1; j <= num_tasks[i]; ++j) {
			fscanf (infile, "%lg", &mean_service[i][j]);
		}
	}

	/* Read the probability distribution of job type. */
	for (i = 1; i <= num_job_types; ++i) {
		fscanf (infile, "%lg", &prob_distrib_job_type[i]);
	}
		

	/* Write report heading and input parameters. */
	fprintf (outfile, "Job-shop model\n\n");
	fprintf (outfile, "Number of work stations%21d\n\n", num_stations);
	fprintf (outfile, "Number of machines in each station     ");
	for (j = 1; j <= num_stations; ++j) {
		fprintf (outfile, "%5d", num_machines[j]);
	}
	fprintf (outfile, "\n\nNumber of job types%25d\n\n", num_job_types);
	fprintf (outfile, "Number of tasks for each job type      ");
	for (i = 1; i <= num_job_types; ++i) {
		fprintf (outfile, "%5d", num_tasks[i]);
	}
	fprintf (outfile, "\n\nDistribution function of job types  ");
	for (i = 1; i <= num_job_types; ++i) {
		fprintf (outfile, "%8.3f", prob_distrib_job_type[i]);
	}
	fprintf (outfile, "\n\nMean interarrival time of jobs%14.2f hours\n\n", mean_interarrival);
	fprintf (outfile, "Length of the simulation%20.1f eight-hour days\n\n\n", length_simulation);
	fprintf (outfile, "Job type     Work stations on route");
	for (i = 1; i <= num_job_types; ++i) {
	fprintf (outfile, "\n\n%4d        ", i);
		for (j = 1; j <= num_tasks[i]; ++j) {
			fprintf (outfile, "%5d", route[i][j]);
		}
	}
	fprintf (outfile, "\n\n\nJob type     ");
	fprintf (outfile, "Mean service time (in hours) for successive tasks");
	for (i = 1; i <= num_job_types; ++i){
		fprintf (outfile, "\n\n%4d    ", i);
		for (j = 1; j <= num_tasks[i]; ++j) {
			fprintf (outfile, "%9.2f", mean_service[i][j]);
		}
	}
	fprintf (outfile, "\n\nLOG \n");
}


void init () {
	/* Initiate jobshop problem. */
	printf("Running Job Shop schedulling\n");

	/* Initialize all machines in all stations to the idle state. */
	for (i=1; i <= NUM_JOBSHOP; i++) {
		for (j = 1; j <= num_stations; ++j) {
			num_machines_busy[i][j] = 0;
		}
	}

	/* Initialize simlib */
	init_simlib ();

	/* Set maxatr = max(maximum number of attributes per record, 4) */
	maxatr = 4;			/* NEVER SET maxatr TO BE SMALLER THAN 4. */

	/* Schedule first arrival with probability 50:50 for jobshop 1 and 2*/
	jobshop_number = random_integer (prob_distrib_job_shop, STREAM_JOBSHOP_ARRIVAL);
	event_schedule (expon (mean_interarrival, STREAM_INTERARRIVAL), getArrivalJobShop(jobshop_number));

	// TODO: Remove old code.
	// /* Schedule the arrival of the first job. */
	// event_schedule (expon (mean_interarrival, STREAM_INTERARRIVAL), EVENT_ARRIVAL);

	/* Schedule the end of the simulation.  (This is needed for consistency of
		 units.) */
	event_schedule (8 * length_simulation, EVENT_END_SIMULATION);
}

int main ()	{
	/* Main function. */
	
	/* Read JobShop specification from the input file. */
	readJobShopParameter();

	/* Initialize the jobshop problem. */
	init();

	/* Run the simulation until it terminates after an end-simulation event
		 (type EVENT_END_SIMULATION) occurs. */
	do {
		/* Determine the next event. */
		timing ();

		/* Invoke the appropriate event function. */
		switch (next_event_type) {
			// TODO: Remove old code
			// case EVENT_ARRIVAL:
			// 	fprintf (outfile, "[ %.2lf ] A Person just arrived \n", sim_time);
			// 	arrive (1);
			// 	break;
			// case EVENT_DEPARTURE:
			// 	fprintf (outfile, "[ %.2lf ] A Person just departed \n", sim_time);
			// 	depart ();
			// 	break;
			case EVENT_ARRIVAL_JOBSHOP_1:
				arrive(1, 1);
				break;
			case EVENT_ARRIVAL_JOBSHOP_2:
				arrive(1, 2);
				break;
			case EVENT_ARRIVAL_JOBSHOP_3:
				arrive(0, 3);
				break;
			case EVENT_DEPARTURE_JOBSHOP_1:
				depart(1);
				break;
			case EVENT_DEPARTURE_JOBSHOP_2:
				depart(2);
				break;
			case EVENT_DEPARTURE_JOBSHOP_3:
				depart(3);
				break;
			case EVENT_END_SIMULATION:
				fprintf (outfile, "[ %.2lf ] Simulation ended \n", sim_time);
				report ();
				break;
		}
		/* If the event just executed was not the end-simulation event (type EVENT_END_SIMULATION), 
			 continue simulating.  Otherwise, end the simulation. */
	} while (next_event_type != EVENT_END_SIMULATION);

	fclose (infile);
	fclose (outfile);

	return 0;
}
