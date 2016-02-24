ccola_component(util_test gmock_test)

	ccola_sources(
		TestAutoResetFunction.cpp
		TestBoundedDeque.cpp
		TestBoundedPriorityQueue.cpp
		TestBoundedString.cpp
		TestBoundedVector.cpp
		TestCompare.cpp
		TestFixedPoint.cpp
		TestFunction.cpp
		TestInterfaceConnector.cpp
		TestIntrusiveBinarySearchTree.cpp
		TestIntrusiveForwardList.cpp
		TestIntrusiveList.cpp
		TestIntrusivePriorityQueue.cpp
		TestIntrusiveSet.cpp
		TestMemoryRange.cpp
		TestOptional.cpp
		TestReferenceCountedSingleton.cpp
		TestStaticStorage.cpp
		TestUnit.cpp
		TestVariant.cpp
	)

	ccola_dependencies(
		util
		util_test_helper
	)

ccola_end_component()
