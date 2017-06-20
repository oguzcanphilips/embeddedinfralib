ccola_component(event static_library)

	ccola_sources(
		ClaimableResource.cpp
		EventDispatcher.cpp
		EventDispatcherWithWeakPtr.cpp
		QueueForOneReaderOneIrqWriter.cpp
		SystemStateManager.cpp
		ClaimableResource.hpp
		EventDispatcher.hpp
		EventDispatcherWithWeakPtr.hpp
		QueueForOneReaderOneIrqWriter.hpp
		SystemStateManager.hpp
	)

	ccola_dependencies(
		util
	)

ccola_end_component()

ccola_subdirectories(
	test_helper
	test
)
