// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; c-file-style: "stroustrup"; -*-
// vi:set ts=4 sts=4 sw=4 noet :
// Copyright 2011, The TPIE development team
// 
// This file is part of TPIE.
// 
// TPIE is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
// 
// TPIE is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with TPIE.  If not, see <http://www.gnu.org/licenses/>
#include "../app_config.h"

#include <cstdlib> // exit
#include <tpie/tpie.h>
#include <tpie/pipelining.h>
#include <iostream>
#include "testtime.h"
#include <boost/filesystem/operations.hpp>

using namespace tpie;
using namespace tpie::pipelining;
using namespace tpie::test;

const size_t count_default=1024*1024*1024/sizeof(uint64_t);

typedef uint64_t test_t;

static std::string prog;

static inline void usage() {
	std::cout << "Usage: " << prog << " [-N] [times [count]]\n"
		<< "-N: Don't use virtual calls\n"
		<< "times: Number of trials\n"
		<< "count: Number of elements in each trial"
		<< std::endl;
	exit(EXIT_FAILURE);
}

template <typename dest_t>
struct number_generator_t {
	typedef typename dest_t::item_type item_type;
	inline number_generator_t(const dest_t & dest, size_t count) : dest(dest), count(count) {}
	inline void operator()() {
		dest.begin();
		for (size_t i = 1; i <= count; ++i) {
			dest.push(i);
		}
		dest.end();
	}
private:
	dest_t dest;
	size_t count;
};

struct number_sink_t {
	typedef test_t item_type;
	inline number_sink_t(test_t & output) : output(output) {}
	inline void begin() { }
	inline void end() { }
	inline void push(const test_t & item) {
		output = output + item;
	}
private:
	test_t & output;
};

template <bool virt>
inline static void do_write(size_t count) {
	file_stream<test_t> s;
	s.open("tmp");
	if (virt) {
		pipeline p = pipe_begin<factory_1<number_generator_t, size_t> >(count) | output(s);
		p();
	} else {
		number_generator_t<output_t<test_t> > p =
			pipe_begin<factory_1<number_generator_t, size_t> >(count) | output(s);
		p();
	}
}

template <bool virt>
inline static test_t do_read() {
	test_t res = 0;
	file_stream<test_t> s;
	s.open("tmp");
	if (virt) {
		pipeline p = input(s) | pipe_end<termfactory_1<number_sink_t, test_t &> >(termfactory_1<number_sink_t, test_t &>(res));
		p();
	} else {
		input_t<number_sink_t> p = input(s) | pipe_end<termfactory_1<number_sink_t, test_t &> >(termfactory_1<number_sink_t, test_t &>(res));
		p();
	}
	return res;
}

template <bool virt>
static void test(size_t count) {
	test_realtime_t start;
	test_realtime_t end;

	boost::filesystem::remove("tmp");

	getTestRealtime(start);
	do_write<virt>(count);
	getTestRealtime(end);
	std::cout << testRealtimeDiff(start,end) << std::flush;
	
	getTestRealtime(start);
	test_t res = do_read<virt>();
	getTestRealtime(end);
	std::cout << " " << testRealtimeDiff(start,end) << ' ' << res << std::endl;

	boost::filesystem::remove("tmp");
}

int main(int argc, char **argv) {
	size_t times = 10;
	size_t count = count_default;
	bool virt = true;
	prog = argv[0];

	while (argc > 1) {
		std::string arg(argv[1]);

		if (arg == "-N")
			virt = false;

		else if (arg == "--help" || arg == "-h")
			usage();

		else break;

		--argc, ++argv;
	}

	if (argc > 1) {
		if (std::string(argv[1]) == "0") {
			times = 0;
		} else {
			std::stringstream(argv[1]) >> times;
			if (!times) usage();
		}
	}
	if (argc > 2) {
		std::stringstream(argv[2]) >> count;
		if (!count) usage();
	}

	std::cout << "Writing " << count << " items, reading them" << std::endl;

	tpie::tpie_init();

	for (size_t i = 0; i < times || !times; ++i) {
		if (virt)
			::test<true>(count);
		else
			::test<false>(count);
	}
	
	tpie::tpie_finish();

	return EXIT_SUCCESS;
}
