/**
 * @file utils.h
 * @brief Public facade for the BitGraph utility library.
 *
 * Includes the general-purpose utility types and functions intended for
 * consumer code. Internal implementation headers from the detail directory
 * are deliberately not exposed through this facade.
 *
 * Consumers may include individual utility headers to reduce compilation
 * dependencies or include this file for convenient access to the complete
 * public utility API.
 *
 * @author Pablo San Segundo
 * @date Last updated: 26/09/2026
 */

#ifndef BITGRAPH_UTILS_UTILS_H
#define BITGRAPH_UTILS_UTILS_H

#include "utils/batch.h"
#include "utils/collection_utils.h"
#include "utils/file_utils.h"
#include "utils/fixed_stack.h"
#include "utils/math_utils.h"
#include "utils/path_utils.h"
#include "utils/prec_timer.h"
#include "utils/random_utils.h"
#include "utils/sort_utils.h"
#include "utils/string_utils.h"
#include "utils/time_utils.h"


 // #include "utils/logger.h"			has to be set explicitly, sice it has MACROS 

#endif // BITGRAPH_UTILS_UTILS_H