cmake_minimum_required(VERSION 3.2.3)
include("${CCOLA_DIR}/ccola.inc")

ccola_project(EmbeddedReuse)

	ccola_subdirectories(
		external/gmock
		infra
	)

ccola_end_project()
