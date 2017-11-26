ccola_component(fsm csharp_executable)

	set(ccola_location "${CMAKE_CURRENT_SOURCE_DIR}/../../$<CONFIGURATION>/fsm.exe")

	ccola_csharp_project(
		fsm.csproj
		ActionsList.cs
		Config.cs
		DoubleBufferedPanel.cs
		Event.cs
		EventCollection.cs
		FormEvent.cs
		FormEvent.Designer.cs
		FormEvent.resx
		FormState.cs
		FormState.Designer.cs
		FormState.resx
		FSM.cs
		FSM.Designer.cs
		FSM.resx
		Generator.cs
		GeneratorC.cs
		GeneratorCPP.cs
		Guard.cs
		ModelChecker.cs
		OutputFile.cs
		Persistent.cs
		Program.cs
		Selectable.cs
		State.cs
		StateCollection.cs
		TransitionalState.cs
		Util.cs
	)

ccola_end_component()
