//
// File: test_ami_matrix.cpp
// Author: Darren Vengroff <darrenv@eecs.umich.edu>
// Created: 12/11/94
//

#include <tpie/portability.h>

#include "app_config.h"        
#include "parse_args.h"

// Define it all.
#include <tpie/scan.h>

// Utitlities for ascii output.
#include <tpie/scan_utils.h>

// Get some scanners.
#include "scan_count.h"

// Get matrices.
#include "matrix.h"

using namespace tpie;

static char def_crf[] = "osc.txt";
static char def_irf[] = "osi.txt";
static char def_frf[] = "osf.txt";

static char *count_results_filename = def_crf;
static char *intermediate_results_filename = def_irf;
static char *final_results_filename = def_frf;

static bool report_results_count = false;
static bool report_results_intermediate = false;
static bool report_results_final = false;

struct options app_opts[] = {
    { 10, "count-results-filename", "", "C", 1 },
    { 11, "report-results-count", "", "c", 0 },
    { 12, "intermediate-results-filename", "", "I", 1 },
    { 13, "report-results-intermediate", "", "i", 0 },
    { 14, "final-results-filename", "", "F", 1 },
    { 15, "report-results-final", "", "f", 0 },
    { 0, NULL, NULL, NULL, 0 }
};

void parse_app_opts(int idx, char *opt_arg) {
    switch (idx) {
    case 10:
	count_results_filename = opt_arg;
    case 11:
	report_results_count = true;
	break;
    case 12:
	intermediate_results_filename = opt_arg;
    case 13:
	report_results_intermediate = true;
	break;
    case 14:
	final_results_filename = opt_arg;
    case 15:
	report_results_final = true;
	break;
    }
}


int main(int argc, char **argv) {

    ami::err ae;

    test_size = 128 * 1024;

    parse_args(argc, argv, app_opts, parse_app_opts);

    if (verbose) {
	std::cout << "test_size = " << test_size << "." << std::endl;
	std::cout << "test_mm_size = " << static_cast<TPIE_OS_OUTPUT_SIZE_T>(test_mm_size) << "." << std::endl;
	std::cout << "random_seed = " << random_seed << "." << std::endl;
    } else {
        std::cout << test_size << ' ' << static_cast<TPIE_OS_OUTPUT_SIZE_T>(test_mm_size) << ' ' << random_seed;
    }
    
    // Set the amount of main memory:
    MM_manager.set_memory_limit (test_mm_size);

    apps::matrix<TPIE_OS_OFFSET> em0(test_size, test_size);
        
    // Streams for reporting values to ascii streams.
    
    std::ofstream *osc;
    std::ofstream *osi;
    std::ofstream *osf;
    ami::cxx_ostream_scan<TPIE_OS_OFFSET> *rptc = NULL;
    ami::cxx_ostream_scan<TPIE_OS_OFFSET> *rpti = NULL;
    ami::cxx_ostream_scan<TPIE_OS_OFFSET> *rptf = NULL;
    
    if (report_results_count) {
        osc = new std::ofstream(count_results_filename);
        rptc = new ami::cxx_ostream_scan<TPIE_OS_OFFSET>(osc);
    }
    
    if (report_results_intermediate) {
        osi = new std::ofstream(intermediate_results_filename);
        rpti = new ami::cxx_ostream_scan<TPIE_OS_OFFSET>(osi);
    }
    
    if (report_results_final) {
        osf = new std::ofstream(final_results_filename);
        rptf = new ami::cxx_ostream_scan<TPIE_OS_OFFSET>(osf);
    }
    
    // Write some ints.
    scan_count sc(test_size*test_size);

    ae = ami::scan(&sc, &em0);

    if (verbose) {
	std::cout << "Wrote the initial sequence of values." << std::endl;
        std::cout << "Stopped (didn't write) with ii = "
		  << sc.ii << ". operate() called " 
		  << sc.called << " times." << std::endl;
        std::cout << "Stream length = " << em0.stream_len() << std::endl;
    }

    if (report_results_count) {
        ae = ami::scan(&em0, rptc);
    }

    {
        // Pad the matrix.

	apps::matrix_pad<int> smp(test_size, test_size, 7);

	apps::matrix<int> em1(7 * ((em0.rows() - 1)/7 + 1),
			      7 * ((em0.cols() - 1)/7 + 1));

        ae = ami::scan(&em0, &smp, &em1);

        
        // Block permute the matrix.

	apps::matrix<int> em1p(7 * ((em0.rows() - 1)/7 + 1),
			       7 * ((em0.cols() - 1)/7 + 1));

	apps::perm_matrix_into_blocks pmib1(7 * ((em0.rows() - 1)/7 + 1),
					    7 * ((em0.cols() - 1)/7 + 1),
					    7);

        ae = ami::general_permute(&em1, &em1p, &pmib1); 

        if (report_results_intermediate) {
            ae = ami::scan(&em1p, rpti);
        }
        
        // Un block permute it.

	apps::matrix<int> em2(7 * ((em0.rows() - 1)/7 + 1),
			      7 * ((em0.cols() - 1)/7 + 1));

	apps::perm_matrix_outof_blocks pmob1(7 * ((em0.rows() - 1)/7 + 1),
					     7 * ((em0.cols() - 1)/7 + 1),
					     7);

        ae = ami::general_permute(&em1p, &em2, &pmob1); 
        
        // Unpad the matrix.

	apps::matrix_unpad<int> smup(test_size, test_size, 7);

	apps::matrix<int> em3(em0.rows(), em0.cols());

        ae = ami::scan(&em2, &smup, &em3);

        if (report_results_final) {
            ae = ami::scan(&em3, rptf);
        }

    }
    
    return 0;
}