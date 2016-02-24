ccola_component(util source)

	ccola_inheritable_include_directories(../..)
	ccola_sources(
		public/AutoResetFunction.hpp
		public/BitLogic.hpp
		public/BoundedDeque.hpp
		public/BoundedPriorityQueue.hpp
		public/BoundedString.hpp
		public/BoundedVector.hpp
		public/ByteRange.hpp
		public/Compare.hpp
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
		public/Optional.hpp
		public/ReallyAssert.hpp
		public/ReferenceCountedSingleton.hpp
		public/ReverseRange.hpp
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
