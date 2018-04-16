/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *         Zhenkai Zhu <zhenkai@cs.ucla.edu>
 */

#ifndef LOGGING4_H
#define LOGGING4_H


#include <log4cxx/logger.h>
#include "bnlogif.h"

#define MEMBER_LOGGER                           \
  static log4cxx::LoggerPtr staticModuleLogger;

#define INIT_MEMBER_LOGGER(className,name)          \
  log4cxx::LoggerPtr className::staticModuleLogger =  log4cxx::Logger::getLogger (name);

#define INIT_LOGGER(name) \
  static log4cxx::LoggerPtr staticModuleLogger = log4cxx::Logger::getLogger (name);

#define _LOG_DEBUG(x) \
  LOG4CXX_DEBUG(staticModuleLogger, x);

#define _LOG_TRACE(x) \
  LOG4CXX_TRACE(staticModuleLogger, x);

#define _LOG_FUNCTION(x) \
  LOG4CXX_TRACE(staticModuleLogger, __FUNCTION__ << "(" << x << ")");

#define _LOG_FUNCTION_NOARGS \
  LOG4CXX_TRACE(staticModuleLogger, __FUNCTION__ << "()");

#define _LOG_INFO(x) \
  LOG4CXX_INFO(staticModuleLogger, x);

#define _LOG_WARN(x) \
  LOG4CXX_WARN(staticModuleLogger, x);

#define _LOG_ERROR(x) \
  LOG4CXX_ERROR(staticModuleLogger, x);

#define _LOG_ERROR_COND(cond,x) \
  if (cond) { _LOG_ERROR(x) }

#define _LOG_DEBUG_COND(cond,x) \
  if (cond) { _LOG_DEBUG(x) }

#define _LOG_FATAL(x) \
  LOG4CXX_FATAL(staticModuleLogger, x);

void
INIT_LOGGERS ();

#endif // LOGGING4_H
