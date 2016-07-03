ccola_component(fsm_test gmock_test)

	generate_fsm(
		TestCheckerFSM.fsm
		TestFSM.fsm
	)

	ccola_sources(
		TestFiniteStateMachine.cpp
	)

ccola_end_component()
