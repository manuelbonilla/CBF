/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#include <cbf/plugin_pool.h>
#include <cbf/controller.h>

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include <schemas.hxx>

#include <boost/program_options.hpp>

namespace po = boost::program_options;


int main(int argc, char *argv[]) {
	po::options_description options_description("Allowed options");
	options_description.add_options() 
		("help", "produce help message")
		("sleep-time", po::value<unsigned int>(), "time to sleep between cycles in microseconds")
		("steps", po::value<unsigned int>(), "run exact number of steps")
		("recipe", po::value<std::vector<std::string > >(), "XML file containing controller recipe (can be used more than once)")
		;

	po::variables_map variables_map;

	po::store(
		po::parse_command_line(
			argc, 
			argv, 
			options_description
		), 
		variables_map
	);

	po::notify(variables_map);

	if (variables_map.count("help")) {
		std::cout << options_description << std::endl;
		exit(EXIT_SUCCESS);
	}

	//! sleep time in microseconds
	unsigned int sleep_time = 0;

	if (variables_map.count("sleep-time"))
		sleep_time = variables_map["sleep-time"].as<int>();

	std::vector<std::string> recipes;

	if (!variables_map.count("recipe")) {
		std::cout << "No recipes specified" << std::endl;
		std::cout << options_description << std::endl;
		exit(EXIT_FAILURE);
	}

	recipes = variables_map["recipe"].as<std::vector<std::string> >();

	for (unsigned int i = 0; i < recipes.size(); ++i) {
		std::string filename(recipes[i]);

		CBF::PluginPool<CBF::Controller> *pp = 
			CBF::PluginPool<CBF::Controller>::get_instance();

		CBF::ControllerPtr c = 
			pp->create_from_file<ControllerType>(filename);

		if (variables_map.count("steps")) 
			for (unsigned int step = 0, steps = variables_map["steps"].as<unsigned int>(); step < steps; ++step)
				{ c->step(); usleep(sleep_time); }
		else
			while (c->step() == false) {
				usleep(sleep_time);
			}
	}

	return 0;
}

