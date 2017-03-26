//
// Copyright (C) 2016-17 by the krims authors
//
// This file is part of krims.
//
// krims is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// krims is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with krims. If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

// Include this header to get the krims exception system.

// Note: This header should always be the first, since it initialises
// the exception system in case the user wants this so
// (via defining KRIMS_INIT_EXCEPTION_SYSTEM)
#include "ExceptionSystem/ExceptionSystem.hh"

#include "ExceptionSystem/Exceptions.hh"
#include "ExceptionSystem/exception_helper_macros.hh"
