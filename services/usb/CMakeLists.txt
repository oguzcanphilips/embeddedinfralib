ccola_component(usb static_library)

	ccola_inheritable_include_directories(../..)
	
	ccola_sources(
		private/UsbDevice.cpp
		private/UsbEndPoint.cpp
		private/UsbInterfaceCustomHid.cpp
		private/UsbInterfaceVirtualComPort.cpp
		public/UsbDevice.hpp
		public/UsbEndPoint.hpp
		public/UsbInterface.hpp
		public/UsbInterfaceCustomHid.hpp
		public/UsbInterfaceVirtualComPort.hpp
	)

	ccola_dependencies(
		hal.interfaces
	)

ccola_end_component()
