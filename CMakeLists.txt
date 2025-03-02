# Copyright (C) 2025 Smirnov Vladimir / mapron1@gmail.com
# SPDX-License-Identifier: CC0-1.0
# See LICENSE file for details.

cmake_minimum_required(VERSION 3.19)
project(ContestChecker)
set(CMAKE_CXX_STANDARD 20)

include(cmake/config.cmake)

add_executable(ContestChecker
	src/main.cpp
	src/CommandLine.cpp
	src/CommandLine.h
	src/CommonProblemTypes.h
	src/CommonTestUtils.h
	src/CustomAlloc.h
	src/PerformanceCounter.h
	src/PerformanceCounter.cpp
)
option(ENABLE_NEW_DELETE_HOOK "Enable replacement for new() and delete()" ON)
if (ENABLE_NEW_DELETE_HOOK)
	target_sources(ContestChecker PRIVATE src/CustomAlloc.cpp)
else()
	target_sources(ContestChecker PRIVATE src/DefaultAlloc.cpp)
endif()

cmake_policy(SET CMP0167 OLD)
find_package(Boost)

target_include_directories(ContestChecker PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/src)

if (Boost_INCLUDE_DIR)
	target_include_directories(ContestChecker PRIVATE ${Boost_INCLUDE_DIR})
endif()

set(generatedInit ${CMAKE_CURRENT_BINARY_DIR}/GeneratedInit)
file(MAKE_DIRECTORY ${generatedInit})

# extract all ProblemFolders
file(GLOB problemDirs Problems/*)
set(allEnabledProblems)

foreach(problemPath ${problemDirs})
	if(NOT IS_DIRECTORY "${problemPath}")
		message(FATAL_ERROR "You are supposed to have only directories in Problems/")
	endif()
	get_filename_component(problemName "${problemPath}" NAME)
	
	set(optionName "DISABLE_PROBLEM_${problemName}")
	option(${optionName} "Exclude '${problemName}' from build.")
	if (${optionName})
		message("Skipping '${problemName}' from build.")
		continue()
	endif()
	list(APPEND allEnabledProblems "${problemName}")
	file(GLOB problemHeaders "${problemPath}/*.h" "${problemPath}/*.hpp")
	file(GLOB problemTestHeaders "${problemPath}/*_tests.h" "${problemPath}/*_tests.hpp")
	list(FILTER problemHeaders EXCLUDE REGEX ".*_tests.*")
	list(LENGTH problemHeaders problemHeadersSize)
	list(LENGTH problemTestHeaders problemTestHeadersSize)
	if ((problemHeadersSize GREATER 1) OR (problemTestHeadersSize GREATER 1))
		message(FATAL_ERROR "Currently having more than 1 header is not supported.\n problemHeaders=${problemHeaders}, \n problemTestHeaders=${problemTestHeaders}")
	endif()
	
	file(GLOB problemDocs "${problemPath}/Problem*.txt")
	file(GLOB problemTestFiles "${problemPath}/input_*.txt" "${problemPath}/output_*.txt")
	file(GLOB problemAnyFile "${problemPath}/**")
	set(allKnownFiles ${problemHeaders} ${problemTestHeaders} ${problemSources} ${problemDocs} ${problemTestFiles})
	foreach(someFile ${problemAnyFile})
		if (NOT (someFile IN_LIST allKnownFiles))
			message(FATAL_ERROR "File ${someFile} has unknown extension, please remove it.")
		endif()
	endforeach()
	
	set(generatedCpp          ${generatedInit}/ProblemInit_${problemName}.cpp)	
	set(generatedCppCodeTests ${generatedInit}/ProblemCodeTestsInit_${problemName}.cpp)
	set(generatedCppFileTests ${generatedInit}/ProblemFileTestsInit_${problemName}.cpp)
	configure_file(cmake/ProblemInit.cpp.in ${generatedCpp} @ONLY)
	list(APPEND allKnownFiles ${generatedCpp})
	
	if (problemTestHeaders)
		configure_file(cmake/ProblemCodeTestsInit.cpp.in ${generatedCppCodeTests} @ONLY)
		list(APPEND allKnownFiles ${generatedCppCodeTests})
	endif()
	
	if (problemTestFiles)
		list(LENGTH problemTestFiles caseCount)
		math(EXPR caseCount "${caseCount} / 2")
		configure_file(cmake/ProblemFileTestsInit.cpp.in ${generatedCppFileTests} @ONLY)
		list(APPEND allKnownFiles ${generatedCppFileTests})
	endif()
	set_source_files_properties(${allKnownFiles} PROPERTIES COMPILE_FLAGS -DPROBLEM_NAMESPACE=${problemName}Details)
	
	target_sources(ContestChecker PRIVATE ${allKnownFiles})
endforeach()

function(checkForAnonymousNamespace filename)
	file(STRINGS "${filename}" lines)
	set(preprocessorStarted false)
	foreach(line IN LISTS lines)
		string(STRIP "${line}" line)
		if ("${line}" STREQUAL "")
			continue()
		endif()
		if ("${line}" MATCHES "^[#]")
			set(preprocessorStarted true)
			continue()
		endif()
		if (NOT preprocessorStarted)
			continue()
		endif()
		if ("${line}" MATCHES "^[/]")
			continue()
		endif()
		if ("${line}" MATCHES "^namespace {")
			return()
		endif()
		message(SEND_ERROR "File \n ${filename} \n"
			"must start with 'namespace {' after preprocesor, but it starts with:\n ${line}")
		return()
	endforeach()
endfunction()

set(allSolutionIds)
file(GLOB_RECURSE solutionFiles Solutions/Solution**.h Solutions/Solution**.hpp)
foreach(solutionPath ${solutionFiles})
	get_filename_component(basename "${solutionPath}" NAME_WE )
	string(REPLACE "Solution" "" basename "${basename}")
	string(REPLACE "_" ";" basenameParts "${basename}")
	if (basename MATCHES "[a-zA-Z0-9]+_[a-zA-Z0-9]+_[a-zA-Z0-9]+")
		list(GET basenameParts 0 problemName)
		list(GET basenameParts 1 authorName)
		list(GET basenameParts 2 implName)
	elseif(basename MATCHES "[a-zA-Z0-9]+_[a-zA-Z0-9]+")
		list(GET basenameParts 0 problemName)
		list(GET basenameParts 1 implName)
		set(authorName "${defaultAuthorName}")
	elseif(basename MATCHES "[a-zA-Z0-9]+")
		list(GET basenameParts 0 problemName)
		set(implName "${defaultImplName}")
		set(authorName "${defaultAuthorName}")
	else()
		continue()
	endif()
	set(fullId "${problemName}_${authorName}_${implName}")
	if (fullId IN_LIST allSolutionIds)
		message(FATAL_ERROR "Duplicate solution id ${fullId}")
	endif()
	list(APPEND allSolutionIds ${fullId})
	
	set(optionName "DISABLE_PROBLEM_${problemName}")
	if (${optionName})
		continue()
	endif()
	checkForAnonymousNamespace("${solutionPath}")
	set(generatedCpp ${generatedInit}/SolutionInit_${fullId}.cpp)
	configure_file(cmake/SolutionInit.cpp.in ${generatedCpp} @ONLY)
	target_sources(ContestChecker PRIVATE ${solutionPath} ${generatedCpp})
	set_source_files_properties(${generatedCpp} PROPERTIES COMPILE_FLAGS -DPROBLEM_NAMESPACE=${problemName}Details)
	
endforeach()
