ccola_component(synchronous_util static_library)

	ccola_sources(
		private/SynchronousFlashRegion.cpp
		private/SynchronousFlashQuadSpi.cpp
		private/SynchronousFlashSpi.cpp
		private/SynchronousSpiMasterWithChipSelect.cpp
		public/SynchronousFlashRegion.hpp
		public/SynchronousFlashQuadSpi.hpp
		public/SynchronousFlashSpi.hpp
		public/SynchronousSpiMasterWithChipSelect.hpp
	)

	ccola_dependencies(
		hal.synchronous_interfaces
	)

ccola_end_component()
