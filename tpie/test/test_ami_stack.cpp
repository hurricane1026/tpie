// Copyright (c) 1994 Darren Vengroff
//
// File: test_ami_stack.cpp
// Author: Darren Vengroff <darrenv@eecs.umich.edu>
// Created: 12/15/94
//

static char test_ami_stack_id[] = "$Id: test_ami_stack.cpp,v 1.1 1994-12-16 21:21:48 darrenv Exp $";

// This is just to avoid an error message since the string above is never
// referenced.  Note that a self referential structure must be defined to
// avoid passing the problem further.
static struct ___test_ami_stack_id_compiler_fooler {
    char *pc;
    ___test_ami_stack_id_compiler_fooler *next;
} the___test_ami_stack_id_compiler_fooler = {
    test_ami_stack_id,
    &the___test_ami_stack_id_compiler_fooler
};


#include <iostream.h>
#include <fstream.h>

#include "app_config.h"        
#include "parse_args.h"

// Define it all.
#include <ami.h>

// Utitlities for ascii output.
#include <ami_scan_utils.h>

// Get stacks

#include <ami_stack.h>


static char def_crf[] = "/var/tmp/osc.txt";
static char def_irf[] = "/var/tmp/osi.txt";
static char def_frf[] = "/var/tmp/osf.txt";

static char *count_results_filename = def_crf;
static char *intermediate_results_filename = def_irf;
static char *final_results_filename = def_frf;

static bool report_results_count = false;
static bool report_results_intermediate = false;
static bool report_results_final = false;

static const char as_opts[] = "C:I:F:cif";
void parse_app_opt(char c, char *optarg)
{
    switch (c) {
        case 'C':
            count_results_filename = optarg;
        case 'c':
            report_results_count = true;
            break;
        case 'I':
            intermediate_results_filename = optarg;
        case 'i':
            report_results_intermediate = true;
            break;
        case 'F':
            final_results_filename = optarg;
        case 'f':
            report_results_final = true;
            break;
    }
}

extern int register_new;

int main(int argc, char **argv)
{
    AMI_err ae;

    parse_args(argc,argv,as_opts,parse_app_opt);

    if (verbose) {
        cout << "test_size = " << test_size << ".\n";
        cout << "test_mm_size = " << test_mm_size << ".\n";
        cout << "random_seed = " << random_seed << ".\n";
    } else {
        cout << test_size << ' ' << test_mm_size << ' ' << random_seed;
    }
    
    // Set the amount of main memory:
    MM_manager.resize_heap(test_mm_size);
    register_new = 1;

    AMI_stack<double> amis0;

    // Streams for reporting values to ascii streams.
    
    ofstream *osc;
    ofstream *osi;
    ofstream *osf;
    cxx_ostream_scan<double> *rptc = NULL;
    cxx_ostream_scan<double> *rpti = NULL;
    cxx_ostream_scan<double> *rptf = NULL;
    
    if (report_results_count) {
        osc = new ofstream(count_results_filename);
        rptc = new cxx_ostream_scan<double>(osc);
    }
    
    if (report_results_intermediate) {
        osi = new ofstream(intermediate_results_filename);
        rpti = new cxx_ostream_scan<double>(osi);
    }
    
    if (report_results_final) {
        osf = new ofstream(final_results_filename);
        rptf = new cxx_ostream_scan<double>(osf);
    }
    
    // Push values.
    
    for (double ii = test_size; ii--; ) {
        amis0.push(ii+0.01);
    }

    if (verbose) {
        cout << "Pushed the initial sequence of values.\n";
        cout << "Stream length = " << amis0.stream_len() << '\n';
    }
        
    if (report_results_count) {
        ae = AMI_scan((AMI_base_stream<double> *)&amis0, rptc);
    }

    // Pop them all off.

    double *jj;
    
    for (ii = 0.0; ii < test_size; ii++ ) {
        amis0.pop(&jj);
        if (ii  + 0.01 != *jj) {
            cout << ii  + 0.01 << " != " << *jj << '\n';
        }
    }

    if (verbose) {
        cout << "Popped the initial sequence of values.\n";
        cout << "Stream length = " << amis0.stream_len() << '\n';
    }
    
    return 0;
}

#ifdef NO_IMPLICIT_TEMPLATES

// Instantiate templates for streams of objects.
TEMPLATE_INSTANTIATE_STREAMS(double)

// Instantiate templates for I/O using C++ streams.
TEMPLATE_INSTANTIATE_OSTREAM(double)

// Instantiate templates for stacks.
TEMPLATE_INSTANTIATE_STACK(double)

#endif

