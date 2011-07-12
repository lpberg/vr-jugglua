#!/bin/bash
(
cd $(dirname "${0}")
GENERATION_DIR="include/osgLua/Introspection"
HEADER_LIST_FILE="${GENERATION_DIR}/headers.cmake"
mkdir -p ${GENERATION_DIR}
cat > "${HEADER_LIST_FILE}" <<EOS
# Header paths relative to the source directory
set(OSGLUA_INTROSPECTION_HEADERS
EOS


for INCLUDE in $1/*; do
	INCLUDENAME=$(basename "${INCLUDE}")
	INCLUDEGUARD="OSGLUA_INTROSPECTION_${INCLUDENAME}"
	cat > "${GENERATION_DIR}/${INCLUDENAME}" <<EOS
/*
	osgLua: use Lua to access dynamically to osg using osgIntrospection
	Copyright(C) 2006 Jose L. Hidalgo Valiño (PpluX) (pplux at pplux.com)
	Copyright(C) 2010-2011 Iowa State University (Author: Ryan Pavlik <rpavlik@acm.org> )

    This library is open source and may be redistributed and/or modified under
    the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
    (at your option) any later version.  The full license is in LICENSE file
    included with this distribution, and on the openscenegraph.org website.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    OpenSceneGraph Public License for more details.
*/
#pragma once
#ifndef ${INCLUDEGUARD}
#define ${INCLUDEGUARD}
#include <osgLua/Config>

#ifdef OSGLUA_USE_CPPINTROSPECTION
#	include <cppintrospection/${INCLUDENAME}>
namespace osgLua {
	namespace introspection {
		using namespace cppintrospection;
	} // end of namespace introspection
} // end of namespace osgLua

#else
#	include <osgIntrospection/${INCLUDENAME}>
namespace osgLua {
	namespace introspection {
		using namespace osgIntrospection;
	} // end of namespace introspection
} // end of namespace osgLua

#endif // ${INCLUDEGUARD}
EOS
	echo "	../../${GENERATION_DIR}/${INCLUDENAME}" >> "${HEADER_LIST_FILE}"
done
echo ")" >> "${HEADER_LIST_FILE}"
)
