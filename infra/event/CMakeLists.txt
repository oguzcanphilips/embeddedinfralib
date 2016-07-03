ccola_component(event static_library)

	ccola_sources(
		private/ClaimableResource.cpp
		private/EventDispatcher.cpp
		private/SystemStateManager.cpp
		public/ClaimableResource.hpp
		public/EventDispatcher.hpp
		public/SystemStateManager.hpp
	)

	ccola_dependencies(
		util
	)

ccola_end_component()

ccola_subdirectories(
	test_helper
	test
)
