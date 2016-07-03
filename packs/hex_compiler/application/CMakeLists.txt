ccola_component(application win: static_library other: noncompile)

	generate_fsm(
		private/CommandLineParser.fsm
	)

	ccola_sources(
		private/BinaryObject.cpp
		private/Compiler.cpp
		private/HexOutput.cpp
		public/BinaryObject.hpp
		public/Compiler.hpp
		public/HexOutput.hpp
		public/SparseVector.hpp
	)

	ccola_dependencies(
		infra.util
		infra.stream
		hal.interfaces
	)
	
ccola_end_component()
