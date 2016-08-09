ccola_component(util static_library)

	ccola_inheritable_include_directories(../..)

	ccola_sources(
		private/CrcCcittCalculator.cpp
		private/Sequencer.cpp
		public/AutoResetFunction.hpp
		public/BitLogic.hpp
		public/BoundedDeque.hpp
		public/BoundedForwardList.hpp
		public/BoundedList.hpp
		public/BoundedPriorityQueue.hpp
		public/BoundedString.hpp
		public/BoundedVector.hpp
		public/ByteRange.hpp
		public/CompareMembers.hpp
		public/CrcCcittCalculator.hpp
		public/CyclicBuffer.hpp
		public/FixedPoint.hpp
		public/Function.hpp
		public/InterfaceConnector.hpp
		public/IntrusiveBinarySearchTree.hpp
		public/IntrusiveForwardList.hpp
		public/IntrusiveList.hpp
		public/IntrusivePriorityQueue.hpp
		public/IntrusiveSet.hpp
		public/MemoryRange.hpp
		public/Observer.hpp
		public/Optional.hpp
		public/PolymorphicVariant.hpp
		public/ProxyCreator.hpp
		public/ReallyAssert.hpp
		public/ReferenceCountedSingleton.hpp
		public/ReferenceCountedSingletons.hpp
		public/ReverseRange.hpp
		public/Sequencer.hpp
		public/SequencerIdiom.hpp
		public/StaticStorage.hpp
		public/Unit.hpp
		public/VariadicTemplates.hpp
		public/Variant.hpp
		public/VariantDetail.hpp
		public/WithStorage.hpp
	)

ccola_end_component()

ccola_subdirectories(
	test_helper
	test
)
