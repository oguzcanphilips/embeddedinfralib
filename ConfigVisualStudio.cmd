mkdir ..\EmbeddedReuse-Build-VisualStudio
pushd ..\EmbeddedReuse-Build-VisualStudio
cmake ../EmbeddedReuse -DCCOLA_DIR="../EmbeddedReuse/ccola" -DCCOLA_INSTALL_DIR="../EmbeddedReuse-Install" -G"Visual Studio 12 2013"
popd
