#pragma once

#ifdef _WIN32
#include "targetver.h"
#endif

#include <tuple>
#include <functional>
#include <memory>
#include <cassert>
#include <fstream>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <set>
#include <map>
#include <iterator>
#include <regex>
#include <thread>
#include <chrono>

#if defined TEST_MODE
#include <gtest/gtest.h>
#endif

#include <opencv2/opencv.hpp>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>
