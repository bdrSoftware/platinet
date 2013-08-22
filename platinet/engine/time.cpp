/*
 * Copyright 2011, 2012, 2013 salvatore barone <salvator.barone@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "time.hpp"
#include <ctime>

using namespace engine;
time::time () {
	std::time_t tempo_unix = std::time(NULL);
	struct std::tm* broken_down_time = std::localtime(&tempo_unix);

	__seconds = broken_down_time -> tm_sec;
	__minutes = broken_down_time -> tm_min;
	__hours = broken_down_time -> tm_hour;
	__day = broken_down_time -> tm_mday;
	__month = broken_down_time -> tm_mon + 1;
	__year = broken_down_time -> tm_year + 1900;
}

std::string time::clock (char sep) const {
	std::string _sep = std::string(1, sep);
	std::string clk = std::to_string(__hours) + _sep + std::to_string(__minutes) + _sep + std::to_string(__seconds);
	return clk;
}

std::string time::date (char sep) const {
	std::string _sep = std::string(1, sep);
	std::string _date = std::to_string(__day) + _sep + std::to_string(__month) + _sep + std::to_string(__year);
	return _date;
}

std::string time::timeStamp (char d_sep, char t_sep) const {
	std::string _d_sep = std::string(1, d_sep);
	std::string _t_sep = std::string(1, t_sep);
	std::string timestamp = date(d_sep) + " " + clock(t_sep);
	return timestamp;
}
