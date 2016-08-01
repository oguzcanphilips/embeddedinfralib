ccola_component(util_test gmock_test)

	ccola_sources(
		TestAutoResetFunction.cpp
		TestBoundedDeque.cpp
		TestBoundedForwardList.cpp
		TestBoundedList.cpp
		TestBoundedPriorityQueue.cpp
		TestBoundedString.cpp
		TestBoundedVector.cpp
		TestCompareMembers.cpp
		TestCyclicBuffer.cpp
		TestFixedPoint.cpp
		TestFunction.cpp
		TestInterfaceConnector.cpp
		TestIntrusiveBinarySearchTree.cpp
		TestIntrusiveForwardList.cpp
		TestIntrusiveList.cpp
		TestIntrusivePriorityQueue.cpp
		TestIntrusiveSet.cpp
		TestMemoryRange.cpp
		TestObserver.cpp
		TestOptional.cpp
		TestPolymorphicVariant.cpp
		TestProxyCreator.cpp
		TestReferenceCountedSingleton.cpp
		TestSequencer.cpp
		TestSignalSlot.cpp
		TestStaticStorage.cpp
		TestUnit.cpp
		TestVariant.cpp
	)

	ccola_dependencies(
		util
		util_test_helper
	)

ccola_end_component()
